#include <stdio.h>
#include "UnicodeString.h"

string::string() : std::u32string()
{
}

string::string(const std::string& str) : std::u32string()
{
    reserve(str.length());
    convertFromUtf8(str.c_str());
}

string::string(const std::u32string& str) : std::u32string(str)
{
}

string::string(const char* str) : std::u32string()
{
    reserve(strlen(str));
    convertFromUtf8(str);
}

string::string(const char* str, int length) : std::u32string()
{
    reserve(length);
    while(length > 0)
    {
        push_back(*str++);
        length--;
    }
}

string::string(const char c) : std::u32string()
{
    push_back(c);
}

string::string(const int nr) : std::u32string()
{
    std::ostringstream stream;
    stream << nr;
    convertFromUtf8(stream.str().c_str());
}

string::string(const unsigned int nr) : std::u32string()
{
    std::ostringstream stream;
    stream << nr;
    convertFromUtf8(stream.str().c_str());
}

string::string(const float nr, int decimals) : std::u32string()
{
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(decimals);
    stream << nr;
    convertFromUtf8(stream.str().c_str());
}

string string::hex(const int nr)
{
    std::ostringstream stream;
    stream << std::hex << nr;
    return stream.str().c_str();
}

/*
    substr works the same as the [start:end] operator in python, allowing negative indexes to get the back of the string.
    It is also garanteed to be safe. So if you request an out of range index, you will get an empty string.
*/
string string::substr(const int pos, const int endpos) const
{
    int start = pos;
    int end = endpos;
    int len = length();
    if (start < 0)
        start = len + start;
    if (end < 0)
        end = len + end;
    if (start < 0)
    {
        end += start;
        start = 0;
    }
    len = std::min(end, len);
    if (end <= start)
    {
        return "";
    }
    return std::u32string::substr(start, end - start);
}

string string::operator*(const int count)
{
    if (count <= 0)
        return "";
    string ret;
    for(int n=0; n<count; n++)
        ret += *this;
    return ret;
}

/*
    Return a copy of the string S with only its first character capitalized.
*/
string string::capitalize()
{
    return substr(0, 1).upper() + substr(1).lower();
}

/*
    Return S centered in a string of length width. Padding is done using the specified fill character (default is a space)
*/
string string::center(const int width, const char fillchar) const
{
    if (width < int(length()))
        return *this;
    int right = width - length();
    int left = right / 2;
    right -= left;
    return string(fillchar) * left + *this + string(fillchar) * right;
}

/*
    Return the number of non-overlapping occurrences of substring sub in
    string S[start:end].  Optional arguments start and end are interpreted
    as in slice notation.
*/
int string::count(const string sub) const
{
    if (length() < sub.length())
        return 0;
    int count = 0;
    for(unsigned int n=0; n<=length() - sub.length(); n++)
    {
        if (substr(n, n + sub.length()) == sub)
            count++;
    }
    return count;
}

/*
    Return True if S ends with the specified suffix, False otherwise.
    With optional start, test S beginning at that position.
    With optional end, stop comparing S at that position.
*/
bool string::endswith(const string suffix) const
{
    if (suffix.length() == 0)
        return true;
    return substr(-suffix.length()) == suffix;
}

/*
    Return a copy of S where all tab characters are expanded using spaces.
    If tabsize is not given, a tab size of 8 characters is assumed.
*/
string string::expandtabs(const int tabsize) const
{
    string ret = "";
    int p = 0;
    int t;
    int start = 0;
    int end = find("\r");
    if (find("\n") > -1 && (end == -1 || find("\n") < end))
        end = find("\n");
    while((t = find("\t", p)) > -1)
    {
        while(end != -1 && end < t)
        {
            start = end + 1;
            end = find("\r", start);
            if (find("\n", start) > -1 && (end == -1 || find("\n", start) < end))
                end = find("\n", start);
        }
        ret += substr(p, t) + string(" ") * (tabsize - ((t - start) % tabsize));
        p = t + 1;
    }
    ret += substr(p);
    return ret;
}

/*
    Return the lowest index in S where substring sub is found,
    such that sub is contained within s[start:end].  Optional
    arguments start and end are interpreted as in slice notation.
*/
int string::find(const string sub, int start) const
{
    if (sub.length() + start > length() || sub.length() < 1)
        return -1;
    for(unsigned int n=start; n<=length() - sub.length(); n++)
    {
        if(substr(n, n+sub.length()) == sub)
            return n;
    }
    return -1;
}

/*
    Return a formatted version of S
*/
string string::format(const std::vector<string> items) const
{
    string ret;
    
    //Reserve the target string to the current length plus the length of all the parameters.
    //Which should be a good rough estimate for the final string length.
    int itemslength = 0;
    for(const string s : items)
    {
        itemslength += s.length();
    }
    ret.reserve(length() + itemslength);
    
    //Run trough the source string, find matching brackets.
    for(unsigned int n=0; n<length(); n++)
    {
        char32_t c = at(n);
        if (c == '{')
        {
            unsigned int end = n;
            while(end < length() && at(end) != '}')
            {
                end++;
            }
            int index = substr(n + 1, end).toInt();
            if (index >= 0 && index < (int)items.size())
            {
                ret += items[index];
            }
            
            n = end;
        }
        else if (c == '\\')
        {
            n++;
            ret.push_back(at(n));
        }
        else
        {
            ret.push_back(c);
        }
    }
    return ret;
}

string string::formatMap(const std::map<string, string> mapping) const
{
    string ret;
    
    //Reserve the target string to the current length plus the length of all the parameters.
    //Which should be a good rough estimate for the final string length.
    int itemslength = 0;
    for(auto it : mapping)
    {
        itemslength += it.second.length();
    }
    ret.reserve(length() + itemslength);
    
    //Run trough the source string, find matching brackets.
    for(unsigned int n=0; n<length(); n++)
    {
        char32_t c = at(n);
        if (c == '{')
        {
            unsigned int end = n;
            while(end < length() && at(end) != '}')
            {
                end++;
            }
            string key = substr(n + 1, end);
            if (mapping.find(key) != mapping.end())
            {
                ret += mapping.find(key)->second;
            }
            
            n = end;
        }
        else if (c == '\\')
        {
            n++;
            ret.push_back(at(n));
        }
        else
        {
            ret.push_back(c);
        }
    }
    return ret;
}

/*
    Return True if all characters in S are alphanumeric
    and there is at least one character in S, False otherwise.
*/
bool string::isalnum() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if (!::isalnum((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}

/*
    Return True if all characters in S are alphabetic
    and there is at least one character in S, False otherwise.
*/
bool string::isalpha() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if (!::isalpha((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}

/*
    Return True if all characters in S are digits
    and there is at least one character in S, False otherwise.
*/
bool string::isdigit() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if (!::isdigit((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}

/*
    Return True if all cased characters in S are lowercase and there is
    at least one cased character in S, False otherwise.
*/
bool string::islower() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if ((*this)[n] == '\n')
            continue;
        if (!::islower((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}

/*
    Return True if all characters in S are whitespace
    and there is at least one character in S, False otherwise.
*/
bool string::isspace() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if (!::isspace((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}


/*
    Return True if S is a titlecased string and there is at least one
    character in S, i.e. uppercase characters may only follow uncased
    characters and lowercase characters only cased ones. Return False
    otherwise.
*/
bool string::istitle() const
{
    int count = 0;
    bool needUpper = true;
    for(unsigned int n=0; n<length(); n++)
    {
        if ((*this)[n] == '\n')
        {
            needUpper = true;
            continue;
        }
        if (::isalpha((*this)[n]))
        {
            if (::isupper((*this)[n]) != needUpper)
                return false;
            needUpper = false;
        }else{
            needUpper = true;
        }
        count++;
    }
    return count > 0;
}

/*
    Return True if all cased characters in S are uppercase and there is
    at least one cased character in S, False otherwise.
*/
bool string::isupper() const
{
    int count = 0;
    for(unsigned int n=0; n<length(); n++)
    {
        if ((*this)[n] == '\n')
            continue;
        if (!::isupper((*this)[n]))
            return false;
        count++;
    }
    return count > 0;
}


/*
    Return a string which is the concatenation of the strings in the
    iterable.  The separator between elements is S.
*/
string string::join(const std::vector<string> list) const
{
    string ret;
    for(unsigned int n=0; n<list.size(); n++)
    {
        if (n > 0)
            ret += string(" ");
        ret += list[n];
    }
    return ret;
}

/*
    Return S left-justified in a string of length width. Padding is
    done using the specified fill character (default is a space).
*/
string string::ljust(const int width, const char fillchar) const
{
    if (int(length()) >= width)
        return *this;
    return *this + string(fillchar) * (width - length());
}

/*
    Return a copy of the string S converted to lowercase.
*/
string string::lower() const
{
    string ret = *this;
    for(unsigned int n=0; n<length(); n++)
        ret[n] = tolower(ret[n]);
    return ret;
}

/*
    Return a copy of the string S with leading whitespace removed.
    If chars is given and not None, remove characters in chars instead.
*/
string string::lstrip(const string chars) const
{
    int start=0;
    while(chars.find(substr(start, start+1)) > -1)
        start++;
    return substr(start);
}

/*
    Search for the separator sep in S, and return the part before it,
    the separator itand the part after it.  If the separator is not
    found, return S and two empty strings.
*/
//TODO: std::vector<string> string::partition(const string sep) const;

/*
    Return a copy of string S with all occurrences of substring
    old replaced by new.  If the optional argument count is
    given, only the first count occurrences are replaced.
*/
//TODO: string string::replace(const string old, const string _new, const int count=-1) const;

/*
    Return the highest index in S where substring sub is found,
    such that sub is contained within s[start:end].  Optional
    arguments start and end are interpreted as in slice notation.
*/
int string::rfind(const string sub, int start) const
{
    if (sub.length() + start > length())
        return -1;
    for(unsigned int n=length() - sub.length(); int(n)>=start; n--)
    {
        if(substr(n, n+sub.length()) == sub)
            return n;
    }
    return -1;
}

/*
    Return S right-justified in a string of length width. Padding is
    done using the specified fill character (default is a space)
*/
string string::rjust(const int width, const char fillchar) const
{
    if (int(length()) >= width)
        return *this;
    return string(fillchar) * (width - length()) + *this;
}

/*
    Search for the separator sep in S, starting at the end of S, and return
    the part before it, the separator itand the part after it.  If the
    separator is not found, return two empty strings and S.
*/
//TODO: std::vector<string> string::rpartition(const string sep) const;

/*
    Return a list of the words in the string S, using sep as the
    delimiter string, starting at the end of the string and working
    to the front.  If maxsplit is given, at most maxsplit splits are
    done. If sep is not specified or is None, any whitespace string
    is a separator.
*/
//TODO: std::vector<string> string::rsplit(const string sep, const int maxsplit) const;

/*
    Return a copy of the string S with trailing whitespace removed.
    If chars is given and not None, remove characters in chars instead.
*/
string string::rstrip(const string chars) const
{
    int end=length()-1;
    while(chars.find(substr(end, end+1)) > -1)
        end--;
    return substr(0, end+1);
}

/*
    Return a list of the words in the string S, using sep as the
    delimiter string.  If maxsplit is given, at most maxsplit
    splits are done. If sep is not specified or is None, any
    whitespace string is a separator and empty strings are removed
    from the result.
*/
std::vector<string> string::split(const string sep, int maxsplit) const
{
    std::vector<string> res;
    int start = 0;
    if(sep.length() == 0)
    {
        res = split(" ", maxsplit);
        for(unsigned int n=0; n<res.size(); n++)
        {
            if (res[n].length() < 1)
            {
                res.erase(res.begin() + n);
                n--;
            }
        }
        return res;
    }
    while(maxsplit != 0 && start < int(length()))
    {
        int offset = find(sep, start);
        if (offset < 0)
        {
            res.push_back(substr(start));
            return res;
        }
        res.push_back(substr(start, offset));
        start = offset + sep.length();
        if (maxsplit > 0)
            maxsplit--;
    }
    if (start < int(length()))
        res.push_back(substr(start));
    return res;
}

/*
    Return a list of the lines in S, breaking at line boundaries.
    Line breaks are not included in the resulting list unless keepends
    is given and true.
*/
//TODO: std::vector<string> string::splitlines(const bool keepends) const;

/*
    Return True if S starts with the specified prefix, False otherwise.
    With optional start, test S beginning at that position.
    With optional end, stop comparing S at that position.
*/
bool string::startswith(const string prefix) const
{
    return substr(0, prefix.length()) == prefix;
}

/*
    Return a copy of the string S with leading and trailing
    whitespace removed.
    If chars is given and not None, remove characters in chars instead.
*/
string string::strip(const string chars) const
{
    return lstrip(chars).rstrip(chars);
}

/*
    Return a copy of the string S with uppercase characters
    converted to lowercase and vice versa.
*/
string string::swapcase() const
{
    string ret = *this;
    for(unsigned int n=0; n<length(); n++)
        if (::isupper(ret[n]))
            ret[n] = ::tolower(ret[n]);
        else
            ret[n] = ::toupper(ret[n]);
    return ret;
}

/*
    Return a titlecased version of S, i.e. words start with uppercase
    characters, all remaining cased characters have lowercase.
*/
string string::title() const
{
    string ret = *this;
    bool needUpper = true;
    for(unsigned int n=0; n<length(); n++)
    {
        if (::isalpha(ret[n]))
        {
            if (needUpper)
                ret[n] = ::toupper(ret[n]);
            else
                ret[n] = ::tolower(ret[n]);
            needUpper = false;
        }else{
            needUpper = true;
        }
    }
    return ret;
}

/*
    Return a copy of the string S, where all characters occurring
    in the optional argument deletechars are removed, and the
    remaining characters have been mapped through the given
    translation table, which must be a string of length 256.
*/
//TODO: string string::translate(const string table, const string deletechars="") const;
/*
    Return a copy of the string S converted to uppercase.
*/
string string::upper() const
{
    string ret = *this;
    for(unsigned int n=0; n<length(); n++)
        ret[n] = toupper(ret[n]);
    return ret;
}

/*
    Pad a numeric string S with zeros on the left, to fill a field
    of the specified width.  The string S is never truncated.
*/
string string::zfill(const int width) const
{
    if (int(length()) > width)
        return *this;
    if ((*this)[0] == '-' || (*this)[0] == '+')
        return substr(0, 1) + string("0") * (width - length()) + substr(1);
    return string("0") * (width - length()) + *this;
}


/* Convert this string to a number */
float string::toFloat()
{
    std::istringstream stream(*this);
    float f = 0.0;
    stream >> f;
    return f;
}

/* Convert this string to a number */
int string::toInt()
{
    std::istringstream stream(*this);
    int i = 0;
    stream >> i;
    return i;
}

void string::convertFromUtf8(const char* str)
{
    //Convert a valid UTF-8 string to the unicode string, appending all unicode characters.
    //This assumes an valid UTF-8 input, invalid bytes at the end of the string could crash this function as it could read beyond the string boundary.
    while(*str)
    {
        char32_t c = (*str++) & 0xff;
        if ((c & 0xfe) == 0xfc) //6 bytes
        {
            c &= 0x01;
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
        }else if ((c & 0xfc) == 0xf8) //5 bytes
        {
            c &= 0x03;
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
        }else if ((c & 0xf8) == 0xf0) //4 bytes
        {
            c &= 0x07;
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
        }else if ((c & 0xf0) == 0xe0) //3 bytes
        {
            c &= 0x0f;
            c = (c << 6) | ((*str++) & 0x3f);
            c = (c << 6) | ((*str++) & 0x3f);
        }else if ((c & 0xe0) == 0xc0) //2 bytes
        {
            c &= 0x1f;
            c = (c << 6) | ((*str++) & 0x3f);
        }else{//single byte
        }
        push_back(c);
    }
}

string::operator std::string() const
{
    std::string ret;
    ret.reserve(this->length());
    for(char32_t c : *this)
    {
        if (c < 0x80)   //1 byte
        {
            ret.push_back(c);
        }
        else if (c < 0x800)   //2 bytes
        {
            ret.push_back(0xc0 | (c >> 6));
            ret.push_back(0x80 | (c & 0x3f));
        }
        else if (c < 0x10000)   //3 bytes
        {
            ret.push_back(0xe0 | (c >> 12));
            ret.push_back(0x80 | ((c >> 6) & 0x3f));
            ret.push_back(0x80 | (c & 0x3f));
        }
        else if (c < 0x200000)   //4 bytes
        {
            ret.push_back(0xf0 | (c >> 18));
            ret.push_back(0x80 | ((c >> 12) & 0x3f));
            ret.push_back(0x80 | ((c >> 6) & 0x3f));
            ret.push_back(0x80 | (c & 0x3f));
        }
        else if (c < 0x4000000)   //5 bytes
        {
            ret.push_back(0xf8 | (c >> 24));
            ret.push_back(0x80 | ((c >> 18) & 0x3f));
            ret.push_back(0x80 | ((c >> 12) & 0x3f));
            ret.push_back(0x80 | ((c >> 6) & 0x3f));
            ret.push_back(0x80 | (c & 0x3f));
        }
        else    //6 bytes
        {
            ret.push_back(0xfc | (c >> 30));
            ret.push_back(0x80 | ((c >> 24) & 0x3f));
            ret.push_back(0x80 | ((c >> 18) & 0x3f));
            ret.push_back(0x80 | ((c >> 12) & 0x3f));
            ret.push_back(0x80 | ((c >> 6) & 0x3f));
            ret.push_back(0x80 | (c & 0x3f));
        }
    }
    return ret;
}
