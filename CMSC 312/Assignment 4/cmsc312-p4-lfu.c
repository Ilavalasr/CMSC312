/*
Suraj ilavala
Assignment 4
cmsc312
*/
/**********************************************************************

   File          : cmsc312-p4-lfu.c

   Description   : This is least frequently used replacement algorithm
                   (see .h for applications)

   By            : Trent Jaeger, Yuquan Shan

***********************************************************************/
/**********************************************************************
Copyright (c) 2016 The Pennsylvania State University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

    * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
    * Neither the name of The Pennsylvania State University nor the names of its contributors may be used to endorse or promote products derived from this softwiare without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>
#include <sched.h>

/* Project Include Files */
#include "cmsc312-p4.h"

/* Definitions */

/* second chance list */

typedef struct lfu_entry{
  int pid;
  ptentry_t *ptentry;
  struct lfu_entry *next;
  struct lfu_entry *prev;
} lfu_entry_t;

typedef struct lfu{
  lfu_entry_t *first;
} lfu_t;

lfu_t *page_list;

/**********************************************************************

    Function    : init_lfu
    Description : initialize lfu list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_lfu( FILE *fp )
{
  page_list = (lfu_t *)malloc(sizeof(lfu_t));
  page_list->first = NULL;
  return 0;
}


/**********************************************************************

    Function    : replace_lfu
    Description : choose victim based on lfu algorithm, take the frame
                  associated the page with the smallest count as victim
    Inputs      : pid - process id of victim frame
                  victim - frame assigned from fifo -- to be replaced
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int replace_lfu( int *pid, frame_t **victim )
{
  /* Task 3 */
  lfu_entry_t *first = page_list->first;
  printf("replace_enh: new starting point: frame: %d\n",first->ptentry->frame);
  /* returns victim info */
  int first_access=1;
  int found_flag=0;
  while((first!=page_list->first) || (first_access)){
    if (((first->ptentry->bits&REFBIT)!=REFBIT) && ((first->ptentry->bits&DIRTYBIT)!=DIRTYBIT)){
      *victim = &physical_mem[first->ptentry->frame];
      *pid = first->pid;
      found_flag=1;
      break;
    }
    first_access=0;
    first=first->next;
  }
  if(!found_flag){
      first_access=1;
      while((first!=page_list->first) || (first_access)) {
        if (((first->ptentry->bits&REFBIT)!=REFBIT) && ((first->ptentry->bits&DIRTYBIT)==DIRTYBIT)){
          *victim = &physical_mem[first->ptentry->frame];
          *pid = first->pid;
          found_flag=1;
          break;
        }
        else{
          first->ptentry->bits-=REFBIT;
        }
        first_access=0;
        first=first->next;
      }
  }
  if(!found_flag){
      first_access=1;
      while((first!=page_list->first) || (first_access)){
        if (((first->ptentry->bits&REFBIT)!=REFBIT) && ((first->ptentry->bits&DIRTYBIT)!=DIRTYBIT)){
          *victim = &physical_mem[first->ptentry->frame];
          *pid = first->pid;
          found_flag=1;
          break;
        }
        first_access=0;
        first=first->next;
      }
  }
  if(!found_flag){
      first_access=1;
      while((first!=page_list->first) || (first_access)){
        if (((first->ptentry->bits&REFBIT)!=REFBIT) && ((first->ptentry->bits&DIRTYBIT)==DIRTYBIT)){
          *victim = &physical_mem[first->ptentry->frame];
          *pid = first->pid;
          found_flag=1;
          break;
        }
        first_access=0;
        first=first->next;
      }
  }

  /* removes item from a specified list */
  first->prev->next=first->next;
  first->next->prev=first->prev;
  page_list->first = first->next;
  free( first );
  return 0;
}


/**********************************************************************

    Function    : update_lfu
    Description : create container for the newly allocated frame (and
                  associated page), and insert it to the end (with respect
                  to page_list->first) of page list
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_lfu( int pid, frame_t *f )
{
  /* Task 3 */
  printf("update_enh: -- current enh_list\n");
  ptentry_t* pid_s_pt=&processes[pid].pagetable[f->page];
  lfu_entry_t *list_entry=( lfu_entry_t *)malloc(sizeof(lfu_entry_t));
  list_entry->ptentry = pid_s_pt;
  list_entry->pid = pid;
  if(page_list->first==NULL){
      list_entry->prev=list_entry;
      list_entry->next=list_entry;
      page_list->first=list_entry;
  }
  else{
      list_entry->prev=page_list->first->prev;
      list_entry->next=page_list->first;
      page_list->first->prev->next=list_entry;
      page_list->first->prev=list_entry;
  }
  return 0;
}
