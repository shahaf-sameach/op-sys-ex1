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


#define OK 0
#define NO_OK -1


int read_conf(char *sConf_file, char** configs)
{
	int fd, counter = 0;
	// char configs[3][160] = {0};
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
    perror('close')
    return -1;
  }

  return 0;
}

int get_file_list(char** file_list, char* dir)
{
  pid_t pid;
  int status, fd, counter = 0, dir_counter = 0;
  char sBuff;
  char dir_name[128];
  char* output_file = 'file_list.txt'

  sprintf(run_args, "%s > %s" , dir, output_file);

  if( ( pid = fork() ) == -1 )
  {
    perror( "fork() failed." );
    exit( EXIT_FAILURE );
  }

  if( pid == 0 )
  {
   execvp('ls', run_args);
   perror( "execvp() failed." );
   exit( EXIT_FAILURE );
  }

  pid = wait( &status );

  fd = open(output_file, O_RDONLY);
  if (fd == -1)
  {
    perror(output_file);
    return -1;
  }

  for(int i = 0; i < 128; i++)
    dir_name[i] = 0;

  while(1)
  {
    r = read(fd, sBuff, 1);

    if(r == -1)
    {
      perror('read');
      return -1;
    }

    // reached end of file
    if (sBuff == 0)
      return 0;

    //read enter (end of line)
    if (sBuff == 10)
    {
      counter = 0;
      dir_list[dir_counter] = dir_name;
      dir_counter++;
      counter = 0;

      for(int i = 0; i < 128; i++)
        dir_name[i] = 0;

      continue;
    }

    dir_name[counter] = sBuff
    counter++

  }

}

int is_c_file(char *file_name)
{
  if (string[strlen(file_name) - 1] == 'c' && string[strlen(file_name) - 2] == '.')
    return OK;
  return NOT_OK;
}


int is_dir(char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode);
}

int delete_file(char* file_name)
{
  if(unlink(file_name) == -1)
  {
    perror(file_name);
    return -1;
  }
  return 0
}

int compile_file(char* file_name, char* args)
{
	pid_t pid;
	int status, fd;
	char* run_args, sBuff;

	sprintf(run_args, "%s -o runnble" , args);

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

		delete_file("gcc_result.txt");

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

int run_file(char* file_name)
{
	pid_t pid;
	int status, fd;

	if( ( pid = fork() ) == -1 )
	{
		perror( "fork() failed." );
		exit( EXIT_FAILURE );
	}

	if( pid == 0 )
	{
	   execvp(file_name, " > output.txts");
	   perror( "execvp() failed." );
	   exit( EXIT_FAILURE );
	}

	pid = wait( &status );
	return 1;
}

int get_dir_list(char** dir_list,char* dir)
{
	pid_t pid;
	int status, fd;
	char* run_args, sBuff;

	sprintf(run_args, "%s -o runnble" , args);


	if( ( pid = fork() ) == -1 )
	{
		perror( "fork() failed." );
		exit( EXIT_FAILURE );
	}

	if( pid == 0 )
	{
	   execvp(ls , "-l ");
	   perror("execvp() failed.");
	   exit( EXIT_FAILURE );
	}

	pid = wait( &status );
	return 1;
}

int main(int argc, char *argv[])
{
  char configs[3][160] = {0};
  char list[128][128]

  if (argc < 2)
  {
    printf("not enough parameters supplied. Usage: %s <path/to/config_file_one.txt>", argv[0]);
    return -1;
  }

  read_conf(argv[1], configs)
  get_file_list(list, ".")
  
}


