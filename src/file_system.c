#include "file_system.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INODES 40
// permissions
#define PERM_READ 0x1  // 0001
#define PERM_WRITE 0x2 // 0010
#define PERM_EXEC 0x4  // 0100

// Define global variables for file system state
static struct superblock super_block;
static struct inode inodes[MAX_INODES];
static struct directory_entry directory[MAX_INODES];

void fs_init()
{
    printf("File system initialized\n");
    // Initialize superblock, inodes, and directory
    super_block.fs_size = 1024;           // Example size
    super_block.block_size = 64;          // Example block size
    super_block.inode_count = MAX_INODES; // Number of inodes

    memset(inodes, 0, sizeof(inodes));
    memset(directory, 0, sizeof(directory));
}

void fs_clean_up()
{
    printf("Cleaning up unused files and directories\n");
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (inodes[i].inode_number != 0)
        {
            printf("Deleting file or directory: %s\n", inodes[i].filename);
            inodes[i].inode_number = 0;
            memset(inodes[i].filename, 0, sizeof(inodes[i].filename));
            memset(&directory[i], 0, sizeof(directory[i]));
        }
    }
}

void fs_create_file(const char *filename, uint8_t permissions)
{
    printf("File '%s' created\n", filename);
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (inodes[i].inode_number == 0)
        {
            inodes[i].inode_number = i + 1;
            inodes[i].file_type = 0;             // Regular file
            inodes[i].permissions = permissions; // Set permissions
            strcpy(inodes[i].filename, filename);
            strcpy(directory[i].name, filename);
            directory[i].inode = &inodes[i];
            break;
        }
    }
}

void fs_delete_file(const char *filename)
{
    printf("File '%s' deleted\n", filename);
    // Find the inode corresponding to the filename and mark it as free
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (strcmp(directory[i].name, filename) == 0)
        {
            inodes[i].inode_number = 0;
            directory[i].inode = NULL;
            break;
        }
    }
}

void fs_read_file(const char *filename)
{
    printf("Reading contents of file '%s'\n", filename);
    // Find the inode corresponding to the filename and read its contents
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (strcmp(directory[i].name, filename) == 0)
        {
            printf("Contents: <Placeholder for actual file contents>\n");
            break;
        }
    }
}

void fs_write_file(const char *filename, const char *data)
{
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (strcmp(directory[i].name, filename) == 0)
        {
            if (inodes[i].permissions & PERM_WRITE)
            {
                printf("Writing data to file '%s'\n", filename);
                printf("Data written: %s\n", data);
                return;
            }
            else
            {
                printf("Error: Write permission denied for file '%s'\n", filename);
                return;
            }
        }
    }

    // If file does not exist, create it
    fs_create_file(filename, PERM_READ | PERM_WRITE);
    printf("Writing data to file '%s'\n", filename);
    printf("Data written: %s\n", data);
}

void fs_create_directory(const char *dirname, uint8_t permissions)
{
    printf("Directory '%s' created\n", dirname);
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (inodes[i].inode_number == 0)
        {
            inodes[i].inode_number = i + 1;
            inodes[i].file_type = 1;             // Directory
            inodes[i].permissions = permissions; // Set permissions
            strcpy(inodes[i].filename, dirname);
            strcpy(directory[i].name, dirname);
            directory[i].inode = &inodes[i];
            break;
        }
    }
}
void fs_delete_directory(const char *dirname)
{
    printf("Directory '%s' deleted\n", dirname);
    // Find the directory inode and mark it as free
    for (uint32_t i = 0; i < super_block.inode_count; i++)
    {
        if (strcmp(directory[i].name, dirname) == 0)
        {
            inodes[i].inode_number = 0;
            directory[i].inode = NULL;
            break;
        }
    }
}
