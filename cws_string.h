//和string大部分操作一样，但是当数据非常多时可以自动创建文件，不会搞爆内存
//数据多时避免对数据靠后部分过多操作，操作文件很慢
#include <string>
#include <stdio.h>
using namespace std;
#pragma once
class CWS_string;
class CWS_string_char;
class CWS_string
{
    friend class CWS_string_char;
    friend CWS_string operator+(const CWS_string &str1, const CWS_string &str2);
    friend bool operator==(const CWS_string &str1, const CWS_string &str2);
    friend bool operator!=(const CWS_string &str1, const CWS_string &str2);
    friend bool operator<(const CWS_string &str1, const CWS_string &str2);
    friend bool operator>(const CWS_string &str1, const CWS_string &str2);

public:
    CWS_string(long long max_str_len = 40000);
    CWS_string(const std::string str, long long max_str_len = 40000);
    CWS_string(const char *str, long long str_len=-1, long long max_str_len = 40000);
    CWS_string(const CWS_string &str, long long max_str_len = 40000);
    ~CWS_string();
    long long length();
    long long size();
    bool empty();
    void swap(CWS_string &str);
    void fill_value(long long pos, char str, char padding=' ');
    long long find(CWS_string str, long long pos = 0);
    CWS_string substr(long long pos, long long len = -1);
    CWS_string &erase(long long pos, long long len = -1);
    CWS_string &operator=(CWS_string str);
    CWS_string &operator+=(CWS_string str);
    std::string to_string(long long pos = 0, long long len = 40000);
    CWS_string_char operator[](long long pos);
    bool select_resource_file(CWS_string file_name);
    void print();
    void println();

private:
    std::string cws_string = "";
    std::string file_name = "";
    FILE *file = nullptr;
    CWS_string *address = this;
    bool using_file = false;
    bool delete_file = true;
    long long total_length = 0;
    long long string_length = 0;
    long long max_string_length = 40000;
    long long shift = 0;
    void set_value(long long pos, char str);
    char get_value(long long pos);
    std::string random_string(int length = 20);
    void generate_resource_file();
};
class CWS_string_char
{
public:
    CWS_string_char(CWS_string *str, long long str_pos);
    operator char();
    void fill_value(CWS_string_char str, char padding=' ');
    void fill_value(char str, char padding=' ');
    CWS_string_char &operator=(CWS_string_char s);
    CWS_string_char &operator=(char s);

private:
    CWS_string *cws_string;
    long long pos;
};
CWS_string::CWS_string(long long max_str_len)
{
    max_string_length = max_str_len;
    cws_string = "";
    string_length = 0;
    total_length = 0;
}
CWS_string::CWS_string(const std::string str, long long max_str_len)
{
    max_string_length = max_str_len;
    total_length = str.length();
    if (total_length > max_string_length)
    {
        cws_string = str.substr(0, max_string_length);
        string_length = max_string_length;
        if (!using_file)
            generate_resource_file();
        fseek(file, 0 + shift, SEEK_SET);
        fwrite(str.substr(max_string_length).c_str(), total_length - max_string_length, 1, file);
    }
    else
    {
        cws_string = str;
        string_length = total_length;
    }
}
CWS_string::CWS_string(const char *char_str, long long str_len, long long max_str_len)
{
    std::string str = "";
    if(str_len==-1)
    {
        str = char_str;
    }
    else
    {
        str = string(char_str, str_len);
    }
    max_string_length = max_str_len;
    total_length = str.length();
    if (total_length > max_string_length)
    {
        cws_string = str.substr(0, max_string_length);
        string_length = max_string_length;
        if (!using_file)
            generate_resource_file();
        fseek(file, 0 + shift, SEEK_SET);
        fwrite(str.substr(max_string_length).c_str(), total_length - max_string_length, 1, file);
    }
    else
    {
        cws_string = str;
        string_length = total_length;
    }
}
CWS_string::CWS_string(const CWS_string &str, long long max_str_len)
{
    max_string_length = max_str_len;
    total_length = str.total_length;
    long long len = total_length;
    if (str.max_string_length <= max_string_length)
    {
        cws_string = str.cws_string;
        len -= str.max_string_length;
        long long file_len = std::min(total_length, max_string_length) - str.max_string_length;
        if (total_length <= str.max_string_length)
            file_len = 0;
        if (file_len > 0)
        {
            fseek(str.file, 0 + shift, SEEK_SET);
            while (file_len > 0)
            {
                char val[5000];
                fread(val, std::min((long long)sizeof(val), file_len), 1, str.file);
                cws_string += std::string(val).substr(0, std::min((long long)sizeof(val), file_len));
                file_len -= 5000;
            }
        }
        len = total_length - max_string_length;
    }
    else
    {
        cws_string = str.cws_string.substr(0, max_string_length);
        len -= max_string_length;
        if (len > 0)
        {
            if (!using_file)
                generate_resource_file();
            fseek(file, 0 + shift, SEEK_SET);
            fwrite(str.cws_string.substr(max_string_length).c_str(), str.cws_string.length() - max_string_length, 1, file);
            len -= str.cws_string.length() - max_string_length;
            cout<<"(*(&))"<<str.cws_string.length() - max_string_length<<endl;
        }
    }
    if (len > 0)
    {
        if (!using_file)
            generate_resource_file();
        cout<<"*&()"<<std::max((unsigned long long)0, str.cws_string.length() - max_string_length) + shift<<endl;
        fseek(file, std::max((unsigned long long)0, str.cws_string.length() - max_string_length) + shift, SEEK_SET);
        fseek(str.file, 0 + shift, SEEK_SET);
        while (len > 0)
        {
            char val[5000];
            size_t val_len=fread(val, sizeof(char), std::min((long long)sizeof(val), len), str.file);
            fwrite(std::string(val,val_len).substr(0, std::min((long long)val_len, len)).c_str(), std::min((long long)sizeof(val), len), 1, file);
            len -= 5000;
        }
    }
    string_length = cws_string.length();
}
CWS_string::~CWS_string()
{
    if (using_file && this == address)
    {
        fclose(file);
        if (delete_file)
        {
            remove(file_name.c_str());
        }
    }
}
CWS_string &CWS_string::operator=(CWS_string str)
{
    total_length = str.length();
    long long len = total_length;
    if (str.max_string_length <= max_string_length)
    {
        cws_string = str.cws_string;
        len -= str.max_string_length;
        long long file_len = std::min(total_length, max_string_length) - str.max_string_length;
        if (total_length <= str.max_string_length)
            file_len = 0;
        if (file_len > 0)
        {
            fseek(str.file, 0 + shift, SEEK_SET);
            while (file_len > 0)
            {
                char val[5000];
                fread(val, std::min((long long)sizeof(val), file_len), 1, str.file);
                cws_string += std::string(val).substr(0, std::min((long long)sizeof(val), file_len));
                file_len -= 5000;
            }
        }
        len = total_length - max_string_length;
    }
    else
    {
        cws_string = str.cws_string.substr(0, max_string_length);
        len -= max_string_length;
        if (len > 0)
        {
            if (!using_file)
                generate_resource_file();
            fseek(file, 0 + shift, SEEK_SET);
            fwrite(str.cws_string.substr(max_string_length).c_str(), str.cws_string.length() - max_string_length, 1, file);
            len -= str.cws_string.length() - max_string_length;
        }
    }
    if (len > 0)
    {
        if (!using_file)
            generate_resource_file();
        fseek(file, std::max((unsigned long long)0, str.cws_string.length() - max_string_length) + shift, SEEK_SET);
        fseek(str.file, 0 + shift, SEEK_SET);
        while (len > 0)
        {
            char val[5000];
            fread(val, std::min((long long)sizeof(val), len), 1, str.file);
            fwrite(std::string(val).substr(0, std::min((long long)sizeof(val), len)).c_str(), std::min((long long)sizeof(val), len), 1, file);
            len -= 5000;
        }
    }
    string_length = cws_string.length();
    return *this;
}
CWS_string &CWS_string::operator+=(CWS_string str)
{
    for (int len = 0; len < str.length(); len += 5000)
    {
        std::string string_str = str.to_string(len, 5000);
        total_length += string_str.length();
        if (total_length > max_string_length)
        {
            cws_string += string_str.substr(0, std::max((long long)0, (long long)string_str.length() - total_length + max_string_length));
            string_length = max_string_length;
            string_str = string_str.substr(std::max((long long)0, (long long)string_str.length() - total_length + max_string_length));
            if (!using_file)
                generate_resource_file();
            // fseek(file, 0, SEEK_END);
            fwrite(string_str.c_str(), string_str.length(), 1, file);
        }
        else
        {
            cws_string += string_str;
            string_length = total_length;
        }
    }
    return *this;
}
std::string CWS_string::to_string(long long pos, long long len)
{
    if (pos + len > total_length)
    {
        len = total_length - pos;
    }
    if (pos < max_string_length)
    {
        if (pos + len <= max_string_length)
        {
            return cws_string.substr(pos, len);
        }
        else
        {
            std::string result = cws_string.substr(pos);
            len -= max_string_length - pos;
            fseek(file, 0 + shift, SEEK_SET);
            while (len > 0)
            {
                char str[5000];
                fread(str, std::min((long long)sizeof(str), len), 1, file);
                result += std::string(str).substr(0, std::min((long long)sizeof(str), len));
                len -= 5000;
            }
            return result;
        }
    }
    std::string result = "";
    fseek(file, pos - max_string_length + shift, SEEK_SET);
    while (len > 0)
    {
        char str[5000];
        fread(str, std::min((long long)sizeof(str), len), 1, file);
        result += std::string(str).substr(0, std::min((long long)sizeof(str), len));
        len -= 5000;
    }
    return result;
}
void CWS_string::fill_value(long long pos, char str, char padding)
{
    if (pos >= total_length)
    {
        for(long long i=total_length;i<=pos;i++)
        {
            char content[1];
            content[0]=padding;
            this->operator+=(CWS_string(content, 1));
        }
    }
    if (pos >= max_string_length)
    {
        fseek(file, pos - max_string_length + shift, SEEK_SET);
        char result[1] = {str};
        fwrite(result, 1, 1, file);
    }
    else
    {
        cws_string[pos] = str;
    }
}
void CWS_string::set_value(long long pos, char str)
{
    if (pos >= total_length)
    {
        return;
    }
    else if (pos >= max_string_length)
    {
        fseek(file, pos - max_string_length + shift, SEEK_SET);
        char result[1] = {str};
        fwrite(result, 1, 1, file);
    }
    else
    {
        cws_string[pos] = str;
    }
}
char CWS_string::get_value(long long pos)
{
    if (pos >= total_length)
        return ' ';
    if (pos >= max_string_length)
    {
        fseek(file, pos - max_string_length + shift, SEEK_SET);
        char result[1];
        fread(result, 1, 1, file);
        return result[0];
    }
    return cws_string[pos];
}
CWS_string_char CWS_string::operator[](long long pos)
{
    return CWS_string_char(this, pos);
}
long long CWS_string::length()
{
    return total_length;
}
long long CWS_string::size()
{
    return total_length;
}
bool CWS_string::empty()
{
    return total_length == 0;
}
void CWS_string::swap(CWS_string &str)
{
    CWS_string temp = *this;
    *this = str;
    str = temp;
}
long long CWS_string::find(CWS_string str, long long pos)
{
    int kmp_p[str.length()];
    int kmp_j = 0;
    kmp_p[0] = 0;
    for (int i = 1; i < str.length(); i++)
    {
        while (kmp_j > 0 && str[kmp_j] != str[i])
            kmp_j = kmp_p[kmp_j - 1];
        if (str[kmp_j] == str[i])
            kmp_j++;
        kmp_p[i] = kmp_j;
    }
    kmp_j = 0;
    for (int i = pos; i < this->length(); i++)
    {
        while (kmp_j > 0 && str[kmp_j] != (*this)[i])
            kmp_j = kmp_p[kmp_j - 1];
        if (str[kmp_j] == (*this)[i])
            kmp_j++;
        if (kmp_j == str.length())
        {
            return i - str.length() + 1;
        }
    }
    return std::string::npos;
}
CWS_string CWS_string::substr(long long pos, long long len)
{
    CWS_string result(max_string_length);
    if ((len == -1) || (pos + len > total_length))
    {
        len = total_length - pos;
    }
    if (pos < max_string_length)
    {
        result += cws_string.substr(pos, std::min(len, max_string_length - pos));
        if (pos + len > max_string_length)
        {
            len = pos + len - max_string_length;
        }
        else
        {
            len = 0;
        }
    }
    if(len>0)
    {
        fseek(file, 0 + shift, SEEK_SET);
    }
    while (len > 0)
    {
        char str[5000];
        fread(str, std::min((long long)sizeof(str), len), 1, file);
        result += std::string(str).substr(0, std::min((long long)sizeof(str), len));
        len -= 5000;
    }
    return result;
}
CWS_string &CWS_string::erase(long long pos, long long len)
{
    if (pos + len > total_length)
    {
        len = total_length - pos;
    }
    this->operator=(this->substr(0, pos) + this->substr(pos + len));
    return *this;
}
std::string CWS_string::random_string(int length)
{
    static std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    std::string result;
    result.resize(length);
    for (int i = 0; i < length; i++)
        result[i] = charset[rand() % charset.length()];
    return result;
}
bool CWS_string::select_resource_file(CWS_string file_name)
{
    FILE *original_file = fopen(file_name.to_string().c_str(), "r");
    if (original_file == nullptr)
        return false;
    char buf[5000] = "";
    size_t len = sizeof(buf);
    while (len >= sizeof(buf))
    {
        len = fread(buf, 1, sizeof(buf), original_file);
        this->operator+=(string(buf).substr(0, len));
    }
    fclose(original_file);
    return true;
}
void CWS_string::generate_resource_file()
{
    while (file == nullptr)
    {
        file_name = random_string();
        file = fopen(file_name.c_str(), "w+");
    }
    using_file = true;
}
void CWS_string::print()
{
    for (int i = 0; i < this->length(); i++)
    {
        printf("%c", (char)this->operator[](i));
    }
}
void CWS_string::println()
{
    for (int i = 0; i < this->length(); i++)
    {
        printf("%c", (char)this->operator[](i));
    }
    printf("\n");
}
CWS_string operator+(const CWS_string &str1, const CWS_string &str2)
{
    CWS_string result(str1);
    result += str2;
    return result;
}
bool operator==(const CWS_string &str1, const CWS_string &str2)
{
    if (str1.total_length != str2.total_length)
        return false;
    for (int pos = 0; pos < str1.total_length; pos++)
    {
        char char1[1], char2[1];
        if (pos < str1.string_length)
            char1[0] = str1.cws_string[pos];
        else
        {
            fseek(str1.file, pos - str1.max_string_length + str1.shift, SEEK_SET);
            fread(char1, 1, 1, str1.file);
        }
        if (pos < str2.string_length)
            char2[0] = str2.cws_string[pos];
        else
        {
            fseek(str2.file, pos - str2.max_string_length + str2.shift, SEEK_SET);
            fread(char2, 1, 1, str2.file);
        }
        if (char1[0] != char2[0])
            return false;
    }
    return true;
}
bool operator>(const CWS_string &str1, const CWS_string &str2)
{
    for (int pos = 0; pos < std::min(str1.total_length, str2.total_length); pos++)
    {
        char char1[1], char2[1];
        if (pos < str1.string_length)
            char1[0] = str1.cws_string[pos];
        else
        {
            fseek(str1.file, pos - str1.max_string_length + str1.shift, SEEK_SET);
            fread(char1, 1, 1, str1.file);
        }
        if (pos < str2.string_length)
            char2[0] = str2.cws_string[pos];
        else
        {
            fseek(str2.file, pos - str2.max_string_length + str2.shift, SEEK_SET);
            fread(char2, 1, 1, str2.file);
        }
        if (char1[0] > char2[0])
            return true;
        if (char1[0] < char2[0])
            return false;
    }
    if (str1.total_length > str2.total_length)
        return true;
    return false;
}
bool operator<(const CWS_string &str1, const CWS_string &str2)
{
    for (int pos = 0; pos < std::min(str1.total_length, str2.total_length); pos++)
    {
        char char1[1], char2[1];
        if (pos < str1.string_length)
            char1[0] = str1.cws_string[pos];
        else
        {
            fseek(str1.file, pos - str1.max_string_length + str1.shift, SEEK_SET);
            fread(char1, 1, 1, str1.file);
        }
        if (pos < str2.string_length)
            char2[0] = str2.cws_string[pos];
        else
        {
            fseek(str2.file, pos - str2.max_string_length + str2.shift, SEEK_SET);
            fread(char2, 1, 1, str2.file);
        }
        if (char1[0] < char2[0])
            return true;
        if (char1[0] > char2[0])
            return false;
    }
    if (str1.total_length < str2.total_length)
        return true;
    return false;
}
bool operator!=(const CWS_string &str1, const CWS_string &str2)
{
    return !(str1 == str2);
}
CWS_string_char::CWS_string_char(CWS_string *str, long long str_pos)
{
    cws_string = str;
    pos = str_pos;
}
CWS_string_char::operator char()
{
    return cws_string->get_value(pos);
}
CWS_string_char &CWS_string_char::operator=(CWS_string_char s)
{
    cws_string->set_value(pos, s);
    return *this;
}
CWS_string_char &CWS_string_char::operator=(char s)
{
    cws_string->set_value(pos, s);
    return *this;
}
void CWS_string_char::fill_value(CWS_string_char str, char padding)
{
    cws_string->fill_value(pos,str,padding);
}
void CWS_string_char::fill_value(char str, char padding)
{
    cws_string->fill_value(pos,str,padding);
}