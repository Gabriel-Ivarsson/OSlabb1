#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PERMS 0644

struct my_msgbuf {
   long mtype;
   int intBuf;  // Integer to be sent
};

int main(void) {
   struct my_msgbuf buf;
   int msqid;
   int len;
   key_t key;
   time_t t;
   srand((unsigned)time(&t));
   int bufInt = 0;

   if ((key = ftok("msgq.txt", 'B')) == -1) {
      perror("ftok");
      exit(1);
   }

   if ((msqid = msgget(key, PERMS)) == -1) {  // Connects to queue
      perror("msgget");
      exit(1);
   }

   printf("message queue: ready to send messages.\n");
   buf.mtype = 1; /* we don't really care in this case */

   for (int i = 0; i < 50; i++) {  // Sends messages for 50 times
      // Random number between INT_MAX - INT_MIN
      buf.intBuf = (rand() % INT_MAX * 2) - INT_MIN;
      printf("Sending iteration: %d Number: %d\n", i, buf.intBuf);
      // Sends random number
      if (msgsnd(msqid, &buf, sizeof(buf.intBuf), 0) == -1)
         perror("msgsnd");
   }

   printf("message queue: done sending messages.\n");
   return 0;
}
