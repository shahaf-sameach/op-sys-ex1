// External Links
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <time.h>
#include <sys/wait.h>


// Defines
#define RUN_FILE_NAME "runable.out"
#define TEMP_OUTPUT "tmp_output.txt"
#define RESULT_FILE "results.csv"
#define _OK 0
#define _ERR -1
#define _IN
#define _OUT


// Function Declarations
int read_conf(_IN char*);
int search_for_c_file(_IN char*, _OUT char*);
int compile_file(_IN char*);
int run_file(_IN char*, _IN char*, _IN char*, int*);
int compare_files(_IN char*, _IN char*, _OUT int*);
int print_result(_IN char*, _IN int, _IN char*);


// Globals
char configs[3][160];


// --------------------------------- Functions -------------------------------- //

// Print error to STDERR
int print_error(_IN char* sMsg)
{
	return write(2, sMsg, sizeof(sMsg));
}

// Print student result into RESULT_FILE file
int print_result(_IN char* sStudent,_IN int iGrade, _IN char* sReason)
{
	char buff[128];
	int fd;

	fd = open(RESULT_FILE, O_RDWR | O_APPEND | O_CREAT, 0664 );
	if (fd == -1)
	{
		print_error(RESULT_FILE);
		return _ERR;
	}

	sprintf(buff, "%s,%d,%s\n", sStudent, iGrade, sReason);
	if ((write(fd, buff, strlen(buff))) == -1)
	{
		print_error(RESULT_FILE);
		return _ERR;
	}

	if (close(fd) == -1)
	{
		print_error(RESULT_FILE);
		return _ERR;
	}

	//student[0] = 0;
	return _OK;
}

// Compare given and expected output files (using Comp.out)
int compare_files(_IN char* sRecieved, _IN char* sExpected, _OUT int* iResult)
{
	pid_t pid;
	int status, stat;

	*iResult = 1;

	pid = fork();
	if(pid == -1)
	{
		print_error(strerror);
		exit(-1);
	}

	if(pid == 0)
	{
		execlp("./Comp.out", "Comp.out", sRecieved, sExpected, NULL);
		print_error(strerror);
		exit(-1);
	}

	pid = wait(&status);
	stat = WEXITSTATUS(status);

	if (stat == 2)
		*iResult = 2;
	else if (stat == 3)
		*iResult = 3;
	else
		*iResult = 1;

	return _OK;
}

// Read config file and save parameters
int read_conf(_IN char *sConf_file)
{
	int fd, counter = 0;
	char sBuff[1];
	int i, j;

	fd = open(sConf_file, O_RDONLY);
	if (fd == -1)
	{
		print_error(strerror);
		return _ERR;
	}

	for(i = 0; i < 3; i++)
	{
		for(j = 0; j < 160; j++)
			configs[i][j] = 0;
	}

	while(counter < 3)
	{
		if(read(fd, sBuff, 1) == -1)
		{
			print_error(strerror);
			return _ERR;
		}

		if(sBuff[0] == 10)
		{
			counter++;
			continue;
		}

		configs[counter][strlen(configs[counter])] = sBuff[0];
	}

	if (close(fd) == -1)
	{
		print_error(strerror);
		return _ERR;
	}

	return _OK;
}

// Look for .c file inside the given path, concat file name (if found) to path.
int search_for_c_file(_IN char* sPath, _OUT char* sFile_Name)
{
	DIR           *d;
	struct dirent *dir;
	d = opendir(sPath);

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_REG)
			{
				if (dir->d_name[strlen(dir->d_name) - 2] == '.' && dir->d_name[strlen(dir->d_name) - 1] == 'c')
				{
					sprintf(sFile_Name, "%s/%s" , sPath, dir->d_name);
					return _OK;
				}
			}
		}
		closedir(d);
	}
	return _ERR;
}

// Compile .c file from a given path
int compile_file(_IN char* sPath)
{
	pid_t pid;
	int status, stat, out1, out2;

	pid = fork();
	if(pid == -1)
	{
		print_error(strerror);
		exit(-1);
	}

	if(pid == 0)
	{
		out1 = open("temp1.txt", O_RDWR | O_APPEND | O_CREAT, 0664 );
		out2 = open("temp2.txt", O_RDWR | O_APPEND | O_CREAT, 0664 );
		dup2(out1, 1);
		dup2(out2, 2);
		close(out1);
		close(out2);
		execlp("gcc", "gcc", sPath, "-o", RUN_FILE_NAME, NULL);
		print_error(strerror);
		exit(-1);
	}

	pid = wait(&status);
	stat = WEXITSTATUS(status);

	unlink("temp1.txt");
	unlink("temp2.txt");
	return stat;
}

// Execute file from given path (redirect input and output).
int run_file(_IN char* sPath, _IN char* sInput_File, _IN char* sOutput_File, _OUT int* iRun_Time)
{
	pid_t pid, wpid = 0;
	int status, in, out;
	time_t start, end;
	char arg[2048];

	*iRun_Time = 0;

	sprintf(arg, "./%s", sPath);

	pid = fork();
	if(pid == -1)
	{
		print_error(strerror);
		exit(-1);
	}

	if(pid == 0)
	{
		in = open(sInput_File, O_RDONLY);
		out = open(sOutput_File, O_RDWR | O_APPEND | O_CREAT, 0664 );
		dup2(in, 0);
		dup2(out, 1);
		close(in);
		close(out);
		execlp(arg, sPath, NULL);
		print_error(strerror);
		exit(-1);
	}

	time(&start);
    //wpid = wait(&status);
	time(&end);

    while  (end - start <= 5) {
        time(&end);
    }

    if (end - start > 5)
	{
		*iRun_Time = 1;
		kill(pid, SIGKILL);
	}

	unlink(sPath);
	return _OK;
}

int duplicate_dir(_IN char* sPath, _OUT char* sFile, _OUT int* iDepth)
{
	char sNext_Dir[2048];
	DIR* d;
	struct dirent *dir;
    int dir_num = 0;

	// if no .c file found, call self with next subdir (if exists).
	d = opendir(sPath);
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

                dir_num++;

			}
		}
		closedir(d);

        if (dir_num == 1)
            return _OK;
	}

	// if we reach here than there are more than 1 folders.
	return _ERR;
}

int One_Student(_IN char* sPath, _OUT char* sFile, _OUT int* iDepth)
{
	char sNext_Dir[2048];
	DIR* d;
	struct dirent *dir;

	// look for .c file in give path
	if (search_for_c_file(sPath, sFile) == _OK)
		return _OK;

	// if no .c file found, call self with next subdir (if exists).
	d = opendir(sPath);
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

				sprintf(sNext_Dir, "%s/%s" , sPath, dir->d_name);
				*iDepth = *iDepth + 1;

				// return if found .c file in subdir
				if (One_Student(sNext_Dir, sFile, iDepth) == _OK)
				{
					closedir(d);
					return _OK;
				}

				// try next subdir
				*iDepth = *iDepth - 1;
				continue;
			}
		}
		closedir(d);
	}

	// at this point no .c file was found.
	return _ERR;
}


int All_Students(_IN char* sPath)
{
	char sStudent_Path[2048], sFile[2048], sStudent[2048];
	DIR* d;
	struct dirent *dir;
	int iResult, iRunTime, iDepth, res, iC = 0;

	d = opendir(sPath);

	if (d)
	{
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type == DT_DIR)
			{
				// ignore 'root' dir for all students and it's parent.
				if (dir->d_name[0] == '.' && strlen(dir->d_name) == 1)
					continue;
				if (dir->d_name[0] == '.' && dir->d_name[1] == '.' && strlen(dir->d_name) == 2)
					continue;

				sprintf(sStudent_Path, "%s/%s" , sPath, dir->d_name);
				strcpy(sStudent, dir->d_name);
				iDepth = 0;

				if (duplicate_dir(sStudent_Path, sFile, &iDepth) == _OK) {
                    if (One_Student(sStudent_Path, sFile, &iDepth) == _OK) {
                        if (compile_file(sFile) == _OK) {
                            if (run_file(RUN_FILE_NAME, configs[1], TEMP_OUTPUT, &iRunTime) == _OK) {
                                if (iRunTime == 1) {
                                    print_result(sStudent, 0, "TIMEOUT");
                                } else if (compare_files(TEMP_OUTPUT, configs[2], &iResult) == _OK) {
                                    if (iResult == 1)
                                        print_result(sStudent, 0, "BAD_OUTPUT");
                                    else if ((iResult == 2) && (iDepth == 0))
                                        print_result(sStudent, 100, "GREAT_JOB");
                                    else if (iResult == 2) {
                                        res = 100 - (10 * iDepth);
                                        if (res < 0)
                                            res = 0;
                                        print_result(sStudent, res, "WRONG_DIRECTORY");
                                    } else
                                        print_result(sStudent, 80, "SIMILAR_OUTPUT");
                                }
                                unlink(TEMP_OUTPUT);
                            }
                        } else {
                            print_result(sStudent, 0, "COMPILATION_ERROR");
                        }
                    } else {
                        print_result(sStudent, 0, "NO_C_FILE");
                    }
                } else {
                    print_result(sStudent, 0, "MULTIPLE_DIRECTORIES");
                }
			}
		}
	}

	closedir(d);
	return _OK;
}

// --------------------------------- Functions -------------------------------- //




// --------------------------------- Main -------------------------------- //

int main(int argc, char *argv[])
{
	argc = 2;

	if (argc < 2)
	{
		printf("not enough parameters supplied. Usage: %s <path/to/config_file_one.txt>", argv[0]);
		return _ERR;
	}

	unlink(RESULT_FILE);

	read_conf(argv[1]);
	All_Students(configs[0]);
}
// --------------------------------- Main -------------------------------- //

