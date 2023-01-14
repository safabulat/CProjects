/*
 * main.c : A program for copying the folders (and their content)/files at given path
 * usage  : main.exe SOURCE_Path DESTINATION_Path
 * author : Safa BULAT, https://www.linkedin.com/in/safabulat/, m.safa.bulat@gmail.com
 * date   : 24.12.2022
*/

/* First approach: 
-Open source and destinated (create if not ) directories,
-Search through files:
- get size
- if folder: go inside copy fils (recursive) 
- else use: fopen, fread, fwrite, fclose
-track prosses
-close folders etc
-end

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

int main etc..
*/

/*
Second approach: 
Updating this code with Boost filesystem library to handle file and directory operations.
This would allow me to simplify the code by using functions such as copy_directory and copy_file,
which handle all of the necessary operations for copying a directory or file.

-Add Boost header #include <boost/filesystem.hpp>
-Modify the copy function with boost filesystem functions
-replace the while loop that reads the contents of the source directory
with a recursive function that iterates over the contents of the source directory and copies each file or directory.

//Function to copy a directory and all its contents and files
void copy_directoryAndFiles(const char* src, const char* dst) {
  // Initialize a counter for the total size of the files being copied
  size_t total_size = 0;
  // Initialize the Boost progress_display class with the total size of the files being copied
  boost::progress_display progress(total_size);

  // Check if the destination directory exists
  if(!boost::filesystem::exists(dst)) {
    // Create the destination directory if it does not exist
    boost::filesystem::create_directory(dst);
  }

  // Iterate over the contents of the source directory
  for(boost::filesystem::directory_iterator it(src); it != boost::filesystem::directory_iterator(); ++it) {
    // Get the full path of the current entry
    boost::filesystem::path src_path = it->path();
    boost::filesystem::path dst_path = dst / src_path.filename();

    // Check if the current entry is a directory
    if(boost::filesystem::is_directory(src_path)) {
      // Recursively copy the subdirectory
      copy_directoryAndFiles(src_path, dst_path);
    }
    else
    {
      // Print debug
      std::cout << "Copying file " << src_path << " to " << dst_path << std::endl;
      // Use the Boost filesystem function to copy the file
      boost::filesystem::copy_file(src_path, dst_path);
      // Add the size of the file to the total size
      total_size += boost::filesystem::file_size(src_path);
      // Increment the progress bar by the size of the file
      ++progress;
    }
  }
}

//We dont need to use directory_exist function anymore
//bool directory_exists(const char* path);

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

*/


/*
Third approach:
-using windows tchar methods

*/

//Headers
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <shlwapi.h>

// Function prototype for copy_directory_windows
void copy_directory_windows(const TCHAR* src, const TCHAR* dst);

// Main
int main(int argc, char* argv[]) {
  // Check if the number of arguments is correct
  if (argc != 3) {
    printf("Usage: %s SOURCE_Path DESTINATION_Path\n", argv[0]);
    return 1;
  }

  // Get the source and destination paths
  const char* src = argv[1];
  const char* dst = argv[2];

  // Convert the source and destination paths to wide character strings
  TCHAR src_path[MAX_PATH];
  TCHAR dst_path[MAX_PATH];
  mbstowcs(src_path, src, MAX_PATH);
  mbstowcs(dst_path, dst, MAX_PATH);

  // Copy the directory and all its contents
  copy_directory_windows(src_path, dst_path);

  // Done 
  return 0;
}


// Function to copy a directory and all its contents and files in the Windows operating system
void copy_directory_windows(const TCHAR* src, const TCHAR* dst) {
  // Find the first file in the source directory
  TCHAR search_path[MAX_PATH];
  _stprintf(search_path, _T("%s\\*"), src);
  WIN32_FIND_DATA find_data;
  HANDLE find_handle = FindFirstFile(search_path, &find_data);
  if (find_handle == INVALID_HANDLE_VALUE) {
    printf("FindFirstFile failed (%d)\n", GetLastError());
    return;
  }

  // Iterate over the contents of the source directory
  do {
    // Skip the "." and ".." entries to avoid infinite recursion
    if (_tcscmp(find_data.cFileName, _T(".")) == 0 || _tcscmp(find_data.cFileName, _T("..")) == 0) {
      continue;
    }

    // Get the full path of the current entry
    TCHAR src_path[MAX_PATH];
    _stprintf(src_path, _T("%s\\%s"), src, find_data.cFileName);

    // Get the full path of the destination
    TCHAR dst_path[MAX_PATH];
    _stprintf(dst_path, _T("%s\\%s"), dst, find_data.cFileName);

    // Check if the current entry is a directory
    if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      // Recursively copy the subdirectory
      copy_directory_windows(src_path, dst_path);
    } 
    else {
      // Print debug
      printf("\nCopying file %ls to %ls\n", src_path, dst_path);

      // Copy the file
      if (!CopyFile(src_path, dst_path, FALSE)) {
        printf("CopyFile failed (%d)\n", GetLastError());
      }
    }
  } while (FindNextFile(find_handle, &find_data) != 0);

  // Close the handle to the file search
  FindClose(find_handle);
}