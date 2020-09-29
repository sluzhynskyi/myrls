#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <iostream>
#include "bitset"

static char *print_permissions(const struct stat *sb);

static int
display_info(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf) {
    printf("%s %2d %7jd   %-40s %d %s\n", print_permissions(sb),
           ftwbuf->level, (intmax_t) sb->st_size,
           fpath, ftwbuf->base, fpath + ftwbuf->base);
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
    std::cout << str << std::endl;
    return str;
}

int
main(int argc, char *argv[]) {
    int flags = 0;

    if (argc > 2 && strchr(argv[2], 'd') != NULL)
        flags |= FTW_DEPTH;
    if (argc > 2 && strchr(argv[2], 'p') != NULL)
        flags |= FTW_PHYS;

    if (nftw((argc < 2) ? "." : argv[1], display_info, 20, flags) == -1) {
        perror("nftw");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
