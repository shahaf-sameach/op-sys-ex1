#include <dirent.h>
#include <stdio.h>
#include <string.h>


int search_for_c_file(char* dir_name)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(dir_name);
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_REG)
      {
        if (dir->d_name[strlen(dir->d_name) - 2] == '.' && dir->d_name[strlen(dir->d_name) - 1] == 'c')
          printf("%s\n", dir->d_name);
      }
    }

    closedir(d);
  }

  return(0);
}

int search_for_dir(char* dir_name)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(dir_name);
  if (d)
  {
    while ((dir = readdir(d)) != NULL)
    {
      if (dir->d_type == DT_DIR)
      {
        if (dir->d_name[0] == '.' && strlen(dir->d_name) == 1)
          continue;

        if (dir->d_name[0] == '.' && dir->d_name[1] == '.' && strlen(dir->d_name) == 2)
          continue;
        
        printf("%s\n", dir->d_name);
      }
    }

    closedir(d);
  }

  return(0);
}


int main(void)
{
  printf("hello world");
}