#pragma once

#include <string>
#ifdef _WIN32
#include <windows.h>
#else
#include <dirent.h>
#endif

namespace glob {

class glob {
   public:
    glob(const std::string &pattern);
    ~glob();

    operator bool() const { return is_valid(); }

    void open(const std::string &pattern);
    void close();

    std::string current_match() const;
    bool next();
    bool is_valid() const;

   private:
    glob(const glob &) = delete;
    void operator=(const glob &) = delete;

   private:
#ifdef _WIN32
    HANDLE find_handle_;
    WIN32_FIND_DATA find_data_;
    bool ok_;
#else
    std::string file_pattern_;
    DIR *dir_;
    struct dirent *dir_entry_;
#endif
};

}  // namespace glob
