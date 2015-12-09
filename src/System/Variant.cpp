#include "Variant.h"

Variant::Variant()
{
    type = None;
}

Variant::Variant(int i)
{
    type = Int;
    data.i = i;
}

Variant::Variant(double d)
{
    type = Double;
    data.d = d;
}

Variant::Variant(const char* str)
{
    type = String;
    string_data = str;
}

Variant::Variant(const std::string str)
{
    type = String;
    string_data = str;
}

bool Variant::isNone()
{
    return type == None;
}

bool Variant::isInt()
{
    return type == Int;
}

bool Variant::isDouble()
{
    return type == Double;
}

bool Variant::isString()
{
    return type == String;
}

int Variant::getInt()
{
    switch(type)
    {
    case Int:
        return data.i;
    case Double:
        return int(data.d);
    default:
        return 0;
    }
}

double Variant::getDouble()
{
    switch(type)
    {
    case Int:
        return double(data.i);
    case Double:
        return data.d;
    default:
        return 0.0;
    }
}

std::string Variant::getString()
{
    switch(type)
    {
    case String:
        return string_data;
    default:
        return "";
    }
}

Variant& Variant::operator= (const std::string& str)
{
    type = String;
    string_data = str;
    return *this;
}

Variant& Variant::operator= (const int i)
{
    type = Int;
    data.i = i;
    string_data = "";
    return *this;
}

Variant& Variant::operator= (const double d)
{
    type = Double;
    data.d = d;
    string_data = "";
    return *this;
}
