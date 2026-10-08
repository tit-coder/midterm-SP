#include "ls.h"
#include <stdio.h>
#include <string.h>

void mode_to_string(mode_t mode, char *str) {
    strcpy(str, "----------");
    if (S_ISDIR(mode))  str[0] = 'd';
    else if (S_ISCHR(mode))  str[0] = 'c';
    else if (S_ISBLK(mode))  str[0] = 'b';
    else if (S_ISLNK(mode))  str[0] = 'l';
    else if (S_ISFIFO(mode)) str[0] = 'p';
    else if (S_ISSOCK(mode)) str[0] = 's';

    if (mode & S_IRUSR) str[1] = 'r';
    if (mode & S_IWUSR) str[2] = 'w';
    if (mode & S_ISUID) str[3] = (mode & S_IXUSR) ? 's' : 'S';
    else if (mode & S_IXUSR) str[3] = 'x';

    if (mode & S_IRGRP) str[4] = 'r';
    if (mode & S_IWGRP) str[5] = 'w';
    if (mode & S_ISGID) str[6] = (mode & S_IXGRP) ? 's' : 'S';
    else if (mode & S_IXGRP) str[6] = 'x';

    if (mode & S_IROTH) str[7] = 'r';
    if (mode & S_IWOTH) str[8] = 'w';
    if (mode & S_ISVTX) str[9] = (mode & S_IXOTH) ? 't' : 'T';
    else if (mode & S_IXOTH) str[9] = 'x';
}

void format_size(off_t size, char *buf, size_t buf_len) {
    if (!opt.h) {
        snprintf(buf, buf_len, "%ld", (long)size);
        return;
    }
    const char *units[] = {"B", "K", "M", "G", "T", "P"};
    int unit_idx = 0;
    double d_size = (double)size;
    while (d_size >= 1024.0 && unit_idx < 5) {
        d_size /= 1024.0;
        unit_idx++;
    }
    if (unit_idx == 0) {
        snprintf(buf, buf_len, "%ldB", (long)size);
    } else {
        snprintf(buf, buf_len, "%.1f%s", d_size, units[unit_idx]);
    }
}
