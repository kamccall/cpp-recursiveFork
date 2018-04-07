#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void dothing(int &num, int procs)
{  
   int child;
   int half1 = num /2;       // handled by parent
   int half2 = num - half1;  // handled by child
   int fd[2];

   cout << getpid() << endl;

   if (procs > 1)
   {
      // procs = procs / 2;
      pipe(fd);
      child = fork();
      if (child == 0)    // child
      {
         close(fd[0]);
         dothing(half2,procs/2);
         write(fd[1], &half2, sizeof(int));
         cout << "half2: " << half2 << endl;
         exit(0);
      }
      else                // parent
      {
         close(fd[1]);
         dothing(half1,procs/2);
         wait(&child);
         read(fd[0], &half2, sizeof(int));
         num = half1 + half2;
      }
   }   
   else
   {
      if (half1 > 1) dothing(half1, procs);
      if (half2 > 1) dothing(half2, procs);
      num = half1 + half2;
   }

   // wait(&child);
}

int main(int argc, char* argv[])
{
   int numProcs = 4;
   int number = atoi(argv[1]);
   dothing(number, numProcs);

   cout << "total: " << number << endl;
   exit(0);   
}