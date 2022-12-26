/*
 * main.c : A program for copying the folders (and their content)/files at given path
 * usage  : main.exe SOURCE_Path DESTINATION_Path
 * author : Safa BULAT, https://www.linkedin.com/in/safabulat/, m.safa.bulat@gmail.com
 * date   : 24.12.2022
*/

//Headers
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>

//Function prototype for copy_directoryAndFiles
void copy_directoryAndFiles(const char* src, const char* dst);

//Function prototype for directory_exist
bool directory_exists(const char* path);

//Main
int main(int argc, char* argv[]) {

  //Check if the number of arguments is correct
  if (argc != 3) {
    printf("Usage: %s SOURCE_Path DESTINATION_Path\n", argv[0]);
    return 1;
  }

  //Get the source and destination paths
  const char* src = argv[1];
  const char* dst = argv[2];

  //Copy the directory and all its contents
  copy_directoryAndFiles(src, dst);

  //Done 
  return 0;
}

//Function to copy a directory and all its contents and files
void copy_directoryAndFiles(const char* src, const char* dst) {

  //Open the source directory
  DIR* dir = opendir(src);
  if (dir == NULL) {
    perror("opendir");
    return;
  }

  //Check if the destinated directory exist
  if(!directory_exists(dst)){

    //Create the destination directory if not exist
    if (mkdir(dst) != 0) {
      perror("mkdir");
      closedir(dir);
      return;
    }    
  }

  //Set the permissions of the destination directory
  if (chmod(dst, 0700) != 0) {
    perror("chmod");
    closedir(dir);
    return;
  }

  //Iterate over the contents of the source directory
  struct dirent* entry;
  while ((entry = readdir(dir)) != NULL) {

    //Skip the "." and ".." entries to avoid infinite recursion
    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
      continue;
    }

    //Get the full path of the current entry
    char src_path[1024];
    snprintf(src_path, sizeof(src_path), "%s/%s", src, entry->d_name);

    //Get the full path of the destination
    char dst_path[1024];
    snprintf(dst_path, sizeof(dst_path), "%s/%s", dst, entry->d_name);

    //Check if the current entry is a directory
    if (entry->d_type == DT_DIR) {

      //Recursively copy the subdirectory
      copy_directoryAndFiles(src_path, dst_path);
    } 
    else {

      //Print debug
      printf("\nCopying file %s to %s\n", src_path, dst_path);

      //Open the source file
      FILE* src_file = fopen(src_path, "rb");
      if (src_file == NULL) {
        perror("fopen");
        continue;
      }

      //Open the destination file
      FILE* dst_file = fopen(dst_path, "wb");
      if (dst_file == NULL) {
        perror("fopen");
        fclose(src_file);
        continue;
      }

      //Size buffs
      size_t total_size =0;
      size_t bytes_copied =0;

      //Get the size of the source file
      fseek(src_file, 0, SEEK_END);
      size_t size = ftell(src_file);
      rewind(src_file);

      //Add the size of the file to the total size
      total_size += size;

      //Copy the file contents
      char buffer[1024];
      size_t bytes_read;
      while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dst_file);
                
        //Update the number of bytes copied
        bytes_copied += bytes_read;

        //Calculate and print the progress in percentage
        float progress = (float)bytes_copied / (float)total_size * 100;

        printf("\rProgress: %.2f%%", progress); //x.xx%
        fflush(stdout);
        
      }

      //To make it MORE beautiful
      printf("\n");

      //Close the files
      fclose(src_file);
      fclose(dst_file);
    }
  }

  //Close the directory
  closedir(dir);
}

//Function to check if the directory exist
bool directory_exists(const char* path) {
  struct stat sb;
  if (stat(path, &sb) == 0 && S_ISDIR(sb.st_mode)) {
    return true; //dir exist
  }
  return false;  //dir not exist
}

//Done