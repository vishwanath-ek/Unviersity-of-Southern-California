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

Details updateTheTruck(Details receivedFromStore1) // This function updates the truck vector and also the outlet vector
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
	 int sockfd, receivedBytes;
	 char Address[INET_ADDRSTRLEN];
	 pid_t childpid;
	 socklen_t clilen;
	 struct sockaddr_in cliaddr, servaddr, udpservaddr, udpcliaddr;
     string readLine;
	 ifstream indata;
	 indata.open("Store-4.txt");
	 int counter=0;
     Details detailsToBePassed;
     int sentBytes;	

	 while(indata>>readLine)// Reading inputs from file store-4.txt
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
     sentBytes=send(sockfd,(char *)&detailsToBePassed,100,0);
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
	 
	 cout<<"Phase 1: store_4 has TCP port number "<<ntohs(cliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 1: The outlet vector <"<<items[0]<<","<<items[1]<<","<<items[2]<<"> for the store_4 has been sent to the central warehouse"<<endl;
	 cout<<"End of Phase 1 for store 4"<<endl; 
    
	 int closingStatus=close(sockfd);// Close the TCP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	// RECEIVE FROM STORE 3  
	/* Creating UDP socket */
	 int newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */ 
  	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(14732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(17732);
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
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
	 cout<<"Phase 2: Store_4 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	
	 Details receivedFromStore3;
	 
	 // Receiving data from Store 3
	 receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore3 , sizeof(receivedFromStore3), 0, (struct sockaddr *) &udpservaddr, & clilen );
	 if(receivedBytes==-1)// Handling error condition in recvfrom
	 { 
	   cout<<"recvfrom Error !"<<endl;
	   perror(strerror(errno));
	 }
	 
     cout<<"Phase 2: Store_4 received the truck-vector <"<<receivedFromStore3.numberOfCameras<<","<<receivedFromStore3.numberOfLaptops<<","<<receivedFromStore3.numberOfPrinters<<"> from store 3"<<endl;    
	 Details sendToStore1=updateTheTruck(receivedFromStore3);// Update the truck after receiving from store 3
     
	 closingStatus=close(newSockfd);// Close the UDP socket created
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	
	
	//SEND TO STORE 1
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
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpservaddr, sizeof(udpservaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
     clilen= sizeof(udpservaddr); 
	 
	/* Getting the port number and IP address for display below*/
	 getsockname(newSockfd,(struct sockaddr *)&udpservaddr,&clilen);
	 inet_ntop(udpservaddr.sin_family,&udpservaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 2: Store_4 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore1.numberOfCameras<<","<<sendToStore1.numberOfLaptops<<","<<sendToStore1.numberOfPrinters<<"> has been sent to store_1\nPhase 2: Store_4 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 
	// Sending Data to Store 1
	 sentBytes = sendto(newSockfd, (void *)&sendToStore1, sizeof(sendToStore1), 0, (const struct sockaddr *) & udpcliaddr, sizeof(udpcliaddr));
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
	 

	// RECEIVE FROM STORE 3
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
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpservaddr, sizeof(udpservaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 clilen= sizeof(udpservaddr); 
	 
	/* Getting the port number and IP address for display below*/
	 getsockname(newSockfd,(struct sockaddr *)&udpservaddr,&clilen);
	 inet_ntop(udpservaddr.sin_family,&udpservaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 2: Store_4 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 clilen=sizeof(udpcliaddr);
	 
	// Receiving Data from Store 3
	 receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore3 , sizeof(receivedFromStore3), 0, (struct sockaddr *) &udpcliaddr, &clilen);
	 if(receivedBytes<0)// Handling error in revfrom
	 {
	  cout<<"reecvfrom Error !"<<endl;
	  perror(strerror(errno));
     }
	 
	 cout<<"Phase 2: truck-vector <"<<receivedFromStore3.numberOfCameras<<","<<receivedFromStore3.numberOfLaptops<<","<<receivedFromStore3.numberOfPrinters<<"> has been received from Store_3"<<endl;
	 Details sendToWarehouse=updateTheTruck(receivedFromStore3); // Update the truck after receiving from store 3	 
     closingStatus=close(newSockfd);// Close the UDP socket
	 if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	//SENDING TO WAREHOUSE
	/* Creating UDP socket */	
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */   
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(20732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(32732);
  	 host = (struct hostent *)gethostbyname("localhost");
	 udpcliaddr.sin_addr = *((struct in_addr *)host->h_addr);
     
	// Binding the socket to IP address and port number   
	 bindValue=bind(newSockfd, (struct sockaddr *) &udpservaddr, sizeof(udpservaddr));
	 if(bindValue<0)// Handling error in bind function
	 {
	  cout<<"Bind Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 clilen= sizeof(udpservaddr); 
	 
	/* Getting the port number and IP address for display below*/ 
	 getsockname(newSockfd,(struct sockaddr *)&udpservaddr,&clilen);
	 inet_ntop(udpservaddr.sin_family,&udpservaddr.sin_addr,Address,sizeof(Address)); 	 
	 cout<<"Phase 2: Store_4 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	// Sending data to Warehouse
	 sentBytes = sendto(newSockfd, (void *)&sendToWarehouse, sizeof(sendToWarehouse), 0, (const struct sockaddr *) & udpcliaddr, sizeof(udpcliaddr));
	 if(sentBytes==-1)// Handling error condition in sendto
	 { 
	   cout<<"sendto Error !"<<endl;
	   perror(strerror(errno));
	 }
	 
	 cout<<"Phase 2: The updated truck-vector <"<<sendToWarehouse.numberOfCameras<<","<<sendToWarehouse.numberOfLaptops<<","<<sendToWarehouse.numberOfPrinters<<"> has been sent to central warehouse\nPhase 2: Store_4 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 cout<<"End of Phase 2 for store_4"<<endl;	
	 closingStatus=close(newSockfd);// Close the UDP socket created
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	return 0;
}

