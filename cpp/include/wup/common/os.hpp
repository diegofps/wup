#ifndef OS_HPP
#define OS_HPP

#include <wup/common/msgs.hpp>
#include <dirent.h>
#include <unistd.h>
#include <string>

namespace wup {
namespace os {

inline void
getEnv(char const * const envVar,
       char const * const defaultValue,
       std::string & dst)
{
    const char * const value = getenv(envVar);
    dst = value == nullptr ? defaultValue : value;
}

inline void
getHostname(std::string & dst)
{
    dst.resize(HOST_NAME_MAX);
    gethostname(&dst[0], HOST_NAME_MAX);

    size_t i = 0;
    while (i!=HOST_NAME_MAX && dst[i] != '\0') ++i;
    dst.resize(i);
}

#ifndef __ANDROID_API__
inline void
getUsername(std::string & dst)
{
    dst.resize(LOGIN_NAME_MAX);
    getlogin_r(&dst[0], LOGIN_NAME_MAX);

    size_t i = 0;
    while (i!=HOST_NAME_MAX && dst[i] != '\0') ++i;
    dst.resize(i);
}
#endif

inline void
createPath(std::string const & folderpath)
{
    system(cat("mkdir -p ", folderpath).c_str());
}

inline void
rmDir(std::string const & folderpath)
{
    system(cat("rm -rf ", folderpath).c_str());
}

inline std::vector<std::string> &
dirFiles(char const * path,
         std::vector<std::string> & list)
{
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir (path)) == NULL)
        throw WUPException(cat("Directory '", path, "' does not exists"));

    while ((ent = readdir (dir)) != NULL) {
        std::string tmp(ent->d_name);

        if (tmp == "." || tmp == "..")
            continue;

        list.push_back(cat(path, tmp));
    }

    closedir(dir);

    return list;
}

inline std::vector<std::string> &
dirFiles(std::string const & path,
         std::vector<std::string> & list)
{
    return dirFiles(path.c_str(), list);
}

}
}
#endif // OS_HPP
