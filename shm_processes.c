#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>

void PoorStudent(int[]);
void DearOldDad(int[]);
int Turn = 0; //shared,global var

int main(int argc, char *argv[])
{
     int ShmID;
     int *ShmPTR;
     pid_t pid;
     int status;

     srandom(getpid()); //seed for random function

     if (argc != 3)
     {
          printf("Use: %s 0 0\n", argv[0]); //inputting the name of prgm
     }

     ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);
     //checks for errors
     if (ShmID < 0)
     {
          printf("*** server error: shmget  ***\n");
          exit(1);
     }

     printf("Server received shared memory: two integers...\n");

     ShmPTR = (int *)shmat(ShmID, NULL, 0);

     if (*ShmPTR == -1)
     {
          printf("*** server error: shmat ***\n");
          exit(1);
     }
     printf("Server attached shared memory...\n");

     ShmPTR[0] = atoi(argv[1]); //BankAcoount (atoi= converts string to integer)
     ShmPTR[1] = atoi(argv[2]); //Turn variable

     printf("Server has filled %d %d in shared memory...\n",
            ShmPTR[0], ShmPTR[1]); //inserting 4 ints into shared mem ptr

     printf("Server is about to fork a child process...\n");

     pid = fork(); //creates a proc

     //checks for errors
     if (pid < 0)
     {
          printf("*** server error: fork ***\n");
          exit(1);
     }
     else if (pid == 0)
     {                         //child proc : Poor Student
          PoorStudent(ShmPTR); //call function below
          exit(0);
     }
     else
     {
          //else call parent proc: Dear Old Dad
          DearOldDad(ShmPTR);
     }

     wait(&status);
     printf("Server's child is finished...\n");
     shmdt((void *)ShmPTR);
     printf("Server's shared memory has beeb detached...\n");
     shmctl(ShmID, IPC_RMID, NULL);
     printf("Server shared memory has been removed...\n");
     printf("Server exits...\n");
     exit(0);
}

void DearOldDad(int SharedMem[])
{
     int i = 0;
     int bankAccount = SharedMem[0];
     int balance;

     for (i = 0; i < 25; i++)
     {
          sleep(random() % 5); //student sleeps for 0-5 seconds

          //Withdrawing Money from Bank Account

          while (SharedMem[1] != 0)
          {    //Turn !=0
               //do nothing
          }
          bankAccount = SharedMem[0];

          if (bankAccount <= 100)
          {
               balance = random() % 100; //Getting a random amount of money from the account.

               //Amount is even
               if (balance % 2 == 0)
               {
                    bankAccount = bankAccount + balance; //deposit balance to BA
                    printf("Dear old Dad: Deposits $%d / Balance = $%d\n", balance, bankAccount);
               }
               //Amount is odd
               else
               {
                    printf("Dear old Dad: Doesn't have any money to give\n");
               }
          }
          else
          {
               printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n", bankAccount);
          }

          SharedMem[0] = bankAccount; //modify the copy and put it back into shared memory
          SharedMem[1] = 1;           //Turn=1
     }
}

void PoorStudent(int SharedMem[])
{
     int i = 0;
     int bankAccount = SharedMem[0];
     int balanceNeed;

     //creating child proc
     for (i = 0; i < 25; i++)
     {
          sleep(random() % 5); //student sleeps for 0-5 seconds
                               //Withdrawing Money from Bank Account

          while (SharedMem[1] != 1)
          {
               //do nothing
          }
          bankAccount = SharedMem[0];
          balanceNeed = random() % 50;
          printf("Poor Student needs $%d\n", balanceNeed);

          if (balanceNeed <= bankAccount)
          {
               bankAccount = bankAccount - balanceNeed; //withdraw balanceNeed from Bank Account
               printf("Poor Student: Withdraws $%d / Bank Account Balance = $%d\n", balanceNeed, bankAccount);
          }
          else
          { //funds are too low can't withdraw ....been there before :/
               printf("Poor Student: Not Enough Cash ($%d)\n", bankAccount);
          }
          SharedMem[0] = bankAccount; //modify the copy and put it back into shared memory

          SharedMem[1] = 0; //Turn=0
     }
}