#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void dothing(int &num, int &procs)
{  
   int child1;
   int half1 = num /2;       // handled by parent
   int half2 = num - half1;  // handled by child
   int fd1[2], fd2[2], fd3[2];

   cout << getpid() << endl;

   if (procs > 1)
   {
      procs = procs / 2;
      pipe(fd1);
      child1 = fork();
      if (child1 == 0)    // child
      {
         close(fd1[0]);
         dothing(half2,procs);
         write(fd1[1], &half2, sizeof(int));
         cout << "half2: " << half2 << endl;
         exit(0);
      }
      else                // parent
      {
         close(fd1[1]);
         dothing(half1,procs);
         read(fd1[0], &half2, sizeof(int));
         num = half1 + half2;
      }
   }   
   else
   {
      if (half1 > 1) dothing(half1, procs);
      if (half2 > 1) dothing(half2, procs);
      num = half1 + half2;
   }

   wait(&child1);
}

int main(int argc, char* argv[])
{
   int numProcs = 4;
   int number = atoi(argv[1]);
   dothing(number, numProcs);

   cout << "total: " << number << endl;
   exit(0);   
}