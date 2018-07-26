/*///////////////////////////////////////////////////////////
*
* FILE:		server.c
* AUTHOR:	Jongwoo Jang
* PROJECT:	CS 3251 Project 1 - Professor Ellen Zegura 
* DESCRIPTION:	Network Server Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/*Included libraries*/

#include <stdio.h>	  /* for printf() and fprintf() */
#include <sys/socket.h>	  /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>	  /* for sockaddr_in and inet_addr() */
#include <stdlib.h>	  /* supports all sorts of functionality */
#include <unistd.h>	  /* for close() */
#include <string.h>	  /* support any string ops */
#include <time.h>

void error_handling(char *message);

#define RCVBUFSIZE 512		/* The receive buffer size */
#define SNDBUFSIZE 512		/* The send buffer size */
#define BUFSIZE 40		/* Your name can be as many as 40 chars*/

struct Accounts {
    int bal;
    int prevTime;
    int timer;
    int freq;

};

void preConfigAccounts(struct Accounts *mySavings, struct Accounts *myChecking,
 struct Accounts *myRetirement, struct Accounts *myCollege);

//Struct Accounts Initialization
struct Accounts mySavings = {0};
struct Accounts myChecking = {0};
struct Accounts myRetirement = {0};
struct Accounts myCollege = {0};

//struct Accounts mySavings, myChecking, myRetirement, myCollege;
//preConfigAccounts(&mySavings, &myChecking, &myRetirement, &myCollege);
/* The main function */
int main(int argc, char *argv[])

{
    preConfigAccounts(&mySavings, &myChecking, &myRetirement, &myCollege);




    int serverSock;				/* Server Socket */
    int clientSock;				/* Client Socket */

    struct sockaddr_in changeServAddr;		/* Local address */
    struct sockaddr_in changeClntAddr;		/* Client address */

    unsigned short changeServPort;		/* Server port */
    unsigned int clntLen;			/* Length of address data struct */

    char nameBuf[BUFSIZE];			/* Buff to store account name from client */
    int  balance;				/* Place to record account balance result */

    char rcvBuf[RCVBUFSIZE];
    char sndBuf[SNDBUFSIZE];

    char option[BUFSIZE];
    char amt[SNDBUFSIZE - 2*BUFSIZE];


    char balBuf[SNDBUFSIZE];
    int curTime;
    //int prevTime;
    /* Create new TCP Socket for incoming requests*/
    /*	    FILL IN	*/

    changeServPort = atoi(argv[1]);
    if((serverSock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        error_handling("failured to create socket for incoming request");
    }

    /* Construct local address structure*/
    /*	    FILL IN	*/
    memset(&changeServAddr, 0, sizeof(changeServAddr));
    changeServAddr.sin_family = AF_INET;
    changeServAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    changeServAddr.sin_port = htons(atoi(argv[1])); //changeServPort

    /* Bind to local address structure */
    /*	    FILL IN	*/
    if(bind(serverSock, (struct sockaddr*)&changeServAddr, sizeof(changeServAddr)) < 0) {
        //error_handling("bind() failed");
        perror("Could not bind");
    }

    /* Listen for incoming connections */
    /*	    FILL IN	*/
    if(listen(serverSock, 5) < 0) {
        error_handling("listen() failed");
    }
    /* Loop server forever*/
    while(1)
    {

        time_t now;
        struct tm *tm;
        now = time(0);
        tm = localtime(&now);
        int seconds;
        seconds = tm -> tm_sec;

        time_t sec;



	/* Accept incoming connection */
	/*	FILL IN	    */
        clntLen = sizeof(changeClntAddr);
        if((clientSock = accept(serverSock, (struct sockaddr*)&changeClntAddr, &clntLen)) < 0) {
            error_handling("accept() failed");
        }
        printf("Handling Client : %s \n", inet_ntoa(changeClntAddr.sin_addr));
        //printf("HANDLING CLIENT %s\n", inet_ntoa(changeClntAddr.sin_addr));

	/* Extract the account name from the packet, store in nameBuf */
	/* Look up account balance, store in balance */
	/*	FILL IN	    */
        if(recv(clientSock, rcvBuf, RCVBUFSIZE, 0) < 0) {
            error_handling("recv from client failed");
        }
        curTime = time(NULL);
        
        //printf("Cur Time : %d\n", curTime);

        
        //printf("Server Rcvd : %s \n", rcvBuf);
        sscanf(rcvBuf, "%[^,],%[^,],%s", nameBuf, option, amt);
        if(strcmp(amt, "(null)") == 0 ) {
            printf("Server Rcvd : %s, %s\n", nameBuf, option);
        } else {
            printf("Server Rcvd : %s, %s, %s\n", nameBuf, option, amt);
        }
        struct Accounts *curAccount = {0};
        //printf("NameBuf = %s\n", nameBuf); // how to send integer value to client.
        //printf("option = %s\n", option);
        //printf("amt = %s\n", amt);

        if(strcmp(nameBuf, "mySavings") == 0) {
            curAccount = &mySavings;
        } else if(strcmp(nameBuf, "myChecking") == 0) {
            curAccount = &myChecking;
        } else if (strcmp(nameBuf, "myRetirement") == 0) {
            curAccount = &myRetirement;
        } else if (strcmp(nameBuf, "myCollege") == 0) { 
            curAccount = &myCollege;
        } else {
            error_handling("Wrong Account Name");
        }
        //printf("Prev Time : %d\n", curAccount->prevTime);
        //if is BALANCE
        if(strcmp(option, "BAL") == 0) {
            //snprintf(sndBuf, SNDBUFSIZE, "%d", curAccount.bal);
            sprintf(sndBuf, "%d", curAccount->bal);
            printf("Server Sent : Balance Check Complete, Balance : %d\n", curAccount->bal);
        } else if(strcmp(option, "WITHDRAWAL") == 0) { //is WITHDRAWAL
            if(curAccount->prevTime == 0) 
                    curAccount->prevTime = curTime;
            curAccount->timer += (curTime - curAccount->prevTime);
            curAccount->freq += 1;
            curAccount->prevTime = curTime; 
            //printf("Timer = %d\n", curAccount->timer);  -------
            //printf("Frequency = %d\n", curAccount->freq);  ---------
            if(curAccount->timer > 60) {
                curAccount->timer = 0;
                curAccount->freq = 1;
            }
            if(curAccount->timer <= 60 && curAccount->freq > 3) { //Error: Account Timed Out
                sprintf(sndBuf, "%s", "Error : Account Timed Out!!");
                printf("Server Sent : Withdrawal Incomplete, Error : Account Timed Out, %d accesses in %dsec. \n", curAccount->freq, curAccount->timer);
                //curAccount->freq -= 1;

            } else {
                if(curAccount->bal < atoi(amt)) {
                    //sprintf(sndBuf, "Server Sent : Withdrawal Incomplete, Error : Insufficient Funds, Balance : %d, Amount : %s\n", curAccount->bal, amt)
                    sprintf(sndBuf, "%s", "Error : Insufficient Funds!!");
                    printf("Server Sent : Withdrawal Incomplete, Error : Insufficient Funds, Balance : %d, Amount : %s\n", curAccount->bal, amt);
                } else {
                    curAccount->bal = curAccount->bal - atoi(amt);
                    snprintf(sndBuf, SNDBUFSIZE, "%d,%s", curAccount->bal, amt);
                    printf("Server Sent : Withdrawal Complete, Balance : %d, Amount : %s\n", curAccount->bal, amt);
                }
            }

            //curAccount->timer = time(NULL);
            //curAccount->freq += 1;

            
        }
        //printf("curAccount.bal : %d\n", curAccount->bal);
        //sprintf(balBuf, "%d", curAccount.bal);
        balance = curAccount->bal;
        
        //printf("Action : %s\n", option);
	/* Return account balance to client */
	/*	FILL IN	    */
        //printf("sndBuf: %s\n", sndBuf);
        if(send(clientSock, sndBuf, SNDBUFSIZE, 0) < 0) {
            error_handling("send balance failed");
        }

        close(clientSock);
    }

}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void preConfigAccounts(struct Accounts *mySavings, struct Accounts *myChecking,
 struct Accounts *myRetirement, struct Accounts *myCollege) {
    /* mySavings, myChecking, myRetirement, myCollege 
     * ./client <account name> <server ip> <port> <BAL/WITHDRAW>
    */
    
    //mySavings = (struct Accounts *)malloc(sizeof(struct Accounts));
    mySavings->bal = 5000;
    mySavings->prevTime = 0;
    mySavings->timer = 0;
    mySavings->freq = 0;

    //myChecking = (struct Accounts *)malloc(sizeof(struct Accounts));
    myChecking->bal = 500;
    myChecking->prevTime = 0;
    myChecking->timer = 0;
    myChecking->freq = 0;

    //myRetirement = (struct Accounts *)malloc(sizeof(struct Accounts));
    myRetirement->bal = 500000;
    myRetirement->prevTime = 0;
    myRetirement->timer = 0;
    myRetirement->freq = 0;

    //myCollege = (struct Accounts *)malloc(sizeof(struct Accounts));
    myCollege->bal = 50000;
    myCollege->prevTime = 0;
    myCollege->timer = 0;
    myCollege->freq = 0;

}
