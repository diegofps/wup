#ifndef OS_HPP
#define OS_HPP

#include <wup/common/msgs.hpp>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <sys/stat.h>


namespace wup {

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

inline bool
fileExists(const char * filepath)
{
    struct stat buffer;
    return (stat (filepath, &buffer) == 0);
}

inline bool
fileDelete(const char * filepath)
{
    return remove(filepath) == 0;
}

inline void
dirCreatePath(std::string const & folderpath)
{
    system(cat("mkdir -p ", folderpath).c_str());
}

inline void
dirRemove(std::string const & folderpath)
{
    system(cat("rm -rf ", folderpath).c_str());
}

inline std::vector<std::string> &
dirListFiles(char const * path,
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
dirListFiles(std::string const & path,
             std::vector<std::string> & list)
{
    return dirListFiles(path.c_str(), list);
}

}

#endif // OS_HPP
