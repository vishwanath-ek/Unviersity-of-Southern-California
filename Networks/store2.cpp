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
	 int newSockfd;
	 char Address[INET_ADDRSTRLEN];
	 pid_t childpid;
	 socklen_t clilen;
	 struct sockaddr_in cliaddr, servaddr;
     string stringToSend="";
     string readLine;
	 ifstream indata;
	 indata.open("Store-2.txt");
	 int counter=0;

     Details detailsToBePassed;

     char stringConvertedToCharArray[10];

	 while(indata>>readLine)// Reading inputs from file store-2.txt
	 {   counter++;
	     if(counter%2==0)
	         istringstream(readLine)>>items[counter/2 -1];
	 }

	 detailsToBePassed.numberOfCameras=items[0];
	 detailsToBePassed.numberOfLaptops=items[1];
	 detailsToBePassed.numberOfPrinters=items[2];

	/* Creating the TCP socket */
	 newSockfd = socket(AF_INET, SOCK_STREAM, 0);

	/* Initializing IP address and Port number parameters */
	 servaddr.sin_family = AF_INET;
	 host = (struct hostent *)gethostbyname("localhost");
	 servaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 servaddr.sin_port = htons(21732);

	 //Request a connection to server  
	 int connValue=connect(newSockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	 if(connValue<0)// Handling error for connect function
	 {
	  cout<<"Connect Error !"<<endl;
	  perror(strerror(errno));
	 }

     int sentBytes;
	//Send data to the Warehouse
	 sentBytes=send (newSockfd,(char *)&detailsToBePassed,100,0);
	 if(sentBytes<0)// Handling error in send function
	 {
	  cout<<"Send Error !"<<endl;
	  perror(strerror(errno));
	 }
	 clilen= sizeof(cliaddr);
     
    // Get client information, connected to the socket 	 
 	 getsockname(newSockfd,(struct sockaddr *)&cliaddr,&clilen);
	
	// Convert to presentation format from numeric 
	 inet_ntop(cliaddr.sin_family,&cliaddr.sin_addr,Address,sizeof(Address));
	 
	 cout<<"Phase 1: Store 2 has TCP port number "<<ntohs(cliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 cout<<"Phase 1: The outlet vector <"<<items[0]<<","<<items[1]<<","<<items[2]<<"> for store_2 has been sent to the central warehouse"<<endl;
     cout<<"End of Phase 1 for store_2"<<endl;
	 int closingStatus=close(newSockfd);// Close the TCP socket created
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }

	 Details receivedFromStore1;
	 struct sockaddr_in udpcliaddr, udpservaddr;
	 
	//RECEIVE FROM STORE 1
	/* Creating UDP socket */
     newSockfd = socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */   
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(6732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(9732);
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
	 cout<<"Phase 2: Store_2 has UDP port "<<ntohs(udpcliaddr.sin_port)<<" and IP address "<<Address<<endl;
	 socklen_t servlen = sizeof(udpservaddr);
    
	// Receiving data from Store 2
	 int receivedBytes = recvfrom(newSockfd,(void *)&receivedFromStore1,sizeof(receivedFromStore1), 0,(struct sockaddr *)&udpservaddr, &servlen);
	 if(receivedBytes==-1)// Handling error condition in recvfrom
	 { 
	   cout<<"recvfrom Error !"<<endl;
	   perror(strerror(errno));
	 }
	 
	 cout<<"Phase 2: Store_2 received the truck-vector <"<<receivedFromStore1.numberOfCameras<<","<<receivedFromStore1.numberOfLaptops<<","<<receivedFromStore1.numberOfPrinters<<"> from store 1"<<endl;

	 Details sendToStore3=updateTheTruck(receivedFromStore1);// Update the truck after receiving from store 1
	 closingStatus=close(newSockfd);// Close the UDP socket created

	// SEND TO STORE 3
	/* Creating UDP socket */ 
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */   
 	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(10732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(13732);
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
	 
	 cout<<"Phase 2: Store_2 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	// Sending Data to Store 3 
	 sentBytes = sendto(newSockfd, (char *)&sendToStore3 , sizeof(sendToStore3), 0, (struct sockaddr *) &udpcliaddr, clilen);
	 if(sentBytes==-1)// Handling error condition in sendto
	 { 
	   cout<<"sendto Error !"<<endl;
	   perror(strerror(errno));
	 } 
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore3.numberOfCameras<<","<<sendToStore3.numberOfLaptops<<","<<sendToStore3.numberOfPrinters<<"> has been sent to store_3\nPhase 2: Store_2 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 closingStatus=close(newSockfd);// Close the UDP socket
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 }
	 
	 
	//RECEIVE STORE 1
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
	 cout<<"Phase 2: Store_2 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	 
	// Receiving Data from Store 2  
	 receivedBytes = recvfrom(newSockfd, (void *)&receivedFromStore1 , sizeof(receivedFromStore1), 0, (struct sockaddr *) &udpcliaddr, &clilen);
	 if(receivedBytes<0)// Handling error in revfrom
	 {
	  cout<<"reecvfrom Error !"<<endl;
	  perror(strerror(errno));
     }
	 
	 cout<<"Phase 2: truck-vector <"<<receivedFromStore1.numberOfCameras<<","<<receivedFromStore1.numberOfLaptops<<","<<receivedFromStore1.numberOfPrinters<<"> has been received from Store_1"<<endl;
	 sendToStore3=updateTheTruck(receivedFromStore1);// Update the truck after receiving from store 2
     closingStatus=close(newSockfd);// Close the UDP socket

	 
	//SEND TO STORE 3
	/* Creating UDP socket */
	 newSockfd= socket(AF_INET, SOCK_DGRAM, 0);
	 
	/* Initializing IP address and Port number parameters */   
	 host = (struct hostent *)gethostbyname("localhost");
	 udpservaddr.sin_addr = *((struct in_addr *)host->h_addr);
	 udpservaddr.sin_port = htons(12732);
	 udpservaddr.sin_family = AF_INET;
	 udpcliaddr.sin_family = AF_INET;
	 udpcliaddr.sin_port = htons(15732);
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
	 cout<<"Phase 2: Store_2 has UDP port "<<ntohs(udpservaddr.sin_port)<<" and IP address "<<Address<<endl;
	
	// Sending data to Store 3
	 sentBytes = sendto(newSockfd, (char *)&sendToStore3, sizeof(sendToStore3), 0, (const struct sockaddr *) & udpcliaddr, sizeof(udpcliaddr));
	 if(sentBytes==-1)// Handling error condition in sendto
	 { 
	   cout<<"sendto Error !"<<endl;
	   perror(strerror(errno));
	 }
	 
	 cout<<"Phase 2: The updated truck-vector <"<<sendToStore3.numberOfCameras<<","<<sendToStore3.numberOfLaptops<<","<<sendToStore3.numberOfPrinters<<"> has been sent to store_3\nPhase 2: Store_2 updated outlet-vector is <"<<items[0]<<","<<items[1]<<","<<items[2]<<">"<<endl;
	 cout<<"End of Phase 2 for store_2"<<endl;
	 closingStatus=close(newSockfd);// Close the UDP socket created
     if(closingStatus<0)//Handling close Error
	 {
	  cout<<"Close socket Error !"<<endl;
	  perror(strerror(errno));
	 } 

	return 0;
}

