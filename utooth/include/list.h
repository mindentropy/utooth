#ifndef LIST_H_

#define LIST_H_

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

#define is_pool_empty(pool)	\
	(pool->next == NULL) ? 1 : 0

void init_pool(void *data_pool,
				uint8_t data_size,
				uint8_t cnt,
				struct list_head *empty_pool/*,
				struct list_head *used_pool*/);

void * remove_from_pool_head(struct list_head *pool);
void add_to_pool_head(struct list_head *pool,void *data);
void mv_entry_to_pool_head(
				struct list_head *topool,
				struct list_head *entry
				);

void mv_entry_after(struct list_head *toentry,
			struct list_head *entry);

void mv_entry_before(struct list_head *toentry,
					struct list_head *entry);
struct list_head * 
	get_entry(struct list_head *pool,
				void *data,
				uint8_t data_size);



#endif

