#include <stdio.h>
#include <string.h>
#include <string>

extern "C" {
#include <dbus-1.0/dbus/dbus.h>
}

#include "DBus.h"

namespace DBus
{

Bus* Bus::instance = nullptr;

Bus* Bus::getInstance()
{
    if (!instance)
    {
        instance = new Bus();
    }
    return instance;
}

Bus::Bus()
{
    DBusError error;
    
    dbus_error_init(&error);
    
    connection = dbus_bus_get(DBUS_BUS_SYSTEM, &error);
    if (dbus_error_is_set(&error))
    {
        fprintf(stderr, "DBUS: Connection Error (%s)\n", error.message); 
        dbus_error_free(&error); 
    }
    if (!connection)
    {
        return;
    }
}

Bus::~Bus()
{
    dbus_connection_close(connection);
}

void Bus::update()
{
    dbus_connection_read_write(connection, 0);
    DBusMessage* msg;
    while((msg = dbus_connection_pop_message(connection)) != nullptr)
    {
        //Allocate a DBus::Message object for this message, this will handle de-allocating the DBusMessage object from libdbus
        Message message(msg);
        
        if (dbus_message_get_type(msg) == DBUS_MESSAGE_TYPE_SIGNAL)
        {
            for(Proxy* proxy : proxies)
            {
                if (strcmp(dbus_message_get_path(msg), proxy->object_path) == 0 && strcmp(dbus_message_get_interface(msg), proxy->interface) == 0)
                {
                    for(auto it : proxy->signals)
                    {
                        if (strcmp(it.first, dbus_message_get_member(msg)) == 0)
                        {
                            it.second(&message);
                        }
                    }
                }
            }
        }
    }
}

Proxy::Proxy(const char* object_name, const char* object_path, const char* interface)
{
    this->object_name = object_name;
    this->object_path = object_path;
    this->interface = interface;
    
    Bus::getInstance()->proxies.push_back(this);
}

Proxy::~Proxy()
{
    Bus::getInstance()->proxies.remove(this);
}

CallPtr Proxy::createCall(const char* method)
{
    return CallPtr(new Call(this, method));
}

void Proxy::attachSignal(const char* signal, std::function<void(Message*)> callback)
{
    char buffer[1024];
    sprintf(buffer, "type='signal',sender='%s', interface='%s',member='%s', path='%s'", object_name, interface, signal, object_path);
    dbus_bus_add_match(Bus::getInstance()->connection, buffer, nullptr);
    
    signals[signal] = callback;
}

void Proxy::detachSignal(const char* signal)
{
    char buffer[1024];
    sprintf(buffer, "type='signal',sender='%s', interface='%s',member='%s', path='%s'", object_name, interface, signal, object_path);
    dbus_bus_remove_match(Bus::getInstance()->connection, buffer, nullptr);
    
    signals.erase(signal);
}

Message::Message(DBusMessage* message)
{
    args = new DBusMessageIter;
    this->message = message;
    if (message)
        can_read = (dbus_message_iter_init(message, args) != 0);
    else
        can_read = false;
}

Message::~Message()
{
    if (message)
        dbus_message_unref(message);
    delete args;
}

double Message::readDouble()
{
    if (!can_read)
    {
        return -1.0;
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_DOUBLE)
    {
        return -1.0;
    }
    double ret;
    dbus_message_iter_get_basic(args, &ret);
    can_read = (dbus_message_iter_next(args) != 0);
    return ret;
}

const char* Message::readString()
{
    if (!can_read)
    {
        return "";
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_STRING && dbus_message_iter_get_arg_type(args) != DBUS_TYPE_OBJECT_PATH)
    {
        return "";
    }
    const char* str;
    dbus_message_iter_get_basic(args, &str);
    dbus_message_iter_next(args);
    return str;
}

bool Message::readBoolean()
{
    if (!can_read)
    {
        return false;
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_BOOLEAN)
    {
        return false;
    }
    dbus_bool_t b;
    dbus_message_iter_get_basic(args, &b);
    dbus_message_iter_next(args);
    return b;
}

int Message::readInt()
{
    if (!can_read)
    {
        return -1;
    }
    switch(dbus_message_iter_get_arg_type(args))
    {
    case DBUS_TYPE_BYTE:
        {
            unsigned char b;
            dbus_message_iter_get_basic(args, &b);
            dbus_message_iter_next(args);
            return b;
        }
    case DBUS_TYPE_INT16:
        {
            dbus_int16_t i;
            dbus_message_iter_get_basic(args, &i);
            dbus_message_iter_next(args);
            return i;
        }
    case DBUS_TYPE_UINT16:
        {
            dbus_uint16_t i;
            dbus_message_iter_get_basic(args, &i);
            dbus_message_iter_next(args);
            return i;
        }
    case DBUS_TYPE_INT32:
        {
            dbus_int32_t i;
            dbus_message_iter_get_basic(args, &i);
            dbus_message_iter_next(args);
            return i;
        }
    case DBUS_TYPE_UINT32:
        {
            dbus_uint32_t i;
            dbus_message_iter_get_basic(args, &i);
            dbus_message_iter_next(args);
            return i;
        }
    }
    printf("Failed to read int: %c\n", dbus_message_iter_get_arg_type(args));
    return -1;
}

MessagePtr Message::readArray()
{
    if (!can_read)
    {
        return nullptr;
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_ARRAY)
    {
        return nullptr;
    }

    MessagePtr ret = MessagePtr(new Message(nullptr));
    dbus_message_iter_recurse(args, ret->args);
    ret->can_read = true;
    dbus_message_iter_next(args);
    return ret;
}

MessagePtr Message::readStruct()
{
    if (!can_read)
    {
        return nullptr;
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_STRUCT)
    {
        return nullptr;
    }

    MessagePtr ret = MessagePtr(new Message(nullptr));
    dbus_message_iter_recurse(args, ret->args);
    ret->can_read = true;
    dbus_message_iter_next(args);
    return ret;
}

MessagePtr Message::readDict()
{
    if (!can_read)
    {
        return nullptr;
    }
    if (dbus_message_iter_get_arg_type(args) != DBUS_TYPE_DICT_ENTRY)
    {
        return nullptr;
    }

    MessagePtr ret = MessagePtr(new Message(nullptr));
    dbus_message_iter_recurse(args, ret->args);
    ret->can_read = true;
    dbus_message_iter_next(args);
    return ret;
}

std::map<std::string, Variant> Message::readStringVariantDictionary()
{
    std::map<std::string, Variant> ret;
    
    if (!can_read)
    {
        return ret;
    }
    
    if (dbus_message_iter_get_arg_type(args) == DBUS_TYPE_ARRAY)
    {
        DBusMessageIter container;
        dbus_message_iter_recurse(args, &container);
        
        while(dbus_message_iter_get_arg_type(&container) == DBUS_TYPE_DICT_ENTRY)
        {
            DBusMessageIter entry;
            DBusMessageIter variant_iterator;
            const char* c_str;
            
            dbus_message_iter_recurse(&container, &entry);
            dbus_message_iter_get_basic(&entry, &c_str);
            dbus_message_iter_next(&entry);
            
            std::string key = c_str;
            Variant value;
            dbus_message_iter_recurse(&entry, &variant_iterator);
            switch(dbus_message_iter_get_arg_type(&variant_iterator))
            {
            case DBUS_TYPE_STRING:
            case DBUS_TYPE_SIGNATURE:
            case DBUS_TYPE_OBJECT_PATH:
                dbus_message_iter_get_basic(&variant_iterator, &c_str);
                value = std::string(c_str);
                break;
            case DBUS_TYPE_INT16:
                {
                    dbus_int16_t val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = (int)val;
                }
                break;
            case DBUS_TYPE_UINT16:
                {
                    dbus_uint16_t val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = (int)val;
                }
                break;
            case DBUS_TYPE_INT32:
                {
                    dbus_int32_t val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = (int)val;
                }
                break;
            case DBUS_TYPE_UINT32:
                {
                    dbus_uint32_t val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = (int)val;
                }
                break;
            case DBUS_TYPE_DOUBLE:
                {
                    double val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = val;
                }
                break;
            case DBUS_TYPE_BOOLEAN:
                {
                    dbus_bool_t val;
                    dbus_message_iter_get_basic(&variant_iterator, &val);
                    value = int(val);
                }
                break;
            default:
                printf("Unknown dict variant type: %s %c\n", c_str, dbus_message_iter_get_arg_type(&variant_iterator));
                break;
            }
            ret[key] = value;
            dbus_message_iter_next(&container);
        }
        dbus_message_iter_next(args);
    }
    return ret;
}

Call::Call(Proxy* proxy, const char* method)
: Message(nullptr)
{
    message = dbus_message_new_method_call(proxy->object_name, proxy->object_path, proxy->interface, method);

    dbus_message_iter_init_append(message, args);
}

Call::~Call()
{
}

void Call::param(bool b)
{
    dbus_bool_t _b = b;
    dbus_message_iter_append_basic(args, DBUS_TYPE_BOOLEAN, &_b);
}

void Call::param(int number)
{
    dbus_int32_t _number = number;
    dbus_message_iter_append_basic(args, DBUS_TYPE_UINT32, &_number);
}

void Call::param(const char* str)
{
    dbus_message_iter_append_basic(args, DBUS_TYPE_STRING, &str);
}

void Call::param(std::map<std::string, std::string> dict)
{
    DBusMessageIter container;
    dbus_message_iter_open_container(args, DBUS_TYPE_ARRAY, "{sv}", &container);

    for(auto it : dict)
    {
        const char* key = it.first.c_str();
        const char* value = it.second.c_str();
        
        DBusMessageIter entry;
        dbus_message_iter_open_container(&container, DBUS_TYPE_DICT_ENTRY, nullptr, &entry);
        dbus_message_iter_append_basic(&entry, DBUS_TYPE_STRING, &key);

        {//Append variant
            DBusMessageIter variant;
            dbus_message_iter_open_container(&entry, DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING, &variant);
            dbus_message_iter_append_basic(&variant, DBUS_TYPE_STRING, &value);
            dbus_message_iter_close_container(&entry, &variant);
        }

        dbus_message_iter_close_container(&container, &entry);
    }

    dbus_message_iter_close_container(args, &container);
}

bool Call::call()
{
    DBusPendingCall* pending;
    
    // send message and get a handle for a reply
    dbus_connection_send_with_reply(Bus::getInstance()->connection, message, &pending, -1);
    dbus_connection_flush(Bus::getInstance()->connection);

    // free message
    dbus_message_unref(message);

    // block until we receive a reply
    dbus_pending_call_block(pending);

    // get the reply message
    message = dbus_pending_call_steal_reply(pending);

    // free the pending message handle
    dbus_pending_call_unref(pending);
    
    can_read = (dbus_message_iter_init(message, args) != 0);
    
    if (dbus_message_get_type(message) == DBUS_MESSAGE_TYPE_ERROR)
    {
        printf("Error on DBus call: %s\n", readString());
    }

    //printf("%s\n", dbus_message_get_signature(message));
    return true;
}

}//!namespace DBus
