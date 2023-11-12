#pragma once

#include <cstring>
#include <iostream>
#include <vector>

class String {
private:
    char*   data;
    size_t  _size;

public:
    String();
    String(const char* str, const size_t & size);
    String(const char* str, int pos, size_t size);
    ~String();
    String(const String& other);
    String& operator= (const String& other);

    void    clear();
    const char* c_str() const ;
    size_t  size() const ;
    size_t  match(const char *str) const;

    String  operator+ (const String& other) const;
    void    operator+= (const char c);
    char    operator[] (const size_t idx) const;
    bool    operator==(const String& other) const;
    bool    operator==( const char * str ) const;
    bool    operator!=(const String& other) const;
};

std::ostream    &operator<< (std::ostream &obj, const String& other);
