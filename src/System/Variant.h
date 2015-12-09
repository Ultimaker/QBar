#ifndef VARIANT_H
#define VARIANT_H

#include <string>

/**
    Basic Variant class, which can store different data types, and keeps track of which data type it is.
*/
class Variant
{
public:
    enum Type
    {
        None,
        Int,
        Double,
        String
    };
private:
    Type type;
    union {
        int i;
        double d;
    } data;
    std::string string_data;
public:
    Variant();
    Variant(int i);
    Variant(double d);
    Variant(const char* str);
    Variant(const std::string str);

    bool isNone();
    bool isInt();
    bool isDouble();
    bool isString();
    
    int getInt();
    double getDouble();
    std::string getString();
    
    Variant& operator= (const std::string& str);
    Variant& operator= (const int i);
    Variant& operator= (const double d);
};

#endif//VARIANT_H
