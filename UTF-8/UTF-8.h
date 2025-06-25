#ifndef _UTF_8_UTILS_H
#define _UTF_8_UTILS_H

#include <string>

/*
    Copyright 2014, Jason Kozak
    Copyright 2019, Tarmo Pikaro
    https://github.com/tapika/cutf

    Based on UTF8-CPP: Copyright 2006, Nemanja Trifunovicm
*/

std::wstring Utf8ToWide(const std::string& str);
std::string  WideToUtf8(const std::wstring& wstr);

#endif // _UTF_8_UTILS_H
