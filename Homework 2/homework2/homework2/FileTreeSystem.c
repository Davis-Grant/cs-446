//Grant Davis
//CS 446 
//Homework #2
//linux os based c program

//libraries
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <sys/stat.h>

//File Tree Structs
typedef struct fileTreeNode
{
   struct stat *stat_ptr;
   struct fileTreeNode *next;
} StatTreeNode;
typedef struct stat_list
{
   StatTreeNode *start_ptr;
} StatFileList;

//Function Declarations
off_t getMax_FileSize(StatTreeNode *statTreeNode);
int decend_Tree_Dir(StatTreeNode *statTreeNode, const char *pathname);
void upDateBin(StatTreeNode *statTreeNode, int binArray[], int binWidth);
void printHistogram(int binArray[], int num_bins, int bin_width);

//main function
int main()
{
    //file tree
    struct stat stat_struct;
    StatFileList filesList;
    StatTreeNode *statTreeNode;
    //variables
    char directoryName[50];
    int bin_width = 1024, num_bins;
    int func_ReturnValue = 0;
    off_t max_Size = 0;
    //user input
    printf("Enter directory name('./name' or current '.'): ");
    scanf("%s", directoryName);
    //input errors
    if (stat(directoryName, &stat_struct) == -1)
    {
        perror("invalid input");
        exit(EXIT_FAILURE);
    }
    if (!S_ISDIR(stat_struct.st_mode))
    {
        fputs("directory name is not a directory\n", stderr);
        exit(EXIT_FAILURE);
    }
    //create list of files
    filesList.start_ptr = malloc(sizeof(StatTreeNode));
    func_ReturnValue = decend_Tree_Dir(filesList.start_ptr, directoryName);
    if (func_ReturnValue != 0)
    {
        exit(EXIT_FAILURE);
    }
    //tree creation from list
    statTreeNode = filesList.start_ptr;
    max_Size = getMax_FileSize(statTreeNode);
    statTreeNode = filesList.start_ptr;
    //bin sorting
    num_bins = (int)ceil(max_Size / bin_width) + 1;
    int bin_array[num_bins];
    int i = 0;
    for (i = 0; i < num_bins; i++)
    {
        bin_array[i] = 0;
    }
    //histogram creation
    upDateBin(statTreeNode, bin_array, bin_width);
    statTreeNode = filesList.start_ptr;
    printHistogram(bin_array, num_bins, bin_width);
return 0;
}
//Defining Functions
void printHistogram(int bin_array[], int num_bins, int bin_width)
{
    puts("Histogram of directory tree with 1024 bin width:");
    for (int i = 0; i < num_bins; i++)
    {
        printf("%5d - %5d\t| ", bin_width * i, bin_width * (i + 1));
        for (int j = 0; j < bin_array[i]; j++)
        {
            printf("%s", "*");
        }
        printf("\n");
    }
}

off_t getMax_FileSize(StatTreeNode *statTreeNode)
{
    off_t largeFileSize = 0;
    StatTreeNode *current = statTreeNode;

    while (current->next != NULL && current->stat_ptr != NULL)
    {
        if (current->stat_ptr->st_size > largeFileSize)
        {
            largeFileSize = current->stat_ptr->st_size;
        }
        current = current->next;
    }
    return largeFileSize;
}

void upDateBin(StatTreeNode *statTreeNode, int binArray[], int binWidth)
{
    StatTreeNode *current = statTreeNode;
    off_t index = 0;

    while (current->next != NULL && current->stat_ptr != NULL)
    {
        index = current->stat_ptr->st_size / binWidth;
        binArray[index]++;
        current = current->next;
    }
}

int decend_Tree_Dir(StatTreeNode *statTreeNode, const char *directory_PathName)
{
    DIR *directoryInput;
    int dir_fd;
    int file_status = -1;
    int err_status = -1;
    struct dirent *direntPtr;
    struct stat *stat_buffer;

    if ((directoryInput = opendir(directory_PathName)) == NULL)
    {
        fprintf(stderr, "Unable to open \"%s\" directory.\n", directory_PathName);
        return errno;
    }

    if ((dir_fd = dirfd(directoryInput)) == -1)
    {
        fprintf(stderr, "Could not able to obtain directory file descriptor "
                            "of: %s\n", directory_PathName);
        return errno;
    }

    while ((direntPtr = readdir(directoryInput)) != NULL)
    {
        if (strcmp(direntPtr->d_name, ".") == 0 || strcmp(direntPtr->d_name, "..") == 0)
        {
            continue;
        }

        stat_buffer = malloc(sizeof(struct stat));
        file_status = fstatat(dir_fd, direntPtr->d_name, stat_buffer, 0);

        if (file_status == -1)
        {
            err_status = errno;
            free(stat_buffer);
            fprintf(stderr, "Unable to get the file status related to "
                        "the file \"%s\" descriptor. \n", direntPtr->d_name);
            return err_status;
        }

        switch (stat_buffer->st_mode & S_IFMT)
        {
        case S_IFREG:
            statTreeNode->next = malloc(sizeof(StatTreeNode));
            statTreeNode->stat_ptr = stat_buffer;
            statTreeNode = statTreeNode->next;
            statTreeNode->next = NULL;
            continue;

        case S_IFDIR:
        {
            char *sub_path = malloc(strlen(directory_PathName) + strlen(direntPtr->d_name) + 2);
            sprintf(sub_path, "%s/%s", directory_PathName, direntPtr->d_name);
            err_status = decend_Tree_Dir(statTreeNode, sub_path);

            if (err_status != 0)
            {
                fprintf(stderr, "Unable to open the sub_directory: %s\n", direntPtr->d_name);
                return err_status;
            }
            free(sub_path);
            break;
        }
        }
        free(stat_buffer);
    }

    if (closedir(directoryInput) == -1)
    {
        fprintf(stderr, "Could not close the \"%s\" directory.\n", directory_PathName);
        return errno;
    }
    return 0;
}

