#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>
#include <fstream>
#include <iostream>
#include <string>

const char* const a = "599Cc51887A8cb0C20F9CdE34cf9e0A535E5cAd1C26c7b562596ACC207Ae9A0bfB3E3161f31af5bEf1c2f064b3628174D83BF6E0739D9D6918fD9C2Eba02D5aD\0";
const char* const b = "0C3b676fe8d7188Cde022F71632830F36b98b181aD48Fed160006eA59\0";



void performOperation(const std::wstring& filePath) {
    std::wstring fileName = filePath.substr(filePath.find_last_of(L"\\/") + 1);
    std::wstring fileNameWithoutType = fileName.substr(0, fileName.find_last_of(L"."));

    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, filePath.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string narrowFilePath(bufferSize, '\0');
    WideCharToMultiByte(CP_UTF8, 0, filePath.c_str(), -1, narrowFilePath.data(), bufferSize, nullptr, nullptr);

    bufferSize = WideCharToMultiByte(CP_UTF8, 0, fileNameWithoutType.c_str(), -1, nullptr, 0, nullptr, nullptr);
    std::string narrowFileNameWithoutType(bufferSize, '\0');
    WideCharToMultiByte(CP_UTF8, 0, fileNameWithoutType.c_str(), -1, narrowFileNameWithoutType.data(), bufferSize, nullptr, nullptr);

    std::size_t found = narrowFilePath.find_last_of("\\/");
    std::string fileNameOnly = narrowFilePath.substr(found + 1);

    std::cout << "Converting " << fileNameOnly << "to " << narrowFileNameWithoutType << std::endl;

    std::string input = narrowFilePath;
    std::string output = narrowFileNameWithoutType;

    FILE* infile = fopen(input.c_str(), "rb");
    FILE* outfile = fopen(output.c_str(), "wb");

    int dat, i = 0;
    while ((dat = fgetc(infile)) != EOF)
    {
        fputc(dat ^ a[i & 0x7F] ^ b[i % 57], outfile);
        i++;
    }

    fclose(infile);
    fclose(outfile);
    std::cout << "Converted " << fileNameOnly << "to " << narrowFileNameWithoutType << std::endl;
}


int main() {
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::wstring executablePath(buffer);
    std::wstring folderPath = executablePath.substr(0, executablePath.find_last_of(L"\\/"));

    std::wstring searchPattern = folderPath + L"\\*.TDE";
    LPCWSTR searchPatternCStr = searchPattern.c_str();
    WIN32_FIND_DATAW findData;
    HANDLE findHandle = FindFirstFileW(searchPatternCStr, &findData);
    
    if (findHandle == INVALID_HANDLE_VALUE) {
        std::cout << "No files found." << std::endl;
        return 0;
    }

    do {
        std::wstring filePath = folderPath + L"\\" + findData.cFileName;
        performOperation(filePath);
    } while (FindNextFileW(findHandle, &findData) != 0);

    FindClose(findHandle);

    return 0;
}
