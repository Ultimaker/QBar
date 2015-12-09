#ifndef DBUS_PRINTER_H
#define DBUS_PRINTER_H

#include <functional>
#include <string>
#include <map>
#include <vector>

// Forward declaration.
namespace DBus
{
    class Proxy;
}

#include "System/Variant.h"
/*!
 * Wrapper class around the griffin.printer DBus service.
 */
class DBusPrinter
{
private:
    static DBusPrinter* instance;
public:
    static DBusPrinter* getInstance();

    enum ErrorLevel
    {
        EMERGENCY   = 0,
        ALERT       = 1,
        CRITICAL    = 2,
        ERROR       = 3,
        WARNING     = 4,
        NOTICE      = 5,
        INFO        = 6,
        DEBUG       = 7,
        NO_ERROR    = 255
    };
    
    typedef std::function<void(std::string)> callback_with_key_t;
    typedef std::function<void()> callback_t;
    typedef std::function<void(ErrorLevel level, int code, std::string message)> callback_error_t;

private:
    DBus::Proxy* proxy;
    
    std::map<std::string, std::map<std::string, Variant> > metadata_cache;
    std::map<std::string, uint64_t > metadata_cache_time;
    
    std::map<std::string, std::vector<std::string> > active_procedures;
    std::map<std::string, std::vector<callback_with_key_t> > procedure_start_callbacks;
    std::map<std::string, std::vector<callback_with_key_t> > procedure_step_callbacks;
    std::map<std::string, std::vector<callback_t> > procedure_finished_callbacks;

    DBusPrinter();
public:
    /*!
     * Start a procedure by key.
     * @param key Key of the procedure to be started.
     * @return bool to indicate succes of call.
     */
    bool startProcedure(std::string key);

    /*!
     * Start a procedure by key.
     * @param key Key of the procedure to be started.
     * @param parameters key value map with parameters (string, string)
     * @return bool to indicate succes of call.
     */
    bool startProcedure(std::string key, std::map<std::string, std::string> parameters);
    
    /*!
     * Send an (already running) procedure a message.
     * For example, you can send the PRINT procedure and ABORT message.
     * @param key Key of the procedure to target.
     * @param message Key of message to be sent.
     * @return bool to indicate succes of call.
     */
    bool messageProcedure(std::string key, std::string message);

    /*!
     * Attach callback for when procedure is started
     * @param key Key of the procedure listened for.
     * @param callback_with_key_t
     */
    void attachStartProcedure(std::string key, callback_with_key_t);

    /*!
     * Attach callback for when procedure reaches next step.
     * @param key Key of the procedure listened for.
     * @param callback_with_key_t
     */
    void attachNextStepProcedure(std::string key, callback_with_key_t);

    /*!
     * Attach callback for when procedure finishes.
     * @param key Key of the procedure listened for.
     * @param callback_t
     */
    void attachFinishedProcedure(std::string key, callback_t);
    
    /*!
     * Get the metadata of a procedure.
     * This data is cached so the rest of the code does not have to deal with requesting data every X time.
     * @param key Key of the procedure listened for.
     * @param cache_time How old the cache may be for it to be valid. In milliseconds. Default is 500
     * @return map with string Variant key value pairs.
     */
    std::map<std::string, Variant> getMetaData(std::string key, uint64_t cache_time = 500);

    /*!
     * Attach an error callback.
     * This callback will directly be called with the current error state to ensure there is consistant information about the current error.
     * @param callback_error_t
     */
    void attachError(callback_error_t);
    
    /*!
     * Read the currently active procedures from the printer service, and call the appropiate callbacks.
     * Should be called once at startup to make sure the display has a consistant state.
     */
    void updateInitialActiveProcedures();
};

#endif//DBUS_PRINTER_H
