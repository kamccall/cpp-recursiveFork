#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

using namespace std;

void dothing(int &num, int &procs)
{  
   bool forked = false;
   int child1 = -1;
   int child2 = -1;
   int child3 = -1;
   int half1 = num /2;       // handled by parent
   int half2 = num - half1;  // handled by child
   int fd1[2], fd2[2], fd3[2];

   cout << getpid() << endl;
   
   if (procs == 4)
   {
      procs = procs / 2;
      pipe(fd1);
      child1 = fork();
      forked = true;
      if (child1 == 0)   // child
      {
         close(fd1[0]);
         dothing(half2,procs);
         write(fd1[1], &half2, sizeof(int));
         exit(0);
      }
      else               // parent
      {
         close(fd1[1]);
         dothing(half1,procs);
         read(fd1[0], &half2, sizeof(int));
         num = half1 + half2;
      }
   }   
   else if (procs == 2)
   {
      procs = procs / 2;
      // if there aren't any other forks, do first fork
      if (!forked)
      {
         pipe(fd1);
         child1 = fork();
         if (child1 == 0)   // child
         {
            close(fd1[0]);
            dothing(half2,procs);
            write(fd1[1], &half2, sizeof(int));
            exit(0);
         }
         else               // parent
         {
            close(fd1[1]);
            dothing(half1,procs);
            read(fd1[0], &half2, sizeof(int));
            num = half1 + half2;
         }
      }
      else
      {
         // already 2 processes running. must fork both
         int half1a = half1 / 2;
         int half1b = half1 - half1a;

         int half2a = half2 / 2;
         int half2b = half2 - half2a;

         if (child1 == 0)  // child from last fork
         {
            // fork into child and grandhild
            pipe(fd2);
            child2 = fork();
            if (child2 == 0)  // granchild
            {
               close(fd2[0]);
               dothing(half2b, procs);
               write(fd2[1], &half2b, sizeof(int));
               exit(0);
            }
            else              // child (now parent)
            {
               close(fd2[1]);
               dothing(half2a, procs);
               read(fd2[1], &half2b, sizeof(int));
               num = half2a + half2b;
            }
         }
         else              // parent from last fork
         {
            pipe(fd3);
            child3 = fork();
            if (child3 == 0)  // new child of parent
            {
               close(fd3[0]);
               dothing(half1b, procs);
               write(fd3[1], &half1b, sizeof(int));
               exit(0);
            }
            else            // grandparent
            {
               close(fd3[1]);
               dothing(half1a, procs);
               read(fd3[1], &half1b, sizeof(int));
               num = half1a + half1b;
            }
         }
      }
   }
   else   // procs == 1
   {
      if (half1 > 1) dothing(half1, procs);
      if (half2 > 1) dothing(half2, procs);
      num = half1 + half2;
   }

   wait(&child2);
   wait(&child3);
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