/*
 * A utility file. A single linked list.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

#define DIMLIB
#include <dim.h>


void sll_init( head )
SLL* head;
{
	head->next = (SLL *)0;
}


int sll_insert_queue( head, item )
register SLL *head, *item;
{
	SLL *auxp;

	DISABLE_AST
	auxp = head;
	while( auxp->next )
		auxp = auxp->next;
	auxp->next = item;
	item->next = 0;
	ENABLE_AST
	return(1);
}


SLL *sll_search( head, data, size )
register SLL *head; 
char *data;
int size;
{
	DISABLE_AST
	while( (head = head->next) )
	{
		if( !memcmp(head->user_info, data, size) )
		{
			break;
		}
	}
	ENABLE_AST
	return(head);
}


SLL *sll_get_next(item)
register SLL *item;
{
	DISABLE_AST
	if( item )
		item = item->next;
	ENABLE_AST
	return(item);
}


int sll_empty( head )
register SLL *head;
{
	register int ret;
 
	DISABLE_AST
	if(head->next)
		ret = 0;
	else
		ret = 1;
	ENABLE_AST
	return(ret);
}


int sll_remove( head, item )
register SLL *head, *item;
{
	register int ret = 0;

	DISABLE_AST
	while( head->next )
	{
		if( head->next == item )
		{
			head->next = item->next;
			ret = 1;
			break;
		}
		head = head->next;
	}
	ENABLE_AST
	return(ret);
}	


SLL *sll_remove_head( head ) 
register SLL *head;
{
	register SLL *auxp;

	DISABLE_AST
	if( (auxp = head->next) )
	{
		head->next = auxp->next;
	}
	ENABLE_AST
	return(auxp);
}

SLL *sll_get_head( head ) 
register SLL *head;
{
	register SLL *auxp;

	DISABLE_AST
	auxp = head->next;
	ENABLE_AST
	return(auxp);
}


SLL *sll_search_next_remove( item, offset, data, size )
register SLL *item; 
int offset;
char *data;
int size;
{
	register SLL *auxp;
 
	DISABLE_AST
	while( (auxp = item->next) )
	{
		if( !memcmp(&(auxp->user_info[offset]), data, size) )
		{
			item->next = auxp->next;
			break;
		}
		item = auxp;
	}
	ENABLE_AST
	return(auxp);
}

