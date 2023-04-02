#include "path.hpp"

#include <fstream>
#include <optional>

#include "error_message.hpp"

//--------------------------------------------------------------------------------

std::unordered_map<std::string, std::string> dom::Path::mPaths =
    dom::Path::getPaths();

//--------------------------------------------------------------------------------

std::unordered_map<std::string, std::string>
dom::Path::getPaths() noexcept
{
    std::unordered_map<std::string, std::string> result;
    std::ifstream inp("paths.path");

    if (!inp.is_open())
    {
        std::cout << "ERROR: no paths file!\n";
        exit(0);
    }

    std::string name, path;
    while (inp >> name >> path)
    {
        result[name] = path;
    }

    return result;
}

//--------------------------------------------------------------------------------

std::optional<std::string>
dom::Path::getPath(const std::string& aName) noexcept
{
    auto it = mPaths.find(aName);
    std::optional<std::string> result;
    if (it != mPaths.end())
    {
        result = it->second;
    }
    return result;
}

//--------------------------------------------------------------------------------

std::string
dom::Path::getMainPath() noexcept
{
    static std::string globalMainPath = getMainPathOnce();
    return globalMainPath;
}

//--------------------------------------------------------------------------------

std::string
dom::Path::getExecutablePath() noexcept
{
    static std::string globalExecutablePath = getExecutablePathOnce();
    return globalExecutablePath;
}

//--------------------------------------------------------------------------------

std::string
dom::Path::getMainPathOnce() noexcept
{
    std::string path = getExecutablePath();
    do path.pop_back();
    while (path.back() != '\\' && path.back() != '/');
    return path;
}

//--------------------------------------------------------------------------------

#if defined(BILL_WINDOWS)
#    include <windows.h>
#elif defined(LINUS_LINUX)
#    include <limits.h>
#    include <unistd.h>
#endif

//--------------------------------------------------------------------------------

std::string
dom::Path::getExecutablePathOnce() noexcept
{
#if defined(BILL_WINDOWS)
    CHAR buffer[MAX_PATH] = {0};
    uint8_t size          = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    for (int i = 0; i < 1; ++i)
        while (buffer[--size] != L'\\')
            ;
    buffer[size + 1] = 0;
    return std::string(buffer);
#elif defined(LINUS_LINUX)
    char buf[PATH_MAX + 1];
    if (readlink("/proc/self/exe", buf, sizeof(buf) - 1) == -1)
        WRITE_ERROR("readlink() failed");
    std::string str(buf);
    int i = str.size() - 1;
    for (int j = 0; j < 1; --i)
    {
        if (str[i] == '/') ++j;
    }
    return str.substr(0, i + 2);
    // return str.substr(0, str.rfind('/'));
#else
    return "";
#endif
}

//--------------------------------------------------------------------------------
