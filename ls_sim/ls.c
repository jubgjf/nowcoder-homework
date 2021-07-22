#include <dirent.h>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/**
 * @brief 按照 `ls -l` 的格式输出指定文件的 stat 信息
 *
 * @param filename 文件路径，要求是绝对路径
 * @return int 若成功找到文件则返回 1；
 *             找不到文件或出现其他错误则返回 0，并设置 errno
 */
int print_file_stat(const char *filename);

/**
 * @brief 按照 `ls -l` 的格式，输出指定目录中所有文件的 stat 信息
 *
 * @param dirname 目录路径，要求必须是以 `/` 结尾的绝对路径
 * @return int 若成功找到目录则返回 1；
 *             找不到目录或出现其他错误则返回 0，并设置 errno
 */
int print_dir(const char *dirname);

int main(int argc, char **argv) {
    if (argc == 1) {
        // 直接运行 ls，默认输出当前目录下所有文件的信息
        if (!print_dir("./")) {
            perror("ls");
        }
    } else {
        // ls 加参数，指定目录或文件
        for (int i = 1; i < argc; i++) {
            if (!print_dir(argv[i])) {
                if (!print_file_stat(argv[i])) {
                    perror("ls");
                } else {
                    printf("%s\n", argv[i]);
                }
            }
        }
    }

    return 0;
}

int print_file_stat(const char *filename) {
    struct stat statbuf;

    if (!stat(filename, &statbuf)) {
        // 权限
        printf((S_ISDIR(statbuf.st_mode)) ? "d" : "-");
        printf((statbuf.st_mode & S_IRUSR) ? "r" : "-");
        printf((statbuf.st_mode & S_IWUSR) ? "w" : "-");
        printf((statbuf.st_mode & S_IXUSR) ? "x" : "-");
        printf((statbuf.st_mode & S_IRGRP) ? "r" : "-");
        printf((statbuf.st_mode & S_IWGRP) ? "w" : "-");
        printf((statbuf.st_mode & S_IXGRP) ? "x" : "-");
        printf((statbuf.st_mode & S_IROTH) ? "r" : "-");
        printf((statbuf.st_mode & S_IWOTH) ? "w" : "-");
        printf((statbuf.st_mode & S_IXOTH) ? "x" : "-");

        // 硬链接数
        printf(" %ld", statbuf.st_nlink);

        // 用户组
        struct group *grp;
        grp = getgrgid(statbuf.st_gid);
        printf(" %s", grp->gr_name);

        // 用户名
        struct passwd *pws;
        pws = getpwuid(statbuf.st_uid);
        printf(" %s", pws->pw_name);

        // 文件大小
        printf(" %5ld", statbuf.st_size);

        // 修改时间
        char mtime[80];
        time_t time = statbuf.st_mtime;
        struct tm localtime;
        localtime_r(&time, &localtime);
        strftime(mtime, sizeof(mtime), "%b %d %H:%M", &localtime);
        printf(" %s ", mtime);

        return 1;
    }

    return 0;
}

int print_dir(const char *dirname) {
    DIR *dir = opendir(dirname);

    if (!dir) {
        return 0;
    }

    printf("%s:\n", dirname);

    struct dirent *ptr;
    while ((ptr = readdir(dir)) != NULL) {
        char *filename = ptr->d_name;
        if (strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0) {
            continue;
        }

        char full_filename[100] = {"\0"};
        strcat(full_filename, dirname);
        strcat(full_filename, filename);

        print_file_stat(full_filename);
        printf("%s\n", filename);
    }

    printf("\n");

    closedir(dir);

    return 1;
}
