#include "Util.h"


std::string strEncrypt(std::string org, int key)
{
	for (std::string::iterator it = org.begin(); it != org.end(); ++it)
		*it += key;
	return org;
}

std::wstring wstrEncrypt(std::wstring org, int key)
{
	for (std::wstring::iterator it = org.begin(); it != org.end(); ++it)
		*it += key;
	return org;
}

wchar_t* wstrEncrypt(wchar_t * org, int key) {
	for (int i = 0; i < wcslen(org); i++) {
		org[i] += key;
	}
	return org;
}