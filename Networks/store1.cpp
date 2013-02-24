#include<stdlib.h>
#include<sys/socket.h>
#include<fstream>
#include<sys/types.h>
#include<errno.h>
#include<string.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<stdio.h>
#include<iostream>
#include<sstream>
#include<netinet/in.h>
#include<sys/stat.h>
#include<fcntl.h>
using namespace std;

struct Details
{
    int numberOfCameras;
    int numberOfLaptops;
    int numberOfPrinters;
};
 
int items[3]; 
 
Details updateTheTruck(Details receivedFromServer)// This function updates the truck vector and also the outlet vector
{
      int tempTruck[3]={0,0,0}; 
      tempTruck[0]=receivedFromServer.numberOfCameras;
	  tempTruck[1]=receivedFromServer.numberOfLaptops;
	  tempTruck[2]=receivedFromServer.numberOfPrinters;
	  
	 for(int i=0;i<3;i++)
      if(items[i] + tempTruck[i] >= 0)
	  {
	    tempTruck[i]=tempTruck[i] + items[i];
	    items[i]=0;
	  }
	  else
	  {
	   items[i]=items[i]+tempTruck[i];
       tempTruck[i]=0;	  
	  }
	 
	 receivedFromServer.numberOfCameras=tempTruck[0];
	 receivedFromServer.numberOfLaptops=tempTruck[1];
	 receivedFromServer.numberOfPrinters=tempTruck[2];
	
   return receivedFromServer; 
}
 
int main()
{
    /* Declaring variables below */ 
     struct hostent *host;
	 int sockfd,sockfd1,sentBytes;
	 char Address[INET_ADDRSTRLEN];
	 pid_t childpid;
	 socklen_t clilen;
	 struct sockaddr_in cliaddr, servaddr;
     string stringToSend="";
     string readLine;
 	 ifstream indata;
	 indata.open("Store-1.txt");
	 int counter=0;
     Details detailsToBePassed;
 

	 while(indata>>readLine)// Reading inputs from file store-1.txt
	 {   counter++;
	     if(counter%2==0)
	         istringstream(readLine)>>items[counter/2 -1];
            
	 }
    
	 detailsToBePassed.numberOfCameras=items[0];
	 detailsToBePassed.numberOfLaptops=items[1];
	 detailsToBePassed.numberOfPrinters=items[2];

	/* Creating the TCP socket */
	 sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Initializing IP address and Port number parameters */
	 host = (struct hostent *)gethostbyname("localhost");
	 servaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 servaddr.sin_family = AF_INET;
	 servaddr.sin_port = htons(21732);
     
	//Request a connection to server by connect and also handling the error condition for connect below
	 if((connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)))==-1)
	 {
		perror("connect error");
		exit(-1);	
	 }

	//Send data to the Warehouse  
     sentBytes = send (sockfd,(char *)&detailsToBePassed,100,0);
	 if(sentBytes<0)// Handling error in send function
	 {
	  cout<<"Send Error !"<<endl;
	  perror(strerror(errno));
	 }
	 clilen= sizeof(cliaddr); 
	 
	// Get client information, connected to the socket 	  
	 getsockname(sockfd,(struct sockaddr *)&cliaddr,&clilen);
	 
	// Convert to presentation format from numeric 
	 inet_ntop(cliaddr.sin_family,&cliaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 1: Store 1 has TCP port number "<<ntohs(cliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 1: The outlet vector <"<<detailsToBePassed.numberOfCameras<<","<<detailsToBePassed.numberOfLaptops<<","<<detailsToBePassed.numberOfPrinters<<">"<<endl;
	 cout<<"End of Phase 1 for store 1"<<endl;
	 int closingStatus=close(sockfd);// Close the TCP socket created
  	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 } 
	
	//RECEIVE FROM WAREHOUSE 
     int newSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */  
     struct sockaddr_in udpcliaddr, udpservaddr;    
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(31732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(5732);
	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 
	// Binding the socket to IP address and port number  
	 int bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 clilen=sizeof(udpcliaddr);
	 
	/* Getting the port number and IP address for display below*/  
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 cout<<"Phase 2: Store_1 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	 Details receivedFromServer;
	 socklen_t servlen = sizeof(udpservaddr);
	
    // Receiving data from Store 2	
	 int receivedBytes = recvfrom(newSockfd,(void *)&receivedFromServer,sizeof(receivedFromServer), 0,(struct sockaddr *)&udpservaddr, &servlen);
	 if(receivedBytes==-1)// Handling error condition in recvfrom
	 { 
	   cout<<"recvfrom Error !"<<endl;
	   perror(strerror(errno));
	 } 
	 cout<<"Phase 2: Store_1 received the truck-vector <"<<receivedFromServer.numberOfCameras<<","<<receivedFromServer.numberOfLaptops<<","<<receivedFromServer.numberOfPrinters<<"> from the central warehouse."<<endl;

	 Details sendToStore2=updateTheTruck(receivedFromServer);// Update the truck after receiving from Warehouse
	 closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	
	// SEND TO STORE 2
	/* Creating UDP socket */ 
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */    
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(9732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(6732);
	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
    
	// Binding the socket to IP address and port number 
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
     if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 clilen=sizeof(udpcliaddr);
	 
	/* Getting the port number and IP address for display below*/  
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 2: Store_1 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore2.numberOfCameras<<","<<sendToStore2.numberOfLaptops<<","<<sendToStore2.numberOfPrinters<<"> has been sent to store_2\nPhase 2: Store_1 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 
	// Sending Data to Store 2 
	 sentBytes = sendto(newSockfd, (char *)&sendToStore2, sizeof(sendToStore2), 0, (const struct sockaddr *) & udpservaddr, sizeof(udpservaddr));
	 if(sentBytes==-1)// Handling error condition in sendto
	 { 
	   cout<<"sendto Error !"<<endl;
	   perror(strerror(errno));
	 }
	 closingStatus=close(newSockfd);// Close the UDP socket
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }

	 
	//RECEIVE FROM STORE 4
	/* Creating UDP socket */	 
     newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
    
	/* Initializing IP address and Port number parameters */    
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(18732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(7732);
	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
	// Binding the socket to IP address and port number   	 
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 } 	  
	 clilen=sizeof(udpcliaddr);
	 
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 2: Store_1 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;	 
	 Details receivedFromStore4;
	 
	// Receiving Data from Store 4   
	 receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore4 , sizeof(receivedFromStore4), 0, (struct sockaddr *) &udpservaddr, & clilen );
     if(receivedBytes<0)// Handling error in revfrom
	 {
	  cout<<"reecvfrom Error !"<<endl;
	  perror(strerror(errno));
     }  
	 cout<<"Phase 2: truck-vector <"<<receivedFromStore4.numberOfCameras<<","<<receivedFromStore4.numberOfLaptops<<","<<receivedFromStore4.numberOfPrinters<<"> has been received from Store_4"<<endl;
	 sendToStore2=updateTheTruck(receivedFromStore4);// Update the truck after receiving from store 4
     closingStatus=close(newSockfd);// Close the UDP socket
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	
	
	//SEND TO STORE 2
	/* Creating UDP socket */
 	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	
	/* Initializing IP address and Port number parameters */   
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(11732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(8732);
     host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);

	// Binding the socket to IP address and port number     
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }

	 // Sending data to Store 3
	 sentBytes = sendto(newSockfd, (char *)&sendToStore2, sizeof(sendToStore2), 0, (const struct sockaddr *) & udpservaddr, sizeof(udpservaddr));
	 clilen=sizeof(udpcliaddr);
	
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 cout<<"Phase 2: Store_1 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore2.numberOfCameras<<","<<sendToStore2.numberOfLaptops<<","<<sendToStore2.numberOfPrinters<<"> has been sent to store_2\nPhase 2: Store_1 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;	
	
     closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 cout<<"End of Phase 2 for Store_1"<<endl;
	 return 0;
}

