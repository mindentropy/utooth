#include <stdio.h>
#include <stdint.h>

#include "list.h"

struct list_head empty_pool;
struct list_head used_pool;

void init_pool(void *data_pool,
				uint8_t data_size,
				uint8_t cnt,
				struct list_head *empty_pool/*,
				struct list_head *used_pool*/) {
	int i = 0;
	uint8_t *ch;

	empty_pool->next = ((struct list_head *)data_pool);
	empty_pool->prev = NULL;

/*	used_pool->next = NULL;
	used_pool->prev = NULL;*/
	
	ch = (uint8_t *)data_pool;
	cnt = cnt - 1;

	((struct list_head *)ch)->prev = NULL; // Make initial node prev to null.
	for(i = 0; i<cnt;i++,ch+=data_size) {
		((struct list_head *)ch)->next = (struct list_head *)(ch+data_size);
		((struct list_head *)(ch+data_size))->prev = (struct list_head *)ch;
	}
	
	((struct list_head *)ch)->next = NULL; // Make final node next to null.

}

void * remove_from_pool_head(struct list_head *pool) {
	void *data;


	if(is_pool_empty(pool))
		return NULL;
	
	data = pool->next; //Pool head.
	if(pool->next->next == NULL) { //Singleton
		pool->next = NULL;
	} else { //Multiple data.
		(pool->next)->next->prev = NULL;
		pool->next = (pool->next)->next;
	}

	return data;
}


void add_to_pool_head(struct list_head *pool,void *data) {
	((struct list_head *)data)->next = pool->next;
	((struct list_head *)data)->prev = NULL; //Make the initial node prev to NULL

	if(!is_pool_empty(pool))
		pool->next->prev = data;

	pool->next = ((struct list_head *)data);
}


void mv_entry_to_pool_head(
				struct list_head *topool,
				struct list_head *entry
				) {

	if(entry->prev != NULL)
		entry->prev->next = entry->next;

	if(entry->next != NULL)
		entry->next->prev = entry->prev;

	add_to_pool_head(topool,entry);

	return;
}

struct list_head * get_entry(struct list_head *pool,
					void *data,
					uint8_t data_size) {

	struct list_head *node = pool->next;

	if(is_pool_empty(pool))
		return NULL;

	while(node != NULL) {
		if(node == data)
			return node;

		node = node->next;
	}

	return NULL;
}

void mv_entry_after(struct list_head *toentry,
			struct list_head *entry) {
	
	if(entry->prev != NULL)
		entry->prev->next = entry->next;

	if(entry->next != NULL)
		entry->next->prev = entry->prev;
	
	entry->next = toentry->next;
	entry->prev = toentry;
	
	if(toentry->next != NULL)
		toentry->next->prev = entry;

	toentry->next = entry;
}

void mv_entry_before(struct list_head *toentry,
					struct list_head *entry) {
	
	if(entry->prev != NULL)
		entry->prev->next = entry->next;

	if(entry->next != NULL)
		entry->next->prev = entry->prev;

	
	entry->next = toentry;
	entry->prev = toentry->prev;
	
	if(toentry->prev != NULL)
		toentry->prev->next = entry;

	toentry->prev = entry;
	
}


/*
struct list_head * dbg_fwd(struct list_head *pool) {
	struct list_head *node = pool->next;
	struct data *data;
	
	if(is_pool_empty(pool))
		return NULL;

	while(1) {
		data = (struct data *)(node);
		printf("%d ",data->x);

		if(node->next == NULL) {
			return node;
		}

		node = node->next;
	}

	return NULL;
}

void dbg_bwd(struct list_head *node) {
	struct data *data;

	if(node == NULL) 
		return;
	
	while(1) {
		data = (struct data *)(node);
		
		printf("%d ",data->x);

		if(node->prev == NULL)
			break;
		
		node = node->prev;
	}
}
*/
