#include "my402list.h"
#include "stdlib.h"
#include "cs402.h"
#include "stdio.h"

int My402ListLength(My402List *list)
{
 return (list->num_members); 
}

int My402ListEmpty(My402List *list)
{
 if(My402ListLength(list)==0)
   return 1;
 return 0;
}

My402ListElem *My402ListFirst(My402List *list)
{
 My402ListElem *anchorElement;
 anchorElement=&(list->anchor);
 if(My402ListEmpty(list))
  return NULL;
  
 return anchorElement->next; 
}

My402ListElem *My402ListLast(My402List *list)
{
 My402ListElem *anchorElement;
 anchorElement=&(list->anchor);
 if(My402ListEmpty(list))
  return NULL;
 
 return anchorElement->prev; 
}

My402ListElem *My402ListNext(My402List *list, My402ListElem *element)
{
  if(My402ListEmpty(list))
   return NULL;
 
 if(element->next==&(list->anchor))  
	return NULL; 
  
  return element->next;
}

My402ListElem *My402ListPrev(My402List *list, My402ListElem *element)
{
  if(My402ListEmpty(list))
   return NULL;
  
  if(element->prev==&(list->anchor))
   return NULL;
  
  return element->prev;
}

int  My402ListAppend(My402List *list, void *object)
{
  My402ListElem *lastElement=My402ListLast(list);
  My402ListElem *ptr=(My402ListElem *)malloc(sizeof(My402ListElem)); 
  if(ptr==NULL)
   fprintf(stderr,"Malloc Error at Line 63");
   
  ptr->obj=object;
  if(My402ListEmpty(list))
  {  
   (list->anchor).next=ptr;
   (list->anchor).prev=ptr;
   ptr->prev=&(list->anchor);
   ptr->next=&(list->anchor);  
   (list->num_members)++;
   return 1;
  }
   
  lastElement->next=ptr;
  ptr->prev=lastElement;
  ptr->next=&(list->anchor);
  (list->anchor).prev=ptr;  
 (list->num_members)++;
  return 1; 
}

int  My402ListPrepend(My402List *list, void *object)
{
  My402ListElem *firstElement=My402ListFirst(list);
  My402ListElem *ptr=(My402ListElem *)malloc(sizeof(My402ListElem));
  if(ptr==NULL)
   fprintf(stderr,"Malloc Error at Line 89");
  
  ptr->obj=object;
  if(My402ListEmpty(list))
  {
   (list->anchor).next=ptr;
   (list->anchor).prev=ptr;
   ptr->prev=&(list->anchor);
   ptr->next=&(list->anchor);
   (list->num_members)++;
   return 1;   
  } 
  
  firstElement->prev=ptr;
  ptr->next=firstElement;
  (list->anchor).next=ptr;
  ptr->prev=&(list->anchor);
  (list->num_members)++;
  return 1; 
}

void My402ListUnlink(My402List *list, My402ListElem *element)
{
 My402ListElem *nxt=element->next;
 My402ListElem *prv=element->prev;
 if(element!=&(list->anchor))
 {
  (nxt->prev)=prv;
  (prv->next)=nxt;
  (list->num_members)--;
  free(element);
  element=NULL;
 }
}

void My402ListUnlinkAll(My402List *list)
{
 My402ListElem *ptr=My402ListFirst(list); 
  while(list->num_members!=0)
  {
    My402ListUnlink(list,ptr);
	ptr = list->anchor.next;
  }
}

int My402ListInsertAfter(My402List *list, void *object, My402ListElem *element)
{
 if(element==My402ListLast(list))
  {
   int success=My402ListAppend(list,object);
   return success;
  } 
  
 My402ListElem *ptr=(My402ListElem *)malloc(sizeof(My402ListElem));
 if(ptr==NULL)
   fprintf(stderr,"Malloc Error at Line 146");
   
 ptr->obj=object;
 ptr->prev=element;
 ptr->next=element->next;
 element->next=ptr;
 ((ptr->next)->prev)=ptr;
 (list->num_members)++;
 return 1;
}

int My402ListInsertBefore(My402List *list, void *object, My402ListElem *element)
{
 if(element==My402ListFirst(list))
  {
	int success=My402ListPrepend(list,object);
	return success;
  }	
  
 My402ListElem *ptr=(My402ListElem *)malloc(sizeof(My402ListElem)); 
 if(ptr==NULL)
   fprintf(stderr,"Malloc Error at Line 167");
  
  
 
 ptr->obj=object;
 ptr->next=element;
 ptr->prev=element->prev;
 element->prev=ptr;
 (ptr->prev)->next=ptr;
 (list->num_members)++; 
 return 1;   
}

My402ListElem *My402ListFind(My402List *list, void *object)
{
 My402ListElem *anc=&(list->anchor);
 My402ListElem *ptr=anc->next;
 
 while( ptr->obj!=object && ptr!=anc )
  ptr=ptr->next;  
 
 if(ptr->obj!=object)
  return NULL;
 
 return ptr;
}

int My402ListInit(My402List *list)
{
 list->anchor.next=&(list->anchor);
 list->anchor.prev=&(list->anchor);
 list->anchor.obj=NULL;
 list->num_members=0;
 return 1;
}
