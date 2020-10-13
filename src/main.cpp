#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include "bitset"
#include <unistd.h>

static char *print_permissions(const struct stat *sb);

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf) {
    if (strcmp(fpath, ".") == 0 || strcmp(fpath, "..") == 0) {
        return 0; // Skip . and ..
    }
    char timestr[30];
    strftime(timestr, sizeof(timestr), " %Y-%m-%d %H:%M:%S ", localtime(&sb->st_mtim.tv_sec));
    printf(//"%s %2d %7jd   %-40s %d %s\n",
           "%s %s %9ld %s %s\n",
           print_permissions(sb),
           getlogin(),
           (intmax_t) sb->st_size,
           timestr,
           // fpath, ftwbuf->base, fpath + ftwbuf->base);
           fpath + ftwbuf->base);
    return 0;           /* To tell nftw() to continue */
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

int
main(int argc, char *argv[]) {
    int flags = 0;

    if (argc > 2) {
        std::cerr << "Error: Received more than one argument" << std::endl;
        exit(EXIT_FAILURE);
    }

    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
