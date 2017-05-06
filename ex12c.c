/*
 * ex12.c
 *
 *  Created on: Apr 1, 2016
 *      Author: stefan
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int search_for_dir(char* dir_name);
int read_conf(char *sConf_file, char** configs);
int search_for_c_file(char* dir_name);
int compile_file(char* file_name);
int run_file(char* file_name, char* output_file_name);




int read_conf(char *sConf_file, char** configs)
{
	int fd, counter = 0;
	char sBuff;

	fd = open(sConf_file, O_RDONLY);
  if (fd == -1) {
  	perror(sConf_file);
      return -1;
  }

  for(int i = 0; i < 3; i++)
  {
  	for(int j = 0; j < 160; j++)
  		configs[i][j] = 0;
  }

  while(counter < 3)
  {
  	if(read(fd, sBuff, 1) == -1)
  	{
  		perror('read');
  		return -1;
  	}

  	if(sBuff == 10)
  	{
  		counter++;
  		continue;
  	}

  	configs[counter][strlen(configs[counter])] = sBuff;
  }

  if (close(fd) == -1)
  {
    perror('close');
    return -1;
  }

  return 0;
}

int search_for_c_file(char* dir_name, char* file_name)
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
        {
          sprintf(file_name, "%s/%s" , dir_name, dir->d_name);
          return 0;
        }
      }
    }

    closedir(d);
  }

  return 1;
}

int search_for_dir(char* dir_name)
{
  DIR           *d;
  struct dirent *dir;
  d = opendir(dir_name);
  char* new_dir, c_file_name, compiled_file;

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
        
        sprintf(new_dir, "%s/%s" , dir_name, dir->d_name);
 
        if (search_for_c_file(new_dir, c_file_name) == 0)
        {
          if(compile(c_file_name) == 0)
          {
            if (run_file(compiled_file, configs[1]) == 0)
            {
              compare_files(output_file, configs[2])
            }
          }else {
            continue;
            //dir->d_name student get 0 COMPILATION_ERROR
          }
        }
      }
    }

    closedir(d);
  }

  return(0);
}

int compile_file(char* file_name)
{
  pid_t pid;
  int status, fd;
  char* run_args, sBuff;

  sprintf(run_args, "%s -o runnble" , file_name);

  if( ( pid = fork() ) == -1 )
  {
    perror( "fork() failed." );
    exit( EXIT_FAILURE );
  }

  if( pid == 0 )
  {
     execvp("gcc", run_args);
     perror( "execvp() failed." );
     exit( EXIT_FAILURE );
  }

  pid = wait( &status );

  if( ( pid = fork() ) == -1 )
  {
    perror( "fork() failed." );
    exit( EXIT_FAILURE );
  }

  if( pid == 0 )
  {
     execvp("echo", "$? > gcc_result.txt");
     perror( "execvp() failed." );
     exit( EXIT_FAILURE );
  }

  pid = wait( &status );

  fd = open('gcc_result.txt', O_RDONLY);
  if (fd == -1) {
      perror('gcc_result.txt');
      return -1;
  }

  if (read(fd, sBuff, 1) != -1)
  {
    if (close(fd) == -1)
      return -1;

    unlink("gcc_result.txt");

    if (sBuff[0] == '0')
    {
      return 0;
    }
    else
    {
      return -1;
    }
  }
}

int run_file(char* file_name, char* input_file_name, char* output_file_name)
{
  pid_t pid;
  int status, fd;
  char* run_args;

  sprintf(run_args, " < %s > %s" ,input_file_name, output_file_name);

  if( ( pid = fork() ) == -1 )
  {
    perror( "fork() failed." );
    exit( EXIT_FAILURE );
  }

  if( pid == 0 )
  {
     execvp(file_name, run_args);
     perror( "execvp() failed." );
     exit( EXIT_FAILURE );
  }

  pid = wait( &status );
  return 0;
}

int main(int argc, char *argv[])
{
  char configs[3][160] = {0};

  if (argc < 2)
  {
    printf("not enough parameters supplied. Usage: %s <path/to/config_file_one.txt>", argv[0]);
    return -1;
  }

  read_conf(argv[1], configs)
  search_for_dir(configs[0])
  
}


