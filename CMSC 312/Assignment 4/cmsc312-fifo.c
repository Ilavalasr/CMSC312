/*
Suraj ilavala
Assignment 4
cmsc312
*/

/**********************************************************************

   File          : cmsc312-fifo.c

   Description   : This is FIFO page replacement algorithm

   By            : Trent Jaeger

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

/* Include Files */
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

/* fifo list */

typedef struct fifo_entry {
  int pid;
  frame_t *frame;
  struct fifo_entry *next;
} fifo_entry_t;

typedef struct fifo {
  fifo_entry_t *first;
  fifo_entry_t *last;
} fifo_t;

fifo_t *frame_list;

/**********************************************************************

    Function    : init_fifo
    Description : initialize fifo list
    Inputs      : fp - input file of data
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int init_fifo( FILE *fp )
{
  frame_list = (fifo_t *)malloc(sizeof(fifo_t));

  return 0;
}


/**********************************************************************

    Function    : replace_fifo
    Description : choose victim from fifo list (first in list is oldest)
    Inputs      : pid - process id of victim frame
                  victim - frame assigned from fifo -- to be replaced
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/


int replace_fifo( int *pid, frame_t **victim )
{
  fifo_entry_t *first = frame_list->first;

  /* info on victim */
  *victim = first->frame;
  *pid = first->pid;

  /* removes from list */
  frame_list->first = first->next;
  free( first );

  return 0;
}


/**********************************************************************

    Function    : update_fifo
    Description : update fifo list on allocation (add entry to end)
    Inputs      : pid - process id
                  f - frame
    Outputs     : 0 if successful, -1 otherwise

***********************************************************************/

int update_fifo( int pid, frame_t *f )
{
  /* makes a new list entry for new material */
  fifo_entry_t *list_entry = ( fifo_entry_t *)malloc(sizeof(fifo_entry_t));
  list_entry->pid = pid;
  list_entry->frame = f;
  list_entry->next = NULL;

  /* puts it at the end of the list */
  if ( frame_list->first == NULL ) {
    frame_list->last = list_entry;
    frame_list->first = list_entry;
  }
  /* if beginning not null it just adds it regularily */
  else {
    frame_list->last->next = list_entry;
    frame_list->last = list_entry;
  }

  return 0;
}
