/* FvwmTaskBar Module for Fvwm.
 *
 *  Copyright 1994,  Mike Finger (mfinger@mermaid.micro.umn.edu or
 *                               Mike_Finger@atk.com)
 *
 * The functions in this source file are the original work of Mike Finger.
 *
 * No guarantees or warantees or anything are provided or implied in any way
 * whatsoever. Use this program at your own risk. Permission to use this
 * program for any purpose is given, as long as the copyright is kept intact.
 *
 *  Things to do:  Convert to C++  (In Progress)
 */

/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "config.h"
#include "libs/Module.h"
#include "libs/safemalloc.h"

#include <stdio.h>
#include <stdlib.h>

#include "FvwmTaskBar.h"
#include "List.h"
#include "Mallocs.h"


/******************************************************************************
  InitList - Initialize the list
******************************************************************************/
void InitList(List *list)
{
  list->head=list->tail=NULL;
  list->count=0;
}

/******************************************************************************
  AddItem - Allocates spaces for and appends an item to the list
******************************************************************************/
void AddItem(List *list, long id, long flags, ConfigWinPacket *cfgpacket,
             long Desk, int count)
{
  Item *new;
  new=(Item *)safemalloc(sizeof(Item));
  new->id=id;
  new->name=NULL;
  new->tb_flags=flags;

  /* If this was M_ADD_WINDOW or M_CONFIGURE_WINDOW, we have real flags  */
  if (cfgpacket != NULL) {
    memcpy(&new->flags, &cfgpacket->flags, sizeof(new->flags));
  }

  new->Desk=Desk;
  new->count=count;
  memset((void *)&(new->p), 0, sizeof(Picture));
  new->next=NULL;
  if (list->tail==NULL) list->head=list->tail=new;
  else {
   list->tail->next=new;
   list->tail=new;
  }
  list->count++;
}

/******************************************************************************
  AddItemName - Allocates spaces for and appends an item to the list
******************************************************************************/
void AddItemName(List *list, char *string, long flags)
{
  Item *new;

  new = (Item *)safemalloc(sizeof(Item));
  new->id = 0L;
  new->name = NULL;
  UpdateString(&new->name, string);
  new->tb_flags = flags;
  new->next = NULL;

  if (list->tail == NULL)
    list->head = list->tail = new;
  else
  {
    list->tail->next = new;
    list->tail = new;
  }
  list->count++;
}


/******************************************************************************
  FindItem - Find the item in the list matching the id
******************************************************************************/
int FindItem(List *list, long id)
{
  Item *temp;

  for(temp=list->head;temp!=NULL && temp->id!=id;temp=temp->next)
    ;
  if (temp==NULL)
    return -1;
  return temp->count;
}

/******************************************************************************
  FindNameItem - Find the item in the list matching the string
******************************************************************************/
int FindNameItem(List *list, char *string)
{
  Item *temp;

  for(temp=list->head;
      temp!=NULL && strcmp(temp->name,string) != 0;
      temp=temp->next);
  if (temp==NULL) return -1;
  return temp->count;
}

/******************************************************************************
  UpdateItem - Update the item in the list, setting name & flags as necessary.
******************************************************************************/
int UpdateItemName(List *list, long id, char *string)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && id!=temp->id;temp=temp->next);
  if (temp==NULL) return -1;
  UpdateString(&temp->name,string);
  return temp->count;
}

int UpdateItemFlags(List *list, long id, long flags)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && id!=temp->id;temp=temp->next);
  if (temp==NULL) return -1;
  if (flags!=-1) temp->tb_flags=flags;
  return temp->count;
}

int UpdateItemFlagsDesk(List *list, long id, long flags, long desk)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && id!=temp->id;temp=temp->next);
  if (temp==NULL) return -1;
  if (flags!=-1) temp->tb_flags=flags;
  temp->Desk=desk;
  return temp->count;
}

int UpdateNameItem(List *list, char *string, long id, long flags) {
  Item *temp;

  for(temp=list->head;
      temp!=NULL && strcmp(temp->name,string) != 0;
      temp=temp->next);
  if (temp==NULL) return -1;
  else {
    if (id != -1) temp->id = id;
    if (flags != -1) temp->tb_flags = flags;
    return temp->count;
  }
}

/******************************************************************************
  FreeItem - Frees allocated space for an Item
******************************************************************************/
void FreeItem(Item *ptr)
{
  if (ptr != NULL) {
    if (ptr->name!=NULL) free(ptr->name);
    free(ptr);
  }
}

/******************************************************************************
  DeleteItem - Deletes an item from the list
******************************************************************************/
int DeleteItem(List *list,long id)
{
Item *temp,*temp2;
int i;
  if (list->head==NULL) return -1;
  if (list->head->id==id) {
    temp2=list->head;
    temp=list->head=list->head->next;
    i=temp2->count;
  } else {
     for(temp=list->head,temp2=temp->next; temp2; temp2=temp2->next) {
      if (temp2->id == id)
        break;
      temp = temp2;
    }
    if (temp2 == NULL) return -1;
    temp->next=temp2->next;
  }

  if (temp2==list->tail) list->tail=temp;

  i=temp2->count;
  FreeItem(temp2);
  list->count--;
  return i;
/*    return i+1; */
}

/******************************************************************************
  FreeList - Free the entire list of Items
******************************************************************************/
void FreeList(List *list)
{
  Item *temp,*temp2;
  for(temp=list->head;temp!=NULL;)
  {
    temp2=temp;
    temp=temp->next;
    FreeItem(temp2);
  }
  list->count=0;
}

/******************************************************************************
  PrintList - Print the list of item on the console. (Debugging)
******************************************************************************/
void PrintList(List *list)
{
  Item *temp;
  ConsoleMessage("List of Items:\n");
  ConsoleMessage("   %10s %-15s %-15s %-15s %-15s Flgs\n","ID","Name","I-Name",
		 "R-Name","R-Class");
  ConsoleMessage("   ---------- --------------- --------------- --------------- --------------- ----\n");
  for(temp=list->head;temp!=NULL;temp=temp->next) {
    ConsoleMessage("   %10ld %-15.15s %4d\n",temp->id,
		   (temp->name==NULL) ? "<null>" : temp->name,
		   temp->tb_flags);
  }
}

/******************************************************************************
  ItemName - Return the name of an Item
******************************************************************************/
char *ItemName(List *list, int n)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && temp->count!=n;temp=temp->next);
  if (temp==NULL) return NULL;
  return temp->name;
}

/******************************************************************************
  ItemFlags - Return the flags for an item
******************************************************************************/
long ItemFlags(List *list, long id)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && id!=temp->id;temp=temp->next);
  if (temp==NULL) return -1;
  else return temp->tb_flags;
}

/******************************************************************************
  ItemIndexFlags - Return the flags for an item of index i
******************************************************************************/
long ItemIndexFlags(List *list, int i)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && temp->count!=i;temp=temp->next);
  if (temp==NULL) return -1;
  else return temp->tb_flags;
}

/* RBW- never used... */
/******************************************************************************
  XorFlags - Exclusive of the flags with the specified value.
******************************************************************************/
/*
long XorFlags(List *list, int n, long value)
{
Item *temp;
long ret;
  for(temp=list->head;temp!=NULL && i<n;temp=temp->next)
  if (temp==NULL) return -1;
  ret=temp->flags;
  temp->flags^=value;
  return ret;
}
*/

/******************************************************************************
  ItemCount - Return the number of items inthe list
******************************************************************************/
int ItemCount(List *list)
{
  return list->count;
}

/******************************************************************************
  ItemID - Return the ID of the item in the list.
******************************************************************************/
long ItemID(List *list, int n)
{
  Item *temp;
  for(temp=list->head;temp!=NULL && temp->count!=n;temp=temp->next);
  if (temp==NULL) return -1;
  return temp->id;
}

/******************************************************************************
  CopyItem - Copy an item from one list to another
******************************************************************************/
void CopyItem(List *dest, List *source, int n)
{
  Item *temp;
  ConfigWinPacket cfgpkt;

  for(temp=source->head;temp!=NULL && temp->count!=n;temp=temp->next);
  if (temp==NULL) return;
  memcpy(&cfgpkt.flags, &temp->flags, sizeof(cfgpkt.flags));
  AddItem(dest, temp->id, temp->tb_flags, &cfgpkt,temp->Desk,temp->count);
  UpdateItemName(dest,temp->id,temp->name);
  DeleteItem(source,temp->id);
}

/******************************************************************************
  UpdateItemPicture - Adds the picture information in the list
******************************************************************************/
void UpdateItemPicture(List *list, int n, Picture *p)
{
  Item *temp;

  for (temp=list->head;temp && temp->count!=n;temp=temp->next);
  if (temp==NULL) return;
  if (p != NULL)
  {
    temp->p.picture = p->picture;
    temp->p.mask    = p->mask;
    temp->p.width   = p->width;
    temp->p.height  = p->height;
    temp->p.depth   = p->depth;
  }
}

/******************************************************************************
  GetDeskNumber - Returns the desknumber of the item
******************************************************************************/
int GetDeskNumber(List *list, int n, long *Desk)
{
  Item *temp;

  for (temp=list->head;temp && temp->count!=n;temp=temp->next);
  if (temp==NULL) return 0;
  *Desk=temp->Desk;
  return 1;
}

/******************************************************************************
  GetItemPicture - Returns the picture
******************************************************************************/
Picture *GetItemPicture(List *list, int n)
{
  Item *temp;

  for (temp=list->head;temp && temp->count!=n;temp=temp->next);
  if (temp==NULL) return 0;
  return &(temp->p);
}
