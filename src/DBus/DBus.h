#ifndef CPP_DBUS_H
#define CPP_DBUS_H

/***
    DBus low level interface wrapper. These classes wrap the libdbus low level API so less houskeeping needs to be done.
    They are by no means a full wrapper on all features of DBus but implement the required features for our system.
    
    They are placed in the DBus namespace to prevent name conflicts with the libdbus names.
    
    The following classes are present:
        DBus::Bus - Singleton object representing the connection to the system bus.
        DBus::Proxy - A proxy object containing information about the object name, path and interface to use to talk to dbus objects.
            Used to create calls to DBus objects, and to setup signal handling.
        DBus::Message - A message that is received from DBus, has multiple data read members to read certain types of data.
        DBus::Call - An instance of a call to the DBus, subclass of message.
            Usage:  Create it from a DBus::Proxy class, fill it with parameters, call(), read the return values by the DBus::Message members.
*/

#include <memory>
#include <list>
#include <map>

#include "System/NoCopy.h"
#include "System/Variant.h"

//Forward declarations
struct DBusConnection;
struct DBusMessage;
struct DBusMessageIter;

namespace DBus
{
class Proxy;
class Call;
class Message;

/**
    DBus object that holds the connection to the DBus server. Never used directly use the DBusProxy objects.
*/
class Bus : NoCopy
{
private:
    static Bus* instance;

    DBusConnection* connection;
    std::list<Proxy*> proxies;
    
    Bus();
public:
    static Bus* getInstance();

    ~Bus();
    
    //Run an update cycle on the DBus connection, get all new incomming messages, and possibly handle signals.
    void update();
    
    friend class Call;
    friend class Proxy;
};

/**
    Proxy object towards and DBus service. Holds the object name, path and interface name.
    Is used to create DBusCall objects which actually call the final DBus service.
*/
typedef std::shared_ptr<Call> CallPtr;
class Proxy : NoCopy
{
public:
    typedef std::function<void(Message* message)> signal_callback_t;
private:
    const char* object_name;
    const char* object_path;
    const char* interface;
    
    std::map<const char*, signal_callback_t> signals;
public:
    Proxy(const char* object_name, const char* object_path, const char* interface);
    ~Proxy();
    
    CallPtr createCall(const char* method);
    void attachSignal(const char* signal, signal_callback_t func);
    void detachSignal(const char* signal);
    
    friend class Call;
    friend class Bus;
};

typedef std::shared_ptr<Message> MessagePtr;
class Message : NoCopy
{
protected:
    DBusMessage* message;
    DBusMessageIter* args;
    bool can_read;

    Message(DBusMessage* message);
public:
    virtual ~Message();
    
    double readDouble();
    const char* readString();
    bool readBoolean();
    int readInt();
    MessagePtr readArray();
    MessagePtr readStruct();
    MessagePtr readDict();
    std::map<std::string, Variant> readStringVariantDictionary();
    
    friend class Bus;
};

class Call : public Message
{
private:
    Call(Proxy* proxy, const char* method);
public:
    virtual ~Call();

    void param(bool b);
    void param(int number);
    void param(const char* str);
    
    //Add a basic key/value dictionary, as an string, variant dictionary.
    void param(std::map<std::string, std::string> dict);
    
    bool call();
    
    friend class Proxy;
};

}

#endif//CPP_DBUS_H
