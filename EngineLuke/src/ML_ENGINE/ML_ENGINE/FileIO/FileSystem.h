#include <Windows.h>
#include <string>
#include <vector>


std::vector<std::string> GetFilesNameFromDirectory(std::string directory);

std::string OpenFile(char* filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr);

std::string SaveFile(char* filter = "All Files (*.*)\0*.*\0", HWND owner = nullptr);

std::string GetFilenameFromPath(std::string filepath);

std::string RemoveFileExtension(const std::string& file);

std::string RemoveFileExtensionPath(const std::string& file);

std::string GetFileExtension(const std::string& file);
