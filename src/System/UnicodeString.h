#ifndef UNICODE_STRING_H
#define UNICODE_STRING_H

#include <string.h>
#include <stdlib.h>
#include <string>
#include <limits>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>

/*
    The improved string class. while this class is not always the most efficient in terms of execution speed.
    It does provide a lot of extra functions which makes it easier to work with strings.
    It implements the same API as python strings where possible. However, python strings are immutable, while C++ strings are mutable.
*/

#define _WHITESPACE " \n\r\t"
class string : public std::u32string
{
public:
    string();
    string(const std::string& str);
    string(const std::u32string& str);
    string(const char* str);
    string(const char* str, int length);

    string(const char c);

    string(const int nr);

    string(const unsigned int nr);

    string(const float nr, int decimals = 2);

    static string hex(const int nr);

    /*
        substr works the same as the [start:end] operator in python, allowing negative indexes to get the back of the string.
        It is also garanteed to be safe. So if you request an out of range index, you will get an empty string.
    */
    string substr(const int pos = 0, const int endpos = std::numeric_limits<int>::max()) const;

    string operator*(const int count);

    /*
        Return a copy of the string S with only its first character capitalized.
    */
    string capitalize();

    /*
        Return S centered in a string of length width. Padding is done using the specified fill character (default is a space)
    */
    string center(const int width, const char fillchar=' ') const;

    /*
        Return the number of non-overlapping occurrences of substring sub in
        string S[start:end].  Optional arguments start and end are interpreted
        as in slice notation.
    */
    int count(const string sub) const;

    /*
        Return True if S ends with the specified suffix, False otherwise.
        With optional start, test S beginning at that position.
        With optional end, stop comparing S at that position.
    */
    bool endswith(const string suffix) const;

    /*
        Return a copy of S where all tab characters are expanded using spaces.
        If tabsize is not given, a tab size of 8 characters is assumed.
    */
    string expandtabs(const int tabsize=8) const;

    /*
        Return the lowest index in S where substring sub is found,
        such that sub is contained within s[start:end].  Optional
        arguments start and end are interpreted as in slice notation.
    */
    int find(const string sub, int start=0) const;

    /*
        Return a formatted version of S
        The vector variant requires {number} tags to index the vector
        The map variant requires {key} tags
    */
    string format(const std::vector<string>) const;
    string formatMap(const std::map<string, string>) const;

    /*
        Return True if all characters in S are alphanumeric
        and there is at least one character in S, False otherwise.
    */
    bool isalnum() const;

    /*
        Return True if all characters in S are alphabetic
        and there is at least one character in S, False otherwise.
    */
    bool isalpha() const;

    /*
        Return True if all characters in S are digits
        and there is at least one character in S, False otherwise.
    */
    bool isdigit() const;

    /*
        Return True if all cased characters in S are lowercase and there is
        at least one cased character in S, False otherwise.
    */
    bool islower() const;

    /*
        Return True if all characters in S are whitespace
        and there is at least one character in S, False otherwise.
    */
    bool isspace() const;


    /*
        Return True if S is a titlecased string and there is at least one
        character in S, i.e. uppercase characters may only follow uncased
        characters and lowercase characters only cased ones. Return False
        otherwise.
    */
    bool istitle() const;

    /*
        Return True if all cased characters in S are uppercase and there is
        at least one cased character in S, False otherwise.
    */
    bool isupper() const;


    /*
        Return a string which is the concatenation of the strings in the
        iterable.  The separator between elements is S.
    */
    string join(const std::vector<string> list) const;

    /*
        Return S left-justified in a string of length width. Padding is
        done using the specified fill character (default is a space).
    */
    string ljust(const int width, const char fillchar=' ') const;

    /*
        Return a copy of the string S converted to lowercase.
    */
    string lower() const;

    /*
        Return a copy of the string S with leading whitespace removed.
        If chars is given and not None, remove characters in chars instead.
    */
    string lstrip(const string chars=_WHITESPACE) const;

    /*
        Search for the separator sep in S, and return the part before it,
        the separator itand the part after it.  If the separator is not
        found, return S and two empty strings.
    */
    std::vector<string> partition(const string sep) const;

    /*
        Return a copy of string S with all occurrences of substring
        old replaced by new.  If the optional argument count is
        given, only the first count occurrences are replaced.
    */
    string replace(const string old, const string _new, const int count=-1) const;

    /*
        Return the highest index in S where substring sub is found,
        such that sub is contained within s[start:end].  Optional
        arguments start and end are interpreted as in slice notation.
    */
    int rfind(const string sub, int start=0) const;

    /*
        Return S right-justified in a string of length width. Padding is
        done using the specified fill character (default is a space)
    */
    string rjust(const int width, const char fillchar=' ') const;

    /*
        Search for the separator sep in S, starting at the end of S, and return
        the part before it, the separator itand the part after it.  If the
        separator is not found, return two empty strings and S.
    */
    std::vector<string> rpartition(const string sep) const;

    /*
        Return a list of the words in the string S, using sep as the
        delimiter string, starting at the end of the string and working
        to the front.  If maxsplit is given, at most maxsplit splits are
        done. If sep is not specified or is None, any whitespace string
        is a separator.
    */
    std::vector<string> rsplit(const string sep=_WHITESPACE, const int maxsplit=-1) const;

    /*
        Return a copy of the string S with trailing whitespace removed.
        If chars is given and not None, remove characters in chars instead.
    */
    string rstrip(const string chars=_WHITESPACE) const;

    /*
        Return a list of the words in the string S, using sep as the
        delimiter string.  If maxsplit is given, at most maxsplit
        splits are done. If sep is not specified or is None, any
        whitespace string is a separator and empty strings are removed
        from the result.
    */
    std::vector<string> split(const string sep="", int maxsplit=-1) const;

    /*
        Return a list of the lines in S, breaking at line boundaries.
        Line breaks are not included in the resulting list unless keepends
        is given and true.
    */
    std::vector<string> splitlines(const bool keepends=false) const;

    /*
        Return True if S starts with the specified prefix, False otherwise.
        With optional start, test S beginning at that position.
        With optional end, stop comparing S at that position.
    */
    bool startswith(const string prefix) const;

    /*
        Return a copy of the string S with leading and trailing
        whitespace removed.
        If chars is given and not None, remove characters in chars instead.
    */
    string strip(const string chars=_WHITESPACE) const;

    /*
        Return a copy of the string S with uppercase characters
        converted to lowercase and vice versa.
    */
    string swapcase() const;

    /*
        Return a titlecased version of S, i.e. words start with uppercase
        characters, all remaining cased characters have lowercase.
    */
    string title() const;

    /*
        Return a copy of the string S, where all characters occurring
        in the optional argument deletechars are removed, and the
        remaining characters have been mapped through the given
        translation table, which must be a string of length 256.
    */
    string translate(const string table, const string deletechars="") const;
    /*
        Return a copy of the string S converted to uppercase.
    */
    string upper() const;

    /*
        Pad a numeric string S with zeros on the left, to fill a field
        of the specified width.  The string S is never truncated.
    */
    string zfill(const int width) const;


    /* Convert this string to a number */
    float toFloat();
    int toInt();
    
    /* Convert the utf-8 string and add the resulting unicode characters to this string. */
    void convertFromUtf8(const char* str);
    
    /* Convert an unicode string to an std::string in utf-8 */
    operator std::string() const;
};
#undef _WHITESPACE

namespace std
{
    //Make a specialization of std::hash for this improved string class which uses the same std::hash as std::string.
    //  (no clue why C++ does not decides to use the specialization for the base class)
    template <> struct hash< ::string>
    {
        std::size_t operator()(const ::string& k) const
        {
            return hash<std::u32string>()(k);
        }
    };
}

#endif//STRING_H
