#include<stdlib.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<iostream>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<vector>
#include<wait.h>
#include<sstream>

using namespace std;

#define MAX_VALUE 5 // Defining MAX value for listening queue

struct Details
{
    int numberOfCameras;
    int numberOfLaptops;
    int numberOfPrinters;
};

int main(void)
{
   // Declaring variables below.
    struct hostent *host;
    int items[3]={0,0,0};
    int sum;
    stringstream ss;
    int varPipe[2];
	int listenfd, connfd, nBytes, nBytesForPipe, sockfd;
	char chararray[10];
	char Address[INET_ADDRSTRLEN];
	pid_t childpid;
    socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	Details globalDetails;
	int counter=0;
	

	globalDetails.numberOfCameras=0;
    globalDetails.numberOfLaptops=0;
    globalDetails.numberOfPrinters=0;
	
	
	/* Creating the TCP server socket */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd<0)// Handling error condition for socket function
	{
	 cout<<"Error in socket"<<endl;
	 perror(strerror(errno)); 
	}

	/* Initializing server socket address structure parameters */
	host=gethostbyname("localhost");
	servaddr.sin_family = AF_INET;
	host = (struct hostent *)gethostbyname("localhost");
	servaddr.sin_addr = *((struct in_addr *)host->h_addr);
	servaddr.sin_port = htons(21732);

	int bindValue=bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));// Binding the socket with the address and port number
    if(bindValue<0)// Handling error condition for Bind function
	{
	 cout<<"Bind Error !"<<endl;
	 perror(strerror(errno));
	}
	
	int listenValue=listen(listenfd, MAX_VALUE); /* MAX_VALUE defined as 5 */
	
    if(listenValue<0)// Handling error condition for Listen function
	{
	 cout<<"Listen function Error !"<<endl;
	 perror(strerror(errno));
	}

    inet_ntop(servaddr.sin_family, &servaddr.sin_addr, Address, sizeof Address);// Converting IP from numeric to presentaion format for displaying	
	cout<<"Phase 1: The central warehouse has TCP port number "<<ntohs(servaddr.sin_port)<<" and IP address "<<Address<<endl;
        
	while(true)// Loop till all data from four stores is accepted
	{ 
		clilen = sizeof(cliaddr);
		connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);// Accept the connection for particular client
       
	   if(connfd<0)// Handling error condition for accept function
		{
		 cout<<"Accept Error !"<<endl;
		 perror(strerror(errno));
		 continue;
		}
		counter++;
        pipe(varPipe);// Creating a PIPE for communication between the parent and child process
	
		inet_ntop(cliaddr.sin_family, &cliaddr.sin_addr, Address, sizeof Address);// Converting IP of client from numeric to presentation format
		int status;
		Details detailsFromChild;

		if((childpid=fork())==0)// For child process i,e,. if Process ID of child is 0 , execute the code below
		{
		 close(listenfd);// Close listening for child 
		 close(varPipe[0]);// Close read end of pipe for child
		 Details detailsGotFromClient;
		 nBytes = recv(connfd,(void *)&detailsGotFromClient,100, 0);// Receive data from store
		 cout<<"Phase 1: The central warehouse received information from store "<<counter<<endl;
		 write(varPipe[1],(char *)&detailsGotFromClient,100);// Write the data received on to the PIPE
		 close(varPipe[1]);// Close the write end of the PIPE 
		 exit(0);
        }
		else
       {
		close(varPipe[1]);// Close the write end of PIPE for parent
		Details detailsFromChild; 
      
		if(wait(&status)>0)// Wait for child to finish  
		 nBytesForPipe=read(varPipe[0],(void *)&detailsFromChild,100);// Read the data from read end of PIPE
	  
	  // Update the total number of items as received
		items[0]=items[0]+detailsFromChild.numberOfCameras;
		items[1]=items[1]+detailsFromChild.numberOfPrinters;
		items[2]=items[2]+detailsFromChild.numberOfLaptops;
	   }
     
	   if(counter==4)// Check whether all data is received i.e., from all stores
        break;
   } 
    
	for(int i=0; i<3;i++)// Update the items thus , truck vector is generated
	 if(items[i]>0)
	   items[i]=0;
	 else
      items[i]=-items[i];	 
	  
     cout<<"End of Phase 1 for the central warehouse"<<endl;	  
	 int closingStatus=close(listenfd);// Close the TCP socket
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 } 
	 
	 
    // SEND TO STORE 1	
	// Create a new Socket for UDP transfer 
	 int newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 struct sockaddr_in udpcliaddr, udpservaddr;

	/* IP and Port declarations below */  
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(31732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(5732);
	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
 
    // Bind the socket with IP address and Port number
	 bindValue=bind(newSockfd, (struct sockaddr *)&udpservaddr, sizeof(udpservaddr));
	 if(bindValue<0)// Handling error condition for bind function
		{
		 cout<<"Accept Error !"<<endl;
		 perror(strerror(errno));
		} 
	 
     //Converting into presentation format from	numeric 
	 inet_ntop(udpservaddr.sin_family, &udpservaddr.sin_addr, Address, sizeof Address);
	 
	 cout<<"Phase 2: The central warehouse has UDP port number "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	 clilen=sizeof(udpcliaddr);
	
	 Details sendToStore1;// The data to be sent is stored in this structure object
	 sendToStore1.numberOfCameras=items[0];
	 sendToStore1.numberOfLaptops=items[1];
	 sendToStore1.numberOfPrinters=items[2];
	 
     cout<<"Phase 2: Sending the truck-vector to outlet store store_1. The truck vector value is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">."<<endl;	
	 /* Sending the data to store 1 */
	 int sentBytes = sendto(newSockfd,(char *)&sendToStore1, sizeof(sendToStore1),0, (struct sockaddr *)& udpcliaddr, clilen); 
	 if(sentBytes==-1)// Handling error in send function
		perror(strerror(errno));
	 closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	
	//RECEIVE FROM STORE 4
	// Create a new Socket for UDP transfer
     newSockfd = socket(AF_INET, SOCK_DGRAM, 0 );

	 /* IP and Port declarations below */   
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
 	 udpservaddr.sin_port = htons(32732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpcliaddr.sin_port = htons(20732);
     
	 // Bind the socket with IP address and Port number 
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpservaddr, sizeof(udpservaddr));
	 if(bindValue<0)// Handling error condition for bind function
		{
		 cout<<"Accept Error !"<<endl;
		 perror(strerror(errno));
		} 
		
	 //Converting into presentation format from	numeric 
	 inet_ntop(udpservaddr.sin_family, &udpservaddr.sin_addr, Address, sizeof Address);
	 cout<<"Phase 2: The central warehouse has UDP port number "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	 //Converting into presentation format from	numeric
	 clilen = sizeof(udpcliaddr);
     Details fromStore4;
	 
	 //Receiving data from Store 4
	 int receivedBytes = recvfrom(newSockfd, (void *)&fromStore4 , sizeof(fromStore4), 0, (struct sockaddr *) &udpcliaddr, &clilen);
	 if(receivedBytes<0)// Handling error in revfrom
	 {
	  cout<<"reecvfrom Error !"<<endl;
	  perror(strerror(errno));
     }
	 cout<<"Phase 2: The final truck-vector is received from the outlet store store 4, the truck-vector value is: <"<<fromStore4.numberOfCameras<<","<<fromStore4.numberOfLaptops<<","<<fromStore4.numberOfPrinters<<">"<<endl;
	 cout<<"End of Phase 2 for the central warehouse"<<endl;
	 closingStatus=close(newSockfd);// Close the UDP socket
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	 return 0;
}

