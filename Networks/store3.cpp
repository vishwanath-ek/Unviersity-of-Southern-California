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

int items[3];

struct Details
{
    int numberOfCameras;
    int numberOfLaptops;
    int numberOfPrinters;
};

Details updateTheTruck(Details receivedFromStore1)// This function updates the truck vector and also the outlet vector
{
      int tempTruck[3]={0,0,0};
	  
      tempTruck[0]=receivedFromStore1.numberOfCameras;
	  tempTruck[1]=receivedFromStore1.numberOfLaptops;
	  tempTruck[2]=receivedFromStore1.numberOfPrinters;
	  
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
	 
	 receivedFromStore1.numberOfCameras=tempTruck[0];
	 receivedFromStore1.numberOfLaptops=tempTruck[1];
	 receivedFromStore1.numberOfPrinters=tempTruck[2];
	
   return receivedFromStore1; 
}

int main()
{
 /* Declaring variables below */
     struct hostent *host;
	 int sockfd;
	 char Address[INET_ADDRSTRLEN];
	 pid_t childpid;
	 socklen_t clilen;
	 struct sockaddr_in cliaddr, servaddr;
     string stringToSend="";
     string readLine;
	 ifstream indata;
	 indata.open("Store-3.txt");
	 int counter=0;
     Details detailsToBePassed;
     
	 while(indata>>readLine)// Reading inputs from file store-3.txt
	 {   counter++;
	     if(counter%2==0)
	         istringstream(readLine)>>items[counter/2 -1];
	 }

	 detailsToBePassed.numberOfCameras=items[0];
	 detailsToBePassed.numberOfLaptops=items[1];
	 detailsToBePassed.numberOfPrinters=items[2];

	/* Creating TCP socket */
	 sockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Initializing IP address and Port number parameters */
	 host=gethostbyname("localhost");
	 servaddr.sin_family = AF_INET;
	 host = (struct hostent *)gethostbyname("localhost");
	 servaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 servaddr.sin_port = htons(21732);

	//Request a connection to server  
	 int connValue=connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	 if(connValue<0)// Handling error for connect function
	 {
	  cout<<"Connect Error !"<<endl;
	  perror(strerror(errno));
	 }
	
    //Send data to the Warehouse	
     int sentBytes=send (sockfd,(char *)&detailsToBePassed,100,0);
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
	 cout<<"Phase 1: store_3 has TCP port number "<<ntohs(cliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 1: The outlet vector <"<<items[0]<<","<<items[1]<<","<<items[2]<<"> for the store_3 has been sent to the central warehouse"<<endl;
	 cout<<"End of Phase 1 for store 3"<<endl; 
     
	 int closingStatus=close(sockfd);// Close the TCP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	//RECEIVE FROM STORE 2
	/* Creating UDP socket */
	 int newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */  
	 struct sockaddr_in udpservaddr,udpcliaddr;
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(10732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(13732);
  	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 
	 Details receivedFromStore2;
	 
	// Binding the socket to IP address and port number
	 int bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 clilen= sizeof(udpcliaddr); 
	/* Getting the port number and IP address for display below*/
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 clilen=sizeof(udpservaddr);
	 cout<<"Phase 2: Store_3 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	// Receiving data from Store 2
	 int receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore2 , sizeof(receivedFromStore2), 0, (struct sockaddr *) &udpservaddr, &clilen);
	 if(receivedBytes==-1)// Handling error condition in recvfrom
	 { 
	   cout<<"recvfrom Error !"<<endl;
	   perror(strerror(errno));
	 }
	 
	 cout<<"Phase 2: Store_3 received the truck-vector <"<<receivedFromStore2.numberOfCameras<<","<<receivedFromStore2.numberOfLaptops<<","<<receivedFromStore2.numberOfPrinters<<"> from store 2"<<endl; 
	 closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
   
   
	// SEND TO STORE 4
	/* Creating UDP socket */
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 Details sendToStore4;
	 
	/* Initializing IP address and Port number parameters */  
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(17732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(14732);
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
	// Binding the socket to IP address and port number 
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 clilen= sizeof(udpcliaddr); 
	 sendToStore4=updateTheTruck(receivedFromStore2);// Update the truck after receiving from store 2
	
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 cout<<"Phase 2: Store_3 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore4.numberOfCameras<<","<<sendToStore4.numberOfLaptops<<","<<sendToStore4.numberOfPrinters<<"> has been sent to store_4\nPhase 2: Store_3 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 
	// Sending Data to Store 4
	 sentBytes = sendto(newSockfd, (char *)&sendToStore4, sizeof(sendToStore4), 0, (const struct sockaddr *) & udpservaddr, sizeof(udpservaddr));
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
	
	
	// RECEIVE FROM STORE 2
    /* Creating UDP socket */	 
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */   
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(11732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(15732);
   	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
	// Binding the socket to IP address and port number   
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 } 
	 clilen= sizeof(udpcliaddr); 
	 
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 clilen = sizeof(udpservaddr);
	 cout<<"Phase 2: Store_3 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl; 
	 
	// Receiving Data from Store 2 
	 receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore2 , sizeof(receivedFromStore2), 0, (struct sockaddr *) &udpservaddr, &clilen);
	 if(receivedBytes<0)// Handling error in revfrom
	 {
	  cout<<"reecvfrom Error !"<<endl;
	  perror(strerror(errno));
     }
	 
     cout<<"Phase 2: truck-vector <"<<receivedFromStore2.numberOfCameras<<","<<receivedFromStore2.numberOfLaptops<<","<<receivedFromStore2.numberOfPrinters<<"> has been received from Store_2"<<endl;
	 
	 sendToStore4=updateTheTruck(receivedFromStore2);// Update the truck after receiving from store 2	 
	 closingStatus=close(newSockfd);// Close the UDP socket
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	// SENDING TO STORE 4
	/* Creating UDP socket */
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */  
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
 	 udpservaddr.sin_port = htons(19732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(16732);
  	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
	// Binding the socket to IP address and port number    
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpcliaddr, sizeof(udpcliaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 clilen= sizeof(udpcliaddr); 
	 
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpcliaddr,&clilen);
	 inet_ntop(udpcliaddr.sin_family,&udpcliaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 2: Store_3 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl; 
	 
	// Sending data to Store 4
	 sentBytes = sendto(newSockfd, (char *)&sendToStore4, sizeof(sendToStore4), 0, (const struct sockaddr *) & udpservaddr, sizeof(udpservaddr));
	 if(sentBytes==-1)// Handling error condition in sendto
	 { 
	   cout<<"sendto Error !"<<endl;
	   perror(strerror(errno));
	 }
	  
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore4.numberOfCameras<<","<<sendToStore4.numberOfLaptops<<","<<sendToStore4.numberOfPrinters<<"> has been sent to store_4\nPhase 2: Store_3 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 cout<<"End of Phase 2 for store_3"<<endl;
	 closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	return 0;
}

