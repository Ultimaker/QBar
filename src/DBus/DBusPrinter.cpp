#include <stdio.h>
#include "DBusPrinter.h"
#include "System/Clock.h"
#include "DBus.h"

DBusPrinter* DBusPrinter::instance = nullptr;

DBusPrinter* DBusPrinter::getInstance()
{
    if (!instance)
    {
        instance = new DBusPrinter();
    }
    return instance;
}

DBusPrinter::DBusPrinter()
{
    proxy = new DBus::Proxy("nl.ultimaker.printer", "/nl/ultimaker/printer", "nl.ultimaker");
    proxy->attachSignal("onProcedureStart", [this](DBus::Message* message)
    {
        std::string key = message->readString();
        std::string step = message->readString();
        
        active_procedures[key].clear();
        active_procedures[key].push_back(step);
        
        for(callback_with_key_t callback : procedure_start_callbacks[key])
        {
            callback(step);
        }
    });
    proxy->attachSignal("onProcedureNextStep", [this](DBus::Message* message)
    {
        std::string key = message->readString();
        std::string step = message->readString();

        active_procedures[key].push_back(step);

        for(callback_with_key_t callback : procedure_step_callbacks[key])
        {
            callback(step);
        }
    });
    proxy->attachSignal("onProcedureFinished", [this](DBus::Message* message)
    {
        std::string key = message->readString();
        
        active_procedures.erase(key);

        for(callback_t callback : procedure_finished_callbacks[key])
        {
            callback();
        }
    });
}

void DBusPrinter::attachStartProcedure(std::string key, callback_with_key_t callback)
{
    procedure_start_callbacks[key].push_back(callback);
}

void DBusPrinter::attachNextStepProcedure(std::string key, callback_with_key_t callback)
{
    procedure_step_callbacks[key].push_back(callback);
}

void DBusPrinter::attachFinishedProcedure(std::string key, callback_t callback)
{
    procedure_finished_callbacks[key].push_back(callback);
}

bool DBusPrinter::startProcedure(std::string key)
{
    std::map<std::string, std::string> parameters;
    return startProcedure(key, parameters);
}

bool DBusPrinter::startProcedure(std::string key, std::map<std::string, std::string> parameters)
{
    DBus::CallPtr call = proxy->createCall("startProcedure");
    call->param(key.c_str());
    call->param(parameters);
    call->call();
    return call->readBoolean();
}

bool DBusPrinter::messageProcedure(std::string key, std::string message)
{
    DBus::CallPtr call = proxy->createCall("messageProcedure");
    call->param(key.c_str());
    call->param(message.c_str());
    call->call();
    return call->readBoolean();
}

std::map<std::string, Variant> DBusPrinter::getMetaData(std::string key, uint64_t cache_time)
{
    if (metadata_cache.find(key) != metadata_cache.end())
    {
        if (getMilliseconds() - metadata_cache_time[key] < cache_time)
        {
            return metadata_cache[key];
        }
    }
    std::map<std::string, Variant> ret;
    
    DBus::CallPtr call = proxy->createCall("getProcedureMetaData");
    call->param(key.c_str());
    call->call();
    ret = call->readStringVariantDictionary();
    
    metadata_cache[key] = ret;
    metadata_cache_time[key] = getMilliseconds();
    return ret;
}

void DBusPrinter::attachError(callback_error_t callback)
{
    DBus::Proxy::signal_callback_t dbus_callback = [callback](DBus::Message* message)
    {
        int level_nr = message->readInt();
        int code = message->readInt();
        std::string error_message = message->readString();
        ErrorLevel level = NO_ERROR;
        switch(level_nr)
        {
        case EMERGENCY: level = EMERGENCY; break;
        case ALERT: level = ALERT; break;
        case CRITICAL: level = CRITICAL; break;
        case ERROR: level = ERROR; break;
        case WARNING: level = WARNING; break;
        case NOTICE: level = NOTICE; break;
        case INFO: level = INFO; break;
        case DEBUG: level = DEBUG; break;
        }
        
        callback(level, code, error_message);
    };
    proxy->attachSignal("onError", dbus_callback);
    
    //Call the getError routine to get the first error state when starting the display.
    DBus::CallPtr call = proxy->createCall("getError");
    call->call();
    
    dbus_callback(call.get());
}

void DBusPrinter::updateInitialActiveProcedures()
{
    DBus::CallPtr call = proxy->createCall("getActiveProcedures");
    call->call();
    
    DBus::MessagePtr array = call->readArray();
    if (array)
    {
        DBus::MessagePtr entry;
        while((entry = array->readStruct()) != nullptr)
        {
            std::string procedure = entry->readString();
            std::string step = entry->readString();
            
            if (active_procedures.find(procedure) == active_procedures.end())
            {
                active_procedures[procedure].push_back(step);
                
                for(callback_with_key_t callback : procedure_start_callbacks[procedure])
                {
                    callback(step);
                }
                for(callback_with_key_t callback : procedure_step_callbacks[procedure])
                {
                    callback(step);
                }
            }
        }
    }
}
