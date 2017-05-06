
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

int write_error(char *serror)
{
  if( ( write( 2, serror, sizeof( serror ) ) ) == -1 )
    return -1;

  return 0;

}

int checkFiles(int fd1, int fd2)
{
  char sBuff1[1], sBuff2[1];
  int iResult = 2, iRead1 = 1, iRead2 = 1;

  while (1)
  {
    // read a char from both files.
    if (iRead1 > 0)
      iRead1 = read(fd1, sBuff1, 1);

    if (iRead2 > 0)
      iRead2 = read(fd2, sBuff2, 1);


    // stop in case of error.
    if ((iRead1 == -1) || (iRead2 == -1))
      return -1;

    // end of file reached for both files - return result.
    if ((iRead1 == 0) && (iRead2 == 0))
      return iResult;

    if (iRead1 == 0)
      sBuff1[0] = 0;

    if (iRead2 == 0)
      sBuff2[0] = 0;


    //chars are the same - keep reading from both files
    if (sBuff1[0] == sBuff2[0])
    {
      iRead1 = 2;
      iRead2 = 2;
      continue;
    }

    // keep reading from the first file
    if ((sBuff1[0] == 10) || (sBuff1[0] == 32))
    {
      iRead1 = 2;
      iRead2 = -2;
      iResult = 3;
      continue;
    }

    // keep reading from the second file
    if ((sBuff2[0] == 10) || (sBuff2[0] == 32))
    {
      iRead1 = -2;
      iRead2 = 2;
      iResult = 3;
      continue;
    }

    // both chars are similar.
    if ((sBuff1[0] - sBuff2[0] == 32) || (sBuff2[0] - sBuff1[0] == 32))
    {
      if (((sBuff1[0] <= 90 && sBuff1[0] >= 65) && (sBuff2[0] <= 122 && sBuff2[0] >= 97)) || ((sBuff2[0] <= 90 && sBuff2[0] >= 65) && (sBuff1[0] <= 122 && sBuff1[0] >= 97)))
      {
        iResult = 3;
        iRead1 = 2;
        iRead2 = 2;
        continue;
      }
    }

    return 1;

  }

  return 1;
}

int compare(char *f1, char* f2)
{
    int fd1, fd2, result;

    // open first file
    fd1 = open(f1, O_RDONLY);
    if (fd1 == -1) {
      write_error(sys_errlist[errno]);
      return -1;
    }

    // open second file
    fd2 = open(f2, O_RDONLY);
    if (fd2 == -1) {
      write_error(sys_errlist[errno]);
      return -1;
    }

    // check file similarities
    result = checkFiles(fd1, fd2);

    // close files
    if ((close(fd1) == -1) || (close(fd2) == -1))
    {
      write_error(sys_errlist[errno]);
      return -1;
    }

    return result;
}

int main (int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("not enough parameters supplied. Usage: %s.out <path/to/file_one.txt> <path/to/file_two.txt>", argv[0]);
        return -1;
    }

    return compare(argv[1], argv[2]);
}

