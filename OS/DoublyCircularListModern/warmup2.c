#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <errno.h>
#include "my402list.h"
#include "cs402.h"
 
My402List tokenQueue;
My402List arrvQueue; 
My402List servQueue;
pthread_t tokenThread;
pthread_t arrvThread;
pthread_t servThread;

FILE *file; 
int B=0,numberOfPackets=0,remainingPackets=0,flag=0,tokens=0,numPacQ1=0,numPacQ2=0,numPacS=0,droppedPac=0,tokenFin=0,arrvFin=0,intFlag=0,intCancel=0;  
double r=0,lambda=0,mu=0,interArrivalTime=0,totalTokens=0,droppedTokens=0,forStdDev=0;
double totalServiceTime=0,timeSpentSys=0,totalTimeSpentArrv=0,totalTimeSpentServ=0,totalTimeSpentS=0;
pthread_mutex_t forPrint= PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t forQueue= PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t servCV= PTHREAD_COND_INITIALIZER;

struct timeval start,end;

typedef struct
{
 double arrvtime;
 int tokens;
 double servtime;
 int id;
 struct timeval pactime;
 struct timeval srvAppTime;
}packet;
 
void *tokenThreadFunc(void *ptr)
{
 sigset_t set;
 sigemptyset(&set);
 sigaddset(&set,SIGINT); 
 sigprocmask(SIG_UNBLOCK,&set,0);

 double sec=0; 
 sec=(double)(1/(*(double *)ptr)); 
 int z=1;
 double t=0,diff=0;
 struct timeval now,reltime;int someflag=0;
 
 pthread_mutex_lock(&forPrint);
 if(remainingPackets!=0)
  someflag=1;
 else
  someflag=0; 
 pthread_mutex_unlock(&forPrint);
 
 while(someflag==1 && intFlag==0) 
 {
  usleep(1000000*sec);  
  totalTokens++;
  pthread_mutex_lock(&forQueue);

  if(My402ListLength(&tokenQueue) < B && remainingPackets!=0)
  {       
   My402ListAppend(&tokenQueue,&z);    
   gettimeofday(&now,0);
   t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
   t=t/1000.0f;
   if(My402ListLength(&tokenQueue)==1)
   { 
    pthread_mutex_lock(&forPrint);
    printf("%011.3fms: token t%d arrives, token bucket now has %d token\n",t,z,My402ListLength(&tokenQueue));
    pthread_mutex_unlock(&forPrint);    
   }
   else
   {
    pthread_mutex_lock(&forPrint);
    printf("%011.3fms: token t%d arrives, token bucket now has %d tokens\n",t,z,My402ListLength(&tokenQueue));
    pthread_mutex_unlock(&forPrint);    
   }
    if(My402ListLength(&arrvQueue)!=0)   
     if(My402ListLength(&tokenQueue)>= ((packet *)(My402ListFirst(&arrvQueue)->obj))->tokens)
     {
	  int j=0;
      for(j=0;j < ((packet *)(My402ListFirst(&arrvQueue)->obj))->tokens ;j++)
        My402ListUnlink(&tokenQueue,My402ListFirst(&tokenQueue));
		
	  gettimeofday(&(((packet *)(My402ListFirst(&arrvQueue)->obj))->srvAppTime),0);
	  My402ListAppend(&servQueue,(packet *)(My402ListFirst(&arrvQueue)->obj));
	  pthread_cond_broadcast(&servCV);
	  
	  pthread_mutex_lock(&forPrint);
	  numPacQ2++;
	  pthread_mutex_unlock(&forPrint);
 
	  gettimeofday(&now,0);	
      reltime=((packet *)(My402ListFirst(&arrvQueue)->obj))->pactime;   	 
	  diff= (now.tv_sec*1000000+now.tv_usec) - (reltime.tv_sec*1000000 + reltime.tv_usec);
	  diff=diff/1000;
	  gettimeofday(&now,0);
      t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
      t=t/1000;   
	  totalTimeSpentArrv=totalTimeSpentArrv+diff;
	
	  pthread_mutex_lock(&forPrint);
	  printf("%011.3fms: packet p%d leaves Q1, time in Q1 = %.3f, token Bucket has %d token\n",t,((packet *)(My402ListFirst(&arrvQueue)->obj))->id,diff,My402ListLength(&tokenQueue));
	   
	  gettimeofday(&now,0);
      t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
      t=t/1000;   
	  printf("%011.3fms: p%d enters Q2\n",t,((packet *)(My402ListFirst(&arrvQueue)->obj))->id);
	  pthread_mutex_unlock(&forPrint);
	  My402ListUnlink(&arrvQueue,My402ListFirst(&arrvQueue));
	  remainingPackets--;
	  pthread_mutex_lock(&forPrint);   
	  pthread_mutex_unlock(&forPrint);	  
	 }      	 	  	
  }
  else
  {    
    gettimeofday(&now,0);
    t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
    t=t/1000;   
	droppedTokens++;
	pthread_mutex_lock(&forPrint);
    printf("%011.3fms: token t%d arrives, dropped\n",t,z);
    pthread_mutex_unlock(&forPrint);      
  }
  pthread_mutex_unlock(&forQueue);

  pthread_mutex_lock(&forPrint);
  if(remainingPackets>0)
   someflag=1;
  else
   someflag=0;
  pthread_mutex_unlock(&forPrint);      
   
  z++;
  if(intFlag==1)
   break;
}
 
 
 if(intFlag==1)
  {
   int x=0;
   pthread_mutex_lock(&forQueue); 
   for(x=0;x<My402ListLength(&tokenQueue);x++)
    free((int *)(My402ListFirst(&tokenQueue)->obj));
   
   My402ListUnlinkAll(&tokenQueue);
   pthread_mutex_unlock(&forQueue);     
  }

 tokenFin=1;
 pthread_cond_signal(&servCV);
 pthread_exit((void*)1);
}

void *arrvThreadFunc(void *ptr)
{
 sigset_t set;
 sigemptyset(&set);
 sigaddset(&set,SIGINT); 
 sigprocmask(SIG_UNBLOCK,&set,0);
 
 struct timeval reltime,now;
 double prev=0,current=0,sec=0;
 if(flag==0)
  sec=(double)(1/(*(double *)ptr)); 
 double t=0,diff=0;
 int i=1;
 char buff[500];
 memset(buff,0,sizeof(buff));
 
 while(i<= numberOfPackets && intFlag==0)
 {
  packet *pac=(packet *)malloc(sizeof(packet));
  memset(pac,0,sizeof(packet));
  pac->id=i;
    
  if(flag==0)    
  {
   pac->arrvtime=sec*1000;
   pac->tokens=tokens;     
  } 
  else
  {
   if(fgets(buff,sizeof(buff),file)!=NULL)
    {
	 char *first=buff; 
   
	 if(*first==' ')
	 {	 
	  fprintf(stderr,"Error in File.\n");
	  exit(EXIT_FAILURE);
	 }
	
	 int j=0;int num=0;
	 for(j=0;j<strlen(buff)-1 && buff[j]!=' ' && buff[j]!='\t';j++)
	  num=num*10+(buff[j]-'0');
	 pac->arrvtime=num;
	
	 while(j<strlen(buff)-1 && (buff[j]==' ' || buff[j]=='\t'))
      j++;
	 num=0;	
	 for(j=j;j<strlen(buff)-1 && buff[j]!=' ' && buff[j]!='\t';j++) 	
	  num=num*10+(buff[j]-'0');
	 pac->tokens=num;	
	 
	 while(j<strlen(buff)-1 && (buff[j]==' ' || buff[j]=='\t'))
      j++;
	 num=0;	
	 for(j=j;j<strlen(buff)-1 && buff[j]!=' ' && buff[j]!='\t';j++)
	  num=num*10+(buff[j]-'0');
	 pac->servtime=num;
	 
	 memset(buff,0,sizeof(buff));
	}
  }
  
  usleep((pac->arrvtime)*1000);   
  gettimeofday(&(pac->pactime),0); 
  if(pac->id==1)
  {   
   prev =0;
  }  
  if(pac->tokens > B)
   {
    gettimeofday(&now,0);
    t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
    t=t/1000; 
    current=t;	
	interArrivalTime=interArrivalTime+(current-prev);
	prev=current;
    pthread_mutex_lock(&forPrint);
	printf("%011.3fms: packet p%d arrives, dropped\n",t,i);
	remainingPackets--;	     
	pthread_mutex_unlock(&forPrint);	   	  
	droppedPac++;
	free(pac);	
	i++;
    continue;
   }
   else
   {	
    pthread_mutex_lock(&forPrint);
    gettimeofday(&now,0);
    t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
    t=t/1000;   
	
    current=(((pac->pactime).tv_sec*1000000+pac->pactime.tv_usec)-(start.tv_sec*1000000+start.tv_usec))/1000;	
    printf("%011.3fms: packet p%d arrives, needs %d tokens, inter-arrival time = %lfms\n",t,i,pac->tokens,current-prev);
    interArrivalTime=interArrivalTime+(current-prev);
    pthread_mutex_unlock(&forPrint);    	
    prev=current;
    
    if(flag==0)
     pac->servtime=(1.0/mu)*1000;
    gettimeofday(&(pac->pactime),0); 
    pthread_mutex_lock(&forQueue);
    My402ListAppend(&arrvQueue,pac);
    numPacQ1++;
    gettimeofday(&now,0);
    t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
    t=t/1000;   
	 
	pthread_mutex_lock(&forPrint);
	printf("%011.3fms: p%d enters Q1\n",t,pac->id);
	pthread_mutex_unlock(&forPrint);    	
  
 	 if(My402ListLength(&tokenQueue) >= ((packet *)(My402ListFirst(&arrvQueue)->obj))->tokens)
     {        
	  int j; 
	  for(j=0;j<pac->tokens;j++)
	   My402ListUnlink(&tokenQueue,My402ListFirst(&tokenQueue));	   
      
          gettimeofday(&(((packet *)(My402ListFirst(&arrvQueue)->obj))->srvAppTime),0);	   
	  My402ListAppend(&servQueue,((packet *)(My402ListFirst(&arrvQueue)->obj)));
	  
	  pthread_mutex_lock(&forPrint);
	  numPacQ2++;
	  pthread_mutex_unlock(&forPrint);
      
	  pthread_cond_broadcast(&servCV);
	  
      
	  packet *p=(packet *)(My402ListFirst(&arrvQueue)->obj);
	  reltime=p->pactime;
	  gettimeofday(&now,0);	      
	  My402ListUnlink(&arrvQueue,My402ListFirst(&arrvQueue));	   	
	  diff= (now.tv_sec*1000000+now.tv_usec) - (reltime.tv_sec*1000000 + reltime.tv_usec);
	  diff=diff/1000;
	  gettimeofday(&now,0);
          t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
          t=t/1000;   
	  
	  totalTimeSpentArrv=totalTimeSpentArrv+diff;
	  		
	  pthread_mutex_lock(&forPrint);
	  printf("%011.3fms: packet p%d leaves Q1, time in Q1 = %.3f, token Bucket now has %d token\n",t,p->id,diff,My402ListLength   (&tokenQueue));	  
	  gettimeofday(&now,0);  
          t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
          t=t/1000;
	  printf("%011.3fms: p%d enters Q2\n",t,p->id);
	  remainingPackets--;	
	  pthread_mutex_unlock(&forPrint);	   
     }    
   pthread_mutex_unlock(&forQueue);  
  i++; 
  }   

  if(intFlag==1)
   break;
 }
 
 if(intFlag==1)
  {
   int x=0;
   pthread_mutex_lock(&forQueue); 
   for(x=0;x<My402ListLength(&arrvQueue);x++)
    free((packet *)(My402ListFirst(&arrvQueue)->obj));
   
   My402ListUnlinkAll(&arrvQueue);
   pthread_mutex_unlock(&forQueue);     
  }
 arrvFin=1;
 pthread_cond_signal(&servCV);
  
 pthread_exit((void*)1);
}

void *servThreadFunc(void *ptr)
{
 sigset_t set;
 sigemptyset(&set);
 sigaddset(&set,SIGINT); 
 sigprocmask(SIG_UNBLOCK,&set,0);
 
 struct timeval now;

 double t=0,diff=0;
 while(1)
 { 
  if((tokenFin==1 && arrvFin==1 && My402ListLength(&servQueue)==0)||intFlag==1)
   break;
  pthread_mutex_lock(&forQueue);  
  while(My402ListLength(&servQueue)==0 && (tokenFin!=1 || arrvFin!=1))
  { 
   pthread_cond_wait(&servCV,&forQueue);
  }  
  packet *p=NULL;  
   double t1=0;
  if(tokenFin==1 && arrvFin==1 && My402ListLength(&servQueue)==0)
   break;
  if(My402ListLength(&servQueue)!=0 && intFlag==0)
  {   
   p=(packet *)(My402ListFirst(&servQueue)->obj);
   My402ListUnlink(&servQueue,My402ListFirst(&servQueue));
  
 
   gettimeofday(&now,0);
   t=(now.tv_sec*1000000+now.tv_usec) - (start.tv_sec*1000000+start.tv_usec);
   diff=t-(((p->srvAppTime).tv_sec*1000000 + (p->srvAppTime).tv_usec) - (start.tv_sec*1000000+start.tv_usec));
   t=t/1000;     
   diff=diff/1000;
   totalTimeSpentServ=totalTimeSpentServ + diff;
  
   pthread_mutex_lock(&forPrint);
   printf("%011.3fms: p%d begins service at S, time in Q2 = %.3fms\n",t,p->id,diff);
   pthread_mutex_unlock(&forPrint);  
  
   gettimeofday(&now,0);
  
   t1=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);  
  }
   pthread_mutex_unlock(&forQueue); 
   usleep((p->servtime)*1000);
   pthread_mutex_lock(&forQueue); 
 if(p!=NULL)
  {
   gettimeofday(&now,0);
   double t2=0;
   t2=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec) - t1;  
  
   totalServiceTime=totalServiceTime+t2/1000;
   numPacS++;
   gettimeofday(&now,0);
  
   gettimeofday(&now,0);
   t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
   t=t/1000;
 
   totalTimeSpentS=totalTimeSpentS+(p->servtime);
   pthread_mutex_lock(&forPrint);
   printf("%011.3fms: p%d departs from S, serivce time = %lfms time in system = %.3fms\n",t,p->id,p->servtime,diff);
   pthread_mutex_unlock(&forPrint);  

   gettimeofday(&now,0);
   t=(now.tv_sec*1000000+now.tv_usec)-(start.tv_sec*1000000+start.tv_usec);
   diff=t - (((p->pactime).tv_sec*1000000 + (p->pactime).tv_usec) - (start.tv_sec*1000000+start.tv_usec));
   diff=diff/1000;
   timeSpentSys=timeSpentSys+diff;
   forStdDev=forStdDev+(diff)*(diff); 
 }
 pthread_mutex_unlock(&forQueue);   
  if(intFlag==1)   
    break;
 }
  
   if(intFlag==1)
  {
   int x=0;
   pthread_mutex_lock(&forQueue);     
   for(x=0;x<My402ListLength(&servQueue);x++)
    free((packet *)(My402ListFirst(&servQueue)->obj));
   
   My402ListUnlinkAll(&servQueue);
   pthread_mutex_unlock(&forQueue);     
  }       
  pthread_exit((void*)1);
}

void handlerInt(int type)
{
 intFlag=1;
 if(!pthread_mutex_trylock(&forQueue))
   pthread_mutex_unlock(&forQueue);
 else
   pthread_mutex_unlock(&forQueue);
 pthread_cancel(tokenThread);
 pthread_cancel(arrvThread); 

 return;
} 

int readFile()
{
 char buff[500];
 int num=0;
 memset(buff,0,sizeof(buff));
 
 if(fgets(buff,sizeof(buff),file)!=NULL) 
  { 
    
    char *first=buff; 	
	char *delimit=buff;
	if(*first==' ')
	 {	 
	  fprintf(stderr,"Error in File.\n");
	  exit(EXIT_FAILURE);
	 }	
	 int j=0;
	char *k=(char *)malloc(sizeof(char)*20); 	
	while((*delimit!=' ' || *delimit!='\t') && j<strlen(buff)-1)    
	 {
	  k[j] = *delimit++;
	  j++;
	 }
	k[j]='\0';
	
	for(j=0;j<strlen(buff)-1;j++)	
	 if(k[j]!='\0' && k[j]!=' ')	    
	   num=num*10+(k[j]-'0');	     		   	
  }
 return num;	 
}

int main(int argc, char **argv)
{  
 gettimeofday(&start,0); 
 printf("Emulation Parameters: \n");

 sigset_t set,oldset;

 My402ListInit(&tokenQueue);
 My402ListInit(&servQueue);
 My402ListInit(&arrvQueue);
 
 int index=1;
 int j=0;
 
 if(argc>1)
  for(j=1;j<argc;j++)
   {
    if(strcmp(argv[j],"-t")==0)
	 break;
   }
 
 if(j<argc && strcmp(argv[j],"-t")==0)
 {   
   j++;   
   file=fopen(argv[j],"r");
   if(file!=NULL)
    flag=1;		   	  
 }   
 mu=0.35;lambda=0.5;
 r=1.5;tokens=3;
 B=10;numberOfPackets=20;
  
 if(flag==1) 
 {
  numberOfPackets=readFile();
  printf("\ttsfile: %s\n",argv[j]);
 }
  
 while(argc>1 && index<=argc-1 && flag==0)
 {
   if(index<=argc-1 && strcmp(argv[index],"-lambda")==0)
   {    
     index++;
    if(atof(argv[index])!=0.0)
	 {
       lambda=atof(argv[index++]);
	   if(lambda<0.1)
	    lambda=0.1;
	 }	 
    else     
	   index++;	       
	continue; 	   	
   }
   
   if(index<=argc-1 && strcmp(argv[index],"-B")==0)
   {    
     index++;
    if(atoi(argv[index])!=0)
	  B=atoi(argv[index++]);
    else     
	  index++;	
	continue; 
   }
   
   if( index<=argc-1 && strcmp(argv[index],"-P")==0)
   {   
    index++;
    if(atoi(argv[index])!=0)
	  tokens=atoi(argv[index++]);
    else     
	   index++;  
    continue;
   }
   
   if(index<=argc-1 && strcmp(argv[index],"-r")==0)
   {    
    index++;
    if(atof(argv[index])!=0.0)
	{
          r=atof(argv[index++]);
          if(r<0.1)
	   r=0.1;
        }
    else     
	   index++;	   	
	continue;
   }
   
   if(index<=argc-1 && strcmp(argv[index],"-n")==0)
   {    
    index++;
    if(atof(argv[index])!=0)
	  numberOfPackets=atoi(argv[index++]);
    else     
	   index++;
	continue;
   }
   
   if(index<=argc-1 && strcmp(argv[index],"-mu")==0)
   {
    index++;
    if(atof(argv[index])!=0.0)
	{
      mu=atof(argv[index++]);
	  if(mu<0.1)
	   mu=0.1;
	}	
    else     	 
	   index++;
	continue;    
   }	
 }
 if(flag==0)
 {
  printf("\tlambda = %g\n",lambda);
  printf("\tmu = %g\n",mu);
  printf("\tr = %g\n",r);
  printf("\tB = %d\n",B);
  printf("\tP = %d\n",tokens);
  printf("\tnumber to arrive = %d\n\n",numberOfPackets);
 }
 
 remainingPackets=numberOfPackets;
 
 sigemptyset(&set);
 sigaddset(&set,SIGINT); 
    
 pthread_create(&tokenThread,0,tokenThreadFunc,&r);
 if(flag==0)
 pthread_create(&arrvThread,0,arrvThreadFunc,&lambda);
 else
 pthread_create(&arrvThread,0,arrvThreadFunc,(void *)0);
 pthread_create(&servThread,0,servThreadFunc,&mu);
 
 sigset(SIGINT,handlerInt);
 sigprocmask(SIG_UNBLOCK,&set,&oldset);
 //pthread_join(tokenThread,0);
 pthread_join(arrvThread,0);
 //pthread_join(servThread,0);
 gettimeofday(&end,0);
 double totalEmuTime=(end.tv_sec*1000000 + end.tv_usec)-(start.tv_sec*1000000 + start.tv_usec);
 totalEmuTime=totalEmuTime/1000;

 free(&tokenQueue);
 free(&arrvQueue);
 free(&servQueue);
 
 printf("\nStatistics:\n");
 printf("\n\t average packet inter-arrival time = %.6g\n",(interArrivalTime/numberOfPackets)/1000);
 
 if(numPacS!=0)
 printf("\t average packet service time = %.6g\n",(double)((totalServiceTime/numPacS)/1000.0f));
 else
 printf("\t average packet service time = Not Valid\n");
 
 printf("\t average numberOfPackets in Q1 = %.6g\n",(totalTimeSpentArrv/totalEmuTime)); 
 printf("\t average numberOfPackets in Q2 = %.6g\n",totalTimeSpentServ/totalEmuTime);
 printf("\t average numberOfPackets at S = %.6g\n",totalTimeSpentS/totalEmuTime);
 
 if(numPacQ1!=0)
 printf("\t average time a packet spent in system = %.6g\n",(timeSpentSys/numPacQ1)/1000);
 else
 printf("\t average time a packet spent in system = Not Valid\n");
 
 double stdDev=0;
 if(numPacQ2!=0)
{ 
  stdDev=sqrt((((forStdDev)/numPacQ2) - ((timeSpentSys/numPacQ2)*(timeSpentSys/numPacQ2)))/1000000);
  printf("\t standard deviation of time spent in system = %.6g\n",stdDev);
}
else
  printf("\t standard deviation of time spent in system = Not Valid\n");
 
 printf("\t token drop probability = %.6g\n",((double)droppedTokens/totalTokens));
 printf("\t packet drop probability = %.6g\n",((double)droppedPac/numberOfPackets));
 
 return 0;
}
