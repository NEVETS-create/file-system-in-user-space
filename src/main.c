#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>
#include <dirent.h>
#include "file_system.h"

// Define constants
#define MAX_FILES 200
#define MAX_FILENAME 256

// Define base directory
#define BASE_DIR "C:/Users/user/Documents/file_system2/tests"

// Define structures
typedef struct
{
    uint32_t inode_number;
    char filename[MAX_FILENAME];
    uint32_t file_size;
    uint32_t is_directory;
    char data[1024];
} Inode;

typedef struct
{
    uint32_t total_inodes;
    uint32_t used_inodes;
    uint32_t free_inodes;
    Inode inodes[MAX_FILES];
} Superblock;

// Global superblock
Superblock superblock;

void initializeFileSystem();
void createFile(const char *filename);
void deleteFile(const char *filename);
void renameFile(const char *oldname, const char *newname);
void changeDirectory(const char *dirname);
void listFiles();
int findInodeByName(const char *filename);
void saveSuperblock();
void loadSuperblock();
void getFilePath(const char *filename, char *filePath);
void on_create_file(GtkWidget *widget, gpointer data);
void on_delete_file(GtkWidget *widget, gpointer data);
void on_rename_file(GtkWidget *widget, gpointer data);
void on_list_files(GtkWidget *widget, gpointer data);
void on_write_file(GtkWidget *widget, gpointer data);
void on_read_file(GtkWidget *widget, gpointer data);
void writeToFile(const char *filename, const char *data);
void readFromFile(const char *filename, char *data, size_t data_size);
void createDirectory(const char *dirname);
void on_create_directory(GtkWidget *widget, gpointer data);

int main(int argc, char *argv[])
{
    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *hbox_create, *hbox_delete, *hbox_list;
    GtkWidget *button_create;
    GtkWidget *button_delete;
    GtkWidget *button_list;
    GtkWidget *entry_create;
    GtkWidget *entry_delete;
    GtkWidget *scrolled_window;
    GtkWidget *text_view;
    GtkTextBuffer *text_buffer;
    GtkWidget *hbox_write, *hbox_read;
    GtkWidget *entry_write_filename, *entry_write_data;
    GtkWidget *entry_read_filename, *entry_read_data;
    GtkWidget *button_write, *button_read;
    GtkWidget *hbox_create_dir;
    GtkWidget *entry_create_dir;
    GtkWidget *button_create_dir;

    gtk_init(&argc, &argv);

    initializeFileSystem(); // Initialize the file system

    // Create the main window
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "File System Simulation");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create a vertical box to hold the widgets
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Create the text view for displaying file list
    scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
    text_view = gtk_text_view_new();
    text_buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_container_add(GTK_CONTAINER(scrolled_window), text_view);

    // Create the entry and button for creating a file
    hbox_create = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_create, FALSE, FALSE, 0);
    entry_create = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_create), "Enter filename to create");
    gtk_box_pack_start(GTK_BOX(hbox_create), entry_create, TRUE, TRUE, 0);
    button_create = gtk_button_new();
    GtkWidget *image_create = gtk_image_new_from_icon_name("document-new", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_create), image_create);
    gtk_box_pack_start(GTK_BOX(hbox_create), button_create, FALSE, FALSE, 0);
    g_signal_connect(button_create, "clicked", G_CALLBACK(on_create_file), entry_create);

    // Create the entry and button for deleting a file
    hbox_delete = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_delete, FALSE, FALSE, 0);
    entry_delete = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_delete), "Enter filename to delete");
    gtk_box_pack_start(GTK_BOX(hbox_delete), entry_delete, TRUE, TRUE, 0);
    button_delete = gtk_button_new();
    GtkWidget *image_delete = gtk_image_new_from_icon_name("edit-delete", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_delete), image_delete);
    gtk_box_pack_start(GTK_BOX(hbox_delete), button_delete, FALSE, FALSE, 0);
    g_signal_connect(button_delete, "clicked", G_CALLBACK(on_delete_file), entry_delete);

    // Create the entries and button for writing to a file
    hbox_write = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_write, FALSE, FALSE, 0);
    entry_write_filename = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_write_filename), "Enter filename to write");
    gtk_box_pack_start(GTK_BOX(hbox_write), entry_write_filename, TRUE, TRUE, 0);
    entry_write_data = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_write_data), "Enter data to write");
    gtk_box_pack_start(GTK_BOX(hbox_write), entry_write_data, TRUE, TRUE, 0);
    button_write = gtk_button_new();
    GtkWidget *image_write = gtk_image_new_from_icon_name("document-save", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_write), image_write);
    gtk_box_pack_start(GTK_BOX(hbox_write), button_write, FALSE, FALSE, 0);
    GtkWidget *write_entries[2] = {entry_write_filename, entry_write_data};
    g_signal_connect(button_write, "clicked", G_CALLBACK(on_write_file), write_entries);

    // Create the entries and button for reading from a file
    hbox_read = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_read, FALSE, FALSE, 0);
    entry_read_filename = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_read_filename), "Enter filename to read");
    gtk_box_pack_start(GTK_BOX(hbox_read), entry_read_filename, TRUE, TRUE, 0);
    entry_read_data = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_read_data), "File data will appear here");
    gtk_box_pack_start(GTK_BOX(hbox_read), entry_read_data, TRUE, TRUE, 0);
    button_read = gtk_button_new();
    GtkWidget *image_read = gtk_image_new_from_icon_name("document-open", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_read), image_read);
    gtk_box_pack_start(GTK_BOX(hbox_read), button_read, FALSE, FALSE, 0);
    GtkWidget *read_entries[2] = {entry_read_filename, entry_read_data};
    g_signal_connect(button_read, "clicked", G_CALLBACK(on_read_file), read_entries);

    // Create the entry and button for creating a directory
    hbox_create_dir = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_create_dir, FALSE, FALSE, 0);
    entry_create_dir = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(entry_create_dir), "Enter directory name to create");
    gtk_box_pack_start(GTK_BOX(hbox_create_dir), entry_create_dir, TRUE, TRUE, 0);
    button_create_dir = gtk_button_new();
    GtkWidget *image_create_dir = gtk_image_new_from_icon_name("folder-new", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_create_dir), image_create_dir);
    gtk_box_pack_start(GTK_BOX(hbox_create_dir), button_create_dir, FALSE, FALSE, 0);
    g_signal_connect(button_create_dir, "clicked", G_CALLBACK(on_create_directory), entry_create_dir);

    // Create the button for listing files
    hbox_list = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(vbox), hbox_list, FALSE, FALSE, 0);
    button_list = gtk_button_new();
    GtkWidget *image_list = gtk_image_new_from_icon_name("view-list-details", GTK_ICON_SIZE_BUTTON);
    gtk_button_set_image(GTK_BUTTON(button_list), image_list);
    gtk_box_pack_start(GTK_BOX(hbox_list), button_list, FALSE, FALSE, 0);
    g_signal_connect(button_list, "clicked", G_CALLBACK(on_list_files), text_buffer);

    gtk_widget_show_all(window);
    gtk_main();

    saveSuperblock(); // Save the superblock before exiting

    return 0;
}

void initializeFileSystem()
{
    // Initialize the superblock and inodes
    superblock.total_inodes = MAX_FILES;
    superblock.used_inodes = 0;
    superblock.free_inodes = MAX_FILES;

    for (uint32_t i = 0; i < MAX_FILES; i++)
    {
        superblock.inodes[i].inode_number = i;
        strcpy(superblock.inodes[i].filename, "");
        superblock.inodes[i].file_size = 0;
        superblock.inodes[i].is_directory = 0;
    }
}

void createFile(const char *filename)
{
    // Check if filename is valid
    if (strlen(filename) == 0)
    {
        g_print("Invalid filename\n");
        return;
    }

    // Check if file already exists
    for (uint32_t i = 0; i < superblock.used_inodes; i++)
    {
        if (strcmp(superblock.inodes[i].filename, filename) == 0)
        {
            g_print("File already exists\n");
            return;
        }
    }

    // Create the file in the filesystem
    if (superblock.used_inodes < MAX_FILES)
    {
        Inode *new_inode = &superblock.inodes[superblock.used_inodes];
        strcpy(new_inode->filename, filename);
        new_inode->file_size = 0;
        new_inode->is_directory = 0;
        superblock.used_inodes++;
        superblock.free_inodes--;

        char filePath[MAX_FILENAME];
        getFilePath(filename, filePath);
        FILE *file = fopen(filePath, "w");
        if (file)
        {
            fclose(file);
        }
    }
    else
    {
        g_print("Filesystem is full\n");
    }
}

void deleteFile(const char *filename)
{
    // Find the inode of the file to delete
    int inode_index = findInodeByName(filename);
    if (inode_index == -1)
    {
        g_print("File not found\n");
        return;
    }

    // Remove the file from the filesystem
    for (uint32_t i = inode_index; i < superblock.used_inodes - 1; i++)
    {
        superblock.inodes[i] = superblock.inodes[i + 1];
    }
    superblock.used_inodes--;
    superblock.free_inodes++;

    char filePath[MAX_FILENAME];
    getFilePath(filename, filePath);
    remove(filePath);
}

void renameFile(const char *oldname, const char *newname)
{
    // Find the inode of the file to rename
    int inode_index = findInodeByName(oldname);
    if (inode_index == -1)
    {
        g_print("File not found\n");
        return;
    }

    // Rename the file
    strcpy(superblock.inodes[inode_index].filename, newname);

    char oldFilePath[MAX_FILENAME], newFilePath[MAX_FILENAME];
    getFilePath(oldname, oldFilePath);
    getFilePath(newname, newFilePath);
    rename(oldFilePath, newFilePath);
}

void changeDirectory(const char *dirname)
{
    // Change the current working directory
    _chdir(dirname);
}

void listFiles()
{
    // Print the list of files
    for (uint32_t i = 0; i < superblock.used_inodes; i++)
    {
        printf("%s\n", superblock.inodes[i].filename);
    }
}

int findInodeByName(const char *filename)
{
    // Find the inode index by filename
    for (uint32_t i = 0; i < superblock.used_inodes; i++)
    {
        if (strcmp(superblock.inodes[i].filename, filename) == 0)
        {
            return i;
        }
    }
    return -1;
}

void saveSuperblock()
{
    // Save the superblock to a file
    FILE *file = fopen(BASE_DIR "/superblock.dat", "wb");
    if (file)
    {
        fwrite(&superblock, sizeof(Superblock), 1, file);
        fclose(file);
    }
}

void loadSuperblock()
{
    // Load the superblock from a file
    FILE *file = fopen(BASE_DIR "/superblock.dat", "rb");
    if (file)
    {
        fread(&superblock, sizeof(Superblock), 1, file);
        fclose(file);
    }
    else
    {
        initializeFileSystem();
    }
}

void getFilePath(const char *filename, char *filePath)
{
    // Construct the full file path
    snprintf(filePath, MAX_FILENAME, BASE_DIR "/%s", filename);
}

void on_create_file(GtkWidget *widget, gpointer data)
{
    // Callback function for creating a file
    (void)widget; // Mark unused parameter
    const char *filename = gtk_entry_get_text(GTK_ENTRY(data));
    createFile(filename);
}

void on_delete_file(GtkWidget *widget, gpointer data)
{
    // Callback function for deleting a file
    (void)widget; // Mark unused parameter
    const char *filename = gtk_entry_get_text(GTK_ENTRY(data));
    deleteFile(filename);
}

void on_rename_file(GtkWidget *widget, gpointer data)
{
    // Callback function for renaming a file
    (void)widget; // Mark unused parameter
    const char *oldname = gtk_entry_get_text(GTK_ENTRY(data));
    const char *newname = gtk_entry_get_text(GTK_ENTRY(data));
    renameFile(oldname, newname);
}

void on_list_files(GtkWidget *widget, gpointer data)
{
    // Callback function for listing files
    (void)widget; // Mark unused parameter
    GtkTextBuffer *buffer = GTK_TEXT_BUFFER(data);
    gtk_text_buffer_set_text(buffer, "", -1);

    for (uint32_t i = 0; i < superblock.used_inodes; i++)
    {
        gtk_text_buffer_insert_at_cursor(buffer, superblock.inodes[i].filename, -1);
        gtk_text_buffer_insert_at_cursor(buffer, "\n", -1);
    }
}

void on_write_file(GtkWidget *widget, gpointer data)
{
    // Callback function for writing to a file
    (void)widget; // Mark unused parameter
    GtkWidget **entries = (GtkWidget **)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    const char *filedata = gtk_entry_get_text(GTK_ENTRY(entries[1]));
    writeToFile(filename, filedata);
}

void on_read_file(GtkWidget *widget, gpointer data)
{
    // Callback function for reading from a file
    (void)widget; // Mark unused parameter
    GtkWidget **entries = (GtkWidget **)data;
    const char *filename = gtk_entry_get_text(GTK_ENTRY(entries[0]));
    char filedata[1024];
    readFromFile(filename, filedata, sizeof(filedata));
    gtk_entry_set_text(GTK_ENTRY(entries[1]), filedata);
}

void writeToFile(const char *filename, const char *data)
{
    // Write data to a file
    char filePath[MAX_FILENAME];
    getFilePath(filename, filePath);
    FILE *file = fopen(filePath, "w");
    if (file)
    {
        fwrite(data, sizeof(char), strlen(data), file);
        fclose(file);
    }
}

void readFromFile(const char *filename, char *data, size_t data_size)
{
    // Read data from a file
    char filePath[MAX_FILENAME];
    getFilePath(filename, filePath);
    FILE *file = fopen(filePath, "r");
    if (file)
    {
        fread(data, sizeof(char), data_size, file);
        fclose(file);
    }
}

void createDirectory(const char *dirname)
{
    // Create a directory
    char dirPath[MAX_FILENAME];
    snprintf(dirPath, MAX_FILENAME, BASE_DIR "/%s", dirname);
    _mkdir(dirPath);
}

void on_create_directory(GtkWidget *widget, gpointer data)
{
    // Callback function for creating a directory
    (void)widget; // Mark unused parameter
    const char *dirname = gtk_entry_get_text(GTK_ENTRY(data));
    createDirectory(dirname);
}
