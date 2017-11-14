#pragma once
#include <string>
#include <stdio.h>

std::string strEncrypt(std::string org, int key);
std::wstring wstrEncrypt(std::wstring org, int key);
wchar_t * wstrEncrypt(wchar_t * org, int key);


