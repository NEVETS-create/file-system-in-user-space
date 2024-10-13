#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <stdint.h>

// Define file system structures
struct superblock
{
    uint32_t fs_size;
    uint32_t block_size;
    uint32_t inode_count;
};

struct inode
{
    uint32_t inode_number;
    uint32_t file_type; // 0 for regular file, 1 for directory
    uint8_t permissions;
    uint32_t block_pointers[40];
    char filename[256]; // Added to store filename
};

struct directory_entry
{
    char name[256];
    struct inode *inode;
};

// Function prototypes
void fs_init();
void fs_clean_up();
void fs_create_file(const char *filename, uint8_t permissions);
void fs_delete_file(const char *filename);
void fs_read_file(const char *filename);
void fs_write_file(const char *filename, const char *data);
void fs_create_directory(const char *dirname, uint8_t permissions);
void fs_delete_directory(const char *dirname);

#endif // FILE_SYSTEM_H
