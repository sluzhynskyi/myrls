#include <ftw.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <iostream>
#include "bitset"
#include <unistd.h>
#include <vector>
#include <string>
#include "algorithm"

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#define GetCurrentDir getcwd
#endif
using std::vector;
using std::string;
using std::cout;
using std::endl;

struct File {
    string fpath;
    struct stat sb;
    int tflag;
    struct FTW ftwbuf;

    bool operator<(const File &other) const {
        if (ftwbuf.level == other.ftwbuf.level) {
            return fpath.substr(0, ftwbuf.base) < other.fpath.substr(0, other.ftwbuf.base);
        } else {
            return ftwbuf.level < other.ftwbuf.level;
        }
    }
};

vector<File> files;

static char *print_permissions(const struct stat *sb);

void display_info(vector<File> &v) {
    std::sort(v.begin(), v.end());
    for (size_t i = 1; i < v.size(); ++i) {
        auto f = v[i];
        if (v[i - 1] < v[i]) {
            printf("%s:\n", v[i].fpath.substr(0, v[i].ftwbuf.base).c_str());
        }
        char timestr[30];
        strftime(timestr, sizeof(timestr), " %Y-%m-%d %H:%M:%S ", localtime(&f.sb.st_mtim.tv_sec));

        char special_file_type = '\0';
        if (S_ISDIR(f.sb.st_mode))
            special_file_type = '/';
        else if (f.sb.st_mode & S_IXUSR)
            special_file_type = '*';
        else if (S_ISLNK(f.sb.st_mode))
            special_file_type = '@';
        else if (S_ISFIFO(f.sb.st_mode))
            special_file_type = '|';
        else if (S_ISSOCK(f.sb.st_mode))
            special_file_type = '=';
        else if (S_ISREG(f.sb.st_mode) == 0)
            special_file_type = '?';
        printf("%s %s %9ld %s %c%s\n",
               print_permissions(&f.sb),
               getlogin(),
               (intmax_t) f.sb.st_size,
               timestr,
               special_file_type,
               f.fpath.substr(f.ftwbuf.base).c_str());
    }
}

static int save_to_vector(const char *fpath, const struct stat *sb, int tflag, struct FTW *ftwbuf) {

    if (strcmp(fpath, ".") == 0 || strcmp(fpath, "..") == 0) {
        return 0; // Skip . and ..
    }
    if (access(fpath, R_OK)) {
        std::cerr << "Error: file or directory is not readable " << std::endl;
        return 0;
    }
    File f = {fpath, *sb, tflag, *ftwbuf};
    files.push_back(f);
    return 0;           /* To tell nftw() to continue */
}

std::string get_current_dir() {
    char buff[FILENAME_MAX]; //create string buffer to hold path
    GetCurrentDir(buff, FILENAME_MAX);
    string current_working_dir(buff);
    return current_working_dir;
}

char *print_permissions(const struct stat *sb) {
    char *str = static_cast<char *>(malloc(10));
    str[0] = (sb->st_mode & S_IRUSR) ? 'r' : '-';
    str[1] = (sb->st_mode & S_IWUSR) ? 'w' : '-';
    str[2] = (sb->st_mode & S_IXUSR) ? 'x' : '-';
    str[3] = (sb->st_mode & S_IRGRP) ? 'r' : '-';
    str[4] = (sb->st_mode & S_IWGRP) ? 'w' : '-';
    str[5] = (sb->st_mode & S_IXGRP) ? 'x' : '-';
    str[6] = (sb->st_mode & S_IROTH) ? 'r' : '-';
    str[7] = (sb->st_mode & S_IWOTH) ? 'w' : '-';
    str[8] = (sb->st_mode & S_IXOTH) ? 'x' : '-';
    str[9] = '\0';
    return str;
}

int main(int argc, char *argv[]) {
    int flags = FTW_PHYS;
    if (argc > 2) {
        std::cerr << "Error: Received more than one argument" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (nftw((argc < 2) ? get_current_dir().c_str() : argv[1], save_to_vector, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }
    display_info(files);

    exit(EXIT_SUCCESS);
}

