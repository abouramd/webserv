#include "String.hpp"

void    strNcpy(char *dest, const char *src, size_t n)
{
   size_t i;

   for (i = 0; i < n ; i++)
       dest[i] = src[i];
}


String::String() : data(NULL), _size(0) {
    this->_size = 0;
}

String::String(const char* str, const size_t & size) {
    if (str) {
        _size = size;
        data = new char[size + 1];
        strNcpy(data, str, size);
        data[size] = '\0';
    } else
        data = NULL;
}

String::String(const char* str, int pos, size_t size) {
    if (str) {
        size_t  i = static_cast<size_t>(pos), k = 0;

        _size = size;
        data = new char[_size + 1];
        while (k < _size) {
            data[k] = str[i];
            i++;
            k++;
        }
        data[_size] = '\0';
    } else
        data = NULL;
}

String::~String() {
    delete[] data;
}

String::String(const String& other) {
    if (other.data != NULL) {
        this->_size = other._size;
        data = new char[other._size + 1];
        strNcpy(data, other.data, other._size);
        this->data[_size] = '\0';
    } else {
        data = NULL;
        _size = 0;
    }
}

String& String::operator= (const String& other) {
    if (this != &other) {
        delete[] data;

        if (other.data != NULL) {
            this->_size = other._size;
            data = new char[other._size + 1];
            strNcpy(data, other.data, other._size);
            data[_size] = '\0';
        } else
            data = NULL;
    }
    return *this;
}

void    String::clear() {
    delete[]    data;
    data = NULL;
    _size = 0;
}

const char* String::c_str() const {
    return data;
}

size_t  String::size() const {
    return _size;
}

size_t  String::match(const char *str) const {
    size_t  i = 0;

    if (strlen(str) > _size)
        return std::string::npos;
    while (i < strlen(str)) {
        if (static_cast<char>(std::tolower(data[i])) != str[i])
            return std::string::npos;
        i++;
    }
    return i;
}

String String::operator+ (const String& other) const {
    String  result;
    size_t  i = 0, j = 0;

    result._size = _size + other._size;
    result.data = new char[result._size + 1];
    while (i < _size) {
        result.data[i] = data[i];
        i++;
    }
    while (j < other._size) {
        result.data[i] = other.data[j];
        i++;
        j++;
    }
    result.data[result._size] = '\0';
    return result;
}

void    String::operator+= (const char c) {
    String result;

    result._size = _size + 1;
    result.data = new char[result._size + 1];
    strNcpy(result.data, data, _size);
    result.data[_size] = c;
    result.data[result._size] = '\0';
    *this = result;
}

char    String::operator[] (const size_t idx) const {
    if (idx < _size)
        return data[idx];
    throw 999;
}

bool    String::operator==(const String& other) const {
    return  (this->_size == other.size() &&
             ((data == NULL && other.data == NULL) ||
              (other.data != NULL && strcmp(data, other.data) == 0)));
}

bool    String::operator==( const char * str ) const {
  if (str == NULL) {
    if (data == NULL)
      return true;
    return false;
  }
    return  (_size == strlen(str) &&
            ((data == NULL && str == NULL) ||
            (str != NULL && strncmp(data, str, _size) == 0)));
}

bool    String::operator!=(const String& other) const {
    return !(*this == other);
}

std::ostream    &operator<< (std::ostream &obj, const String& other) {
    size_t  i = 0;

    if (other.size()) {
        while (i < other.size()) {
            obj << other[i];
            obj.flush();
            i++;
        }
    }
    return obj;
}
