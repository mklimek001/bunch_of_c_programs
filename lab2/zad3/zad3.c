#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>


void inspectFolder(char *given_folder_name){

    char* folder_path = calloc(100,sizeof(char));

    for (int i = 0; i < strlen(given_folder_name); i++) {
        folder_path[i] = given_folder_name[i];
    }

    DIR *curr_dir;
    struct dirent *dir_info;
    struct stat stats;
    curr_dir = opendir(folder_path);


    if (curr_dir != NULL) {
        while ((dir_info = readdir(curr_dir)) != NULL) {
            char curr_file_path[512];
            snprintf(curr_file_path, 512, "%s/%s", folder_path, dir_info->d_name);

            printf("%s\n",dir_info->d_name);


            if (stat(curr_file_path, &stats) == 0)
            {
                printf("    Path: %s\n", curr_file_path);

                printf("    Number of links: %ld\n", stats.st_nlink);

                if (S_ISREG(stats.st_mode)) {printf("    Type: - file\n");}
                if (S_ISDIR(stats.st_mode)) {printf("    Type: - dir\n");}
                if (S_ISCHR(stats.st_mode)) {printf("    Type: - char dev\n");}
                if (S_ISBLK(stats.st_mode)) {printf("    Type: - block dev\n");}
                if (S_ISFIFO(stats.st_mode)) {printf("    Type: - fifo\n");}
                if (S_ISLNK(stats.st_mode)) {printf("    Type: - slink\n");}
                if (S_ISSOCK(stats.st_mode)) {printf("    Type: - sock\n");}

                printf("    Size in bytes: %ld\n", stats.st_size);

                struct tm dt;

                dt = *(gmtime(&stats.st_ctime));
                printf("    Created on: %d-%d-%d %d:%d:%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
                       dt.tm_hour, dt.tm_min, dt.tm_sec);

                dt = *(gmtime(&stats.st_mtime));
                printf("    Modified on: %d-%d-%d %d:%d:%d\n", dt.tm_mday, dt.tm_mon, dt.tm_year + 1900,
                       dt.tm_hour, dt.tm_min, dt.tm_sec);
            }

            printf("\n");


        }
        closedir(curr_dir);
    }



}


int main(){
    char dir_name[100];
    printf("Give the path to the folder to inspect: ");
    scanf("%s",dir_name);

    inspectFolder(dir_name);

    return 0;
}
