//用和string差不多的方式操作文件
//避免大量不是顺序的复杂读写，操作文件比起内存慢很多
#include <string>
#include <stdio.h>
#include <unistd.h>
#include "cws_string.h"
using namespace std;
#pragma once
class CWS_file;
class CWS_file_char;
class CWS_file
{
    friend class CWS_file_char;
    friend bool operator==(CWS_file str1, CWS_file str2);
    friend bool operator==(CWS_string str1, CWS_file str2);
    friend bool operator==(CWS_file str1, CWS_string str2);
    friend bool operator!=(CWS_file str1, CWS_file str2);
    friend bool operator!=(CWS_string str1, CWS_file str2);
    friend bool operator!=(CWS_file str1, CWS_string str2);

public:
    CWS_file(int method, CWS_string file_name, int mode);
    ~CWS_file();
    long long length();
    long long size();
    bool empty();
    long long find(CWS_string str, long long pos = 0);
    CWS_file &erase(long long pos, long long len = -1);
    CWS_file &operator=(CWS_string str);
    CWS_file &operator=(CWS_file str);
    CWS_file &operator+=(CWS_string str);
    CWS_file &operator+=(CWS_file str);
    char *to_char(long long pos = 0, long long len = 40000);
    string to_std_string(long long pos = 0, long long len = 40000, long long *length = nullptr);
    CWS_string to_string(long long pos = 0, long long len = 40000, long long *length = nullptr);
    CWS_file_char operator[](long long pos);
    long long get_position();
    bool select_resource_file(int method, CWS_string file_name, int mode);
    void print();
    void println();

private:
    std::string file_name = "";
    CWS_file *address = this;
    int file_mode = -1;
    int file_method = 0;
    long long total_length = 0;
    long long position = 0;
    void set_value(long long pos, char str);
    char get_value(long long pos);
    FILE *file = nullptr;
};
class CWS_file_char
{
public:
    CWS_file_char(CWS_file *str, long long str_pos);
    operator char();
    CWS_file_char &operator=(CWS_file_char s);
    CWS_file_char &operator=(char s);

private:
    CWS_file *cws_string;
    long long pos;
};
/// @brief constructor
/// @param method 1: r+
/// @param file_name the name of the file
/// @param mode 1: 完全由本类控制; 2: 可能被其他人修改（自己处理锁）;
CWS_file::CWS_file(int method, CWS_string file_name, int mode)
{
    file_mode = mode;
    file_method = method;
    if (method == 1)
    {
        file = fopen(file_name.to_string().c_str(), "r+");
    }
    if (method == 2)
    {
        file = fopen(file_name.to_string().c_str(), "w+");
    }
    if (mode == 1)
    {
        fseek(file, 0, SEEK_END);
        total_length = ftell(file);
        position = ftell(file);
    }
}
CWS_file::~CWS_file()
{
    if (file_method != -1)
    {
        fclose(file);
    }
}
CWS_file &CWS_file::operator=(CWS_string str)
{
    if (file_mode == 1)
    {
        total_length = str.length();
        fseek(file, 0, SEEK_SET);
        for (long long len = 0; len < str.length(); len += 5000)
        {
            std::string string_str = str.to_string(len, 5000);
            total_length += string_str.length();
            fwrite(string_str.c_str(), sizeof(char), string_str.length(), file);
        }
    }
    if (file_mode == 2)
    {
        fseek(file, 0, SEEK_SET);
        for (long long len = 0; len < str.length(); len += 5000)
        {
            std::string string_str = str.to_string(len, 5000);
            fwrite(string_str.c_str(), sizeof(char), string_str.length(), file);
        }
        ftruncate(fileno(file), str.length());
    }
    position = ftell(file);
    return *this;
}
CWS_file &CWS_file::operator=(CWS_file str)
{
    if (file_method == 0 || str.file_method == 0)
    {
        return *this;
    }
    if (file_mode == 1)
    {
        total_length = 0;
        if (str.file_mode == 1)
        {
            total_length = str.total_length;
            for (long long i = 0; i < str.total_length; i += 5000)
            {
                this->operator+=(str.to_string(i, 5000));
            }
        }
        if (str.file_mode == 2)
        {
            long long pos = 0;
            CWS_string buf = str.to_string(pos, 5000);
            while (buf.length())
            {
                this->operator+=(buf);
                total_length += buf.length();
                pos += 5000;
                buf = str.to_string(pos, 5000);
            }
        }
    }
    if (file_mode == 2)
    {
        long long len_sum = 0;
        if (str.file_mode == 1)
        {
            len_sum = str.total_length;
            for (long long i = 0; i < str.total_length; i += 5000)
            {
                this->operator+=(str.to_string(i, 5000));
            }
        }
        if (str.file_mode == 2)
        {
            long long pos = 0;
            CWS_string buf = str.to_string(pos, 5000);
            while (buf.length())
            {
                this->operator+=(buf);
                len_sum += buf.length();
                pos += 5000;
                buf = str.to_string(pos, 5000);
            }
        }
        ftruncate(fileno(file), len_sum);
    }
    position = ftell(file);
    return *this;
}
CWS_file &CWS_file::operator+=(CWS_string str)
{
    if (file_mode == 1)
    {
        fseek(file, 0, SEEK_END);
        for (long long len = 0; len < str.length(); len += 5000)
        {
            std::string string_str = str.to_string(len, 5000);
            total_length += string_str.length();
            fwrite(string_str.c_str(), sizeof(char), string_str.length(), file);
        }
    }
    if (file_mode == 2)
    {
        fseek(file, 0, SEEK_END);
        for (long long len = 0; len < str.length(); len += 5000)
        {
            std::string string_str = str.to_string(len, 5000);
            fwrite(string_str.c_str(), sizeof(char), string_str.length(), file);
        }
    }
    position = ftell(file);
    return *this;
}
CWS_file &CWS_file::operator+=(CWS_file str)
{
    if (file_mode == 1)
    {
        if (str.file_mode == 1)
        {
            total_length += str.total_length;
            for (long long i = 0; i < str.total_length; i += 5000)
            {
                this->operator+=(str.to_string(i, 5000));
            }
        }
        if (str.file_mode == 2)
        {
            long long pos = 0;
            CWS_string buf = str.to_string(pos, 5000);
            while (buf.length())
            {
                this->operator+=(buf);
                total_length += buf.length();
                pos += 5000;
                buf = str.to_string(pos, 5000);
            }
        }
    }
    if (file_mode == 2)
    {
        if (str.file_mode == 1)
        {
            for (long long i = 0; i < str.total_length; i += 5000)
            {
                this->operator+=(str.to_string(i, 5000));
            }
        }
        if (str.file_mode == 2)
        {
            long long pos = 0;
            CWS_string buf = str.to_string(pos, 5000);
            while (buf.length())
            {
                this->operator+=(buf);
                pos += 5000;
                buf = str.to_string(pos, 5000);
            }
        }
    }
    position = ftell(file);
    return *this;
}
string CWS_file::to_std_string(long long pos, long long len, long long *length)
{
    string result;
    if (file_mode == 1)
    {
        if (pos >= total_length)
        {
            return "";
        }
        if (pos + len > total_length)
        {
            len = total_length - pos;
        }
        char buf[5000];
        if (position != pos)
        {
            fseek(file, pos, SEEK_SET);
        }
        for (long long i = 0; i < len; i += 5000)
        {
            size_t buf_len = fread(buf, sizeof(char), min((long long)5000, len - i), file);
            result += string(buf, buf_len);
        }
    }
    if (file_mode == 2)
    {
        char buf[5000] = "";
        fseek(file, 0, SEEK_SET);
        size_t len = sizeof(buf);
        while (len >= sizeof(buf))
        {
            len = fread(buf, 1, sizeof(buf), file);
            result += (string(buf).substr(0, len));
        }
    }
    position = ftell(file);
    if (length!=nullptr)
    {
        *length=len;
    }
    return result;
}
CWS_string CWS_file::to_string(long long pos, long long len, long long *length)
{
    CWS_string result;
    if (file_mode == 1)
    {
        if (pos >= total_length)
        {
            return "";
        }
        if (pos + len > total_length)
        {
            len = total_length - pos;
        }
        char buf[5000];
        if (position != pos)
        {
            fseek(file, pos, SEEK_SET);
        }
        for (long long i = 0; i < len; i += 5000)
        {
            size_t buf_len = fread(buf, sizeof(char), min((long long)5000, len - i), file);
            result += string(buf, buf_len);
        }
    }
    if (file_mode == 2)
    {
        char buf[5000] = "";
        fseek(file, 0, SEEK_SET);
        size_t len = sizeof(buf);
        while (len >= sizeof(buf))
        {
            len = fread(buf, 1, sizeof(buf), file);
            result += (string(buf).substr(0, len));
        }
    }
    position = ftell(file);
    if (length!=nullptr)
    {
        *length=len;
    }
    return result;
}
void CWS_file::set_value(long long pos, char str)
{
    if (file_mode == 1 && pos >= total_length)
    {
        return;
    }
    if (position != pos)
    {
        fseek(file, pos, SEEK_SET);
    }
    char result[1] = {str};
    fwrite(result, 1, 1, file);
    position = ftell(file);
}
char CWS_file::get_value(long long pos)
{
    if (file_mode == 1 && pos >= total_length)
    {
        return ' ';
    }
    if (position != pos)
    {
        fseek(file, pos, SEEK_SET);
    }
    char result[1];
    fread(result, 1, 1, file);
    position = ftell(file);
    return result[0];
}
CWS_file_char CWS_file::operator[](long long pos)
{
    return CWS_file_char(this, pos);
}
long long CWS_file::length()
{
    if (file_mode == 1)
    {
        return total_length;
    }
    if (file_mode == 2)
    {
        fseek(file, 0, SEEK_END);
        position = ftell(file);
        return ftell(file);
    }
    return 0;
}
long long CWS_file::size()
{
    return length();
}
bool CWS_file::empty()
{
    return length() == 0;
}
long long CWS_file::get_position()
{
    return position;
}
long long CWS_file::find(CWS_string str, long long pos)
{
    long long str_length = str.length();
    long long kmp_p[str_length];
    long long kmp_j = 0;
    kmp_p[0] = 0;
    for (long long i = 1; i < str_length; i++)
    {
        while (kmp_j > 0 && str[kmp_j] != str[i])
            kmp_j = kmp_p[kmp_j - 1];
        if (str[kmp_j] == str[i])
            kmp_j++;
        kmp_p[i] = kmp_j;
    }
    kmp_j = 0;
    for (long long i = pos; i < this->length(); i++)
    {
        while (kmp_j > 0 && str[kmp_j] != (*this)[i])
            kmp_j = kmp_p[kmp_j - 1];
        if (str[kmp_j] == (*this)[i])
            kmp_j++;
        if (kmp_j == str_length)
        {
            return i - str_length + 1;
        }
    }
    return std::string::npos;
}
CWS_file &CWS_file::erase(long long pos, long long len)
{
    if (file_mode == 1)
    {
        if (len == -1)
        {
            total_length = pos;
        }
        else
        {
            if (pos + len > total_length)
            {
                len = total_length - pos;
            }
            for (long long i = pos; i < total_length - len; i++)
            {
                this->set_value(i, get_value(i + len));
            }
            total_length -= len;
        }
    }
    if (file_mode == 2)
    {
        if (len == -1)
        {
            ftruncate(fileno(file), pos);
        }
        else
        {
            long long file_len = length();
            if (pos + len > file_len)
            {
                len = file_len - pos;
            }
            for (long long i = pos; i < file_len - len; i++)
            {
                this->set_value(i, get_value(i + len));
            }
            total_length -= len;
        }
    }
    return *this;
}
bool CWS_file::select_resource_file(int method, CWS_string file_name, int mode)
{
    file_mode = mode;
    file_method = method;
    if (method == 1)
    {
        file = fopen(file_name.to_string().c_str(), "w+");
    }
    if (file == nullptr)
    {
        return false;
    }
    if (mode == 1)
    {
        fseek(file, 0, SEEK_END);
        total_length = ftell(file);
        position = ftell(file);
    }
    return true;
}
void CWS_file::print()
{
    for (long long i = 0; i < this->length(); i++)
    {
        printf("%c", (char)this->operator[](i));
    }
}
void CWS_file::println()
{
    for (long long i = 0; i < this->length(); i++)
    {
        printf("%c", (char)this->operator[](i));
    }
    printf("\n");
}
bool operator==(CWS_file str1, CWS_file str2)
{
    if (str1.length() != str2.length())
        return false;
    for (long long pos = 0; pos < str1.length(); pos++)
    {
        if (str1[pos] != str2[pos])
        {
            return false;
        }
    }
    return true;
}
bool operator==(CWS_string str1, CWS_file str2)
{
    if (str1.length() != str2.length())
        return false;
    for (long long pos = 0; pos < str1.length(); pos++)
    {
        if (str1[pos] != str2[pos])
        {
            return false;
        }
    }
    return true;
}
bool operator==(CWS_file str1, CWS_string str2)
{
    if (str1.length() != str2.length())
        return false;
    for (long long pos = 0; pos < str1.length(); pos++)
    {
        if (str1[pos] != str2[pos])
        {
            return false;
        }
    }
    return true;
}
bool operator!=(CWS_file str1, CWS_file str2)
{
    return !(str1 == str2);
}
bool operator!=(CWS_string str1, CWS_file str2)
{
    return !(str1 == str2);
}
bool operator!=(CWS_file str1, CWS_string str2)
{
    return !(str1 == str2);
}
CWS_file_char::CWS_file_char(CWS_file *str, long long str_pos)
{
    cws_string = str;
    pos = str_pos;
}
CWS_file_char::operator char()
{
    return cws_string->get_value(pos);
}
CWS_file_char &CWS_file_char::operator=(CWS_file_char s)
{
    cws_string->set_value(pos, s);
    return *this;
}
CWS_file_char &CWS_file_char::operator=(char s)
{
    cws_string->set_value(pos, s);
    return *this;
}