/*///////////////////////////////////////////////////////////
*
* FILE:		client.c
* AUTHOR:	Jongwoo Jang
* PROJECT:	CS 3251 Project 1 - Professor Ellen Zegura 
* DESCRIPTION:	Network Client Code
* CREDIT:	Adapted from Professor Traynor
*
*////////////////////////////////////////////////////////////

/* Included libraries */

#include <stdio.h>		    /* for printf() and fprintf() */
#include <sys/socket.h>		    /* for socket(), connect(), send(), and recv() */
#include <arpa/inet.h>		    /* for sockaddr_in and inet_addr() */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

void error_handling(char *message);

/* Constants */
#define RCVBUFSIZE 512		    /* The receive buffer size */
#define SNDBUFSIZE 512		    /* The send buffer size */
#define REPLYLEN 32
#define BUFSIZE 40      /* Your name can be as many as 40 chars*/

/* The main function */
int main(int argc, char *argv[])
{

    time_t now;
    struct tm *tm;
    now = time(0);
    tm = localtime(&now);
    int seconds;
    seconds = tm -> tm_sec;
    printf("Time : %s", asctime(tm));


    int clientSock;		    /* socket descriptor */
    struct sockaddr_in serv_addr;   /* server address structure */

    char *accountName;		    /* Account Name  */
    char *servIP;		    /* Server IP address  */
    unsigned short servPort;	    /* Server Port number */
    

    char sndBuf[SNDBUFSIZE];	    /* Send Buffer */
    char rcvBuf[RCVBUFSIZE];	    /* Receive Buffer */
    
    int balance;		    /* Account balance */

    char *option;            /* BALANCE/WITHDRAWAL */
    char *withdrawl;

    char amt[RCVBUFSIZE - BUFSIZE];
    char rcvBal[BUFSIZE];
    /* Get the Account Name from the command line */
    /* mySavings, myChecking, myRetirement, myCollege 
     * ./client <account name> <server ip> <port> <BAL/WITHDRAW>
    */
    if (argc < 5 && argc > 6) 
    {
	printf("Incorrect number of arguments. The correct format is:\n\taccountName serverIP serverPort");
	exit(1);
    } 
    accountName = argv[1];
    memset(&sndBuf, 0, SNDBUFSIZE);
    memset(&rcvBuf, 0, RCVBUFSIZE);

    /* Get the addditional parameters from the command line */
    /*	    FILL IN	*/
    servIP = argv[2];

    servPort = atoi(argv[3]);

    //For withdrawl / balance option
    // if(argc == 5) {
    //     option = argv[4];
    //     //strcpy accountName + option 
    //     snprintf(sndBuf, SNDBUFSIZE, "%s:%s", accountName, option);
    // } else
    // if(argc == 6) {
    //      option = argv[4];
    //      withdrawl = argv[5];
    //      //strcpy accountName + option + withdrawl
    //      snprintf(sndBuf, SNDBUFSIZE, "%s:%s:%s", accountName, option, withdrawl);
    // }
    option = argv[4];
    withdrawl = argv[5];

    snprintf(sndBuf, SNDBUFSIZE, "%s,%s,%s", accountName, option, withdrawl);

    

    /* Create a new TCP socket*/
    /*	    FILL IN	*/
    if((clientSock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        error_handling("failed to create a new TCP socket");

    /* Construct the server address structure */
    /*	    FILL IN	 */
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[2]); // servIP
    serv_addr.sin_port = htons(atoi(argv[3])); //servPort


    /* Establish connecction to the server */
    /*	    FILL IN	 */
    if(connect(clientSock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connection to serverrrr error");
        //error_handling("connection to server error");
    }
    //puts("connected\n");

    /* Send the string to the server */
    /*	    FILL IN	 */
    
    //strcpy(sndBuf, accountName);
    if(argc == 5) {
        printf("Client Sent : %s, %s\n", accountName, option);
    }else if(argc == 6){
        printf("Client Sent : %s, %s, %s\n", accountName, option, withdrawl);
    }
    //printf("Client Sent :  %s \n", sndBuf);
    if(send(clientSock, sndBuf, SNDBUFSIZE, 0) < 0) {

        perror("send to server failed");
        //error_handling("send to server failed");
        return 1;
    }
    //puts("data sent \n");

    /* Receive and print response from the server */
    /*	    FILL IN	 */
    if(recv(clientSock, rcvBuf, RCVBUFSIZE, 0) < 0) {
        error_handling("recv from server failed");
    }
    //puts("reply received\n");
    //printf("Client Rcvd : %s\n", rcvBuf);
    sscanf(rcvBuf, "%[^,],%s", rcvBal, amt);
    printf("amt = %d \n", amt[0]);
    balance = atoi(rcvBal);
    
    if(argc == 5) {
        printf("Client Rcvd : Balance Check Complete, Account Name : %s, Balance : %s \n", accountName, rcvBal);
        //printf("%s\n", accountName);
        //printf("Balance: %s\n", rcvBal);
    } else {
        //printf("amt = %s \n", amt);
        if(amt[0] == '\0'){
        //if(strlen(amt) == 0) {
            strcpy(amt, "Error!!");
            printf("Client Rcvd : Withdrawal Incomplete, %s\n", rcvBal);
        } else {
            printf("Client Rcvd : Withdrawal Complete, Account Name : %s, Withdrawal Amount : %s, Balance, %s \n", accountName, amt, rcvBal);

        }
        
        // printf("%s\n", accountName);
        // printf("Withdrawal Amount: %s\n", amt);
        // printf("Balance: %s\n", rcvBal);
    }
    //printf("BBBBBBalance is: %i\n", balance);

    close(clientSock);
    return 0;
}

void error_handling(char *message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

