/*******************************************************************************
* Copyright (C), 2000-2016,  Electronic Technology Co., Ltd.
*                
* @filename: es_list.h 
*                
* @author: Clarence.Chow <zhou_chenz@163.com> 
*                
* @version:
*                
* @date: 2016-10-17    
*                
* @brief:          
*                  
*                  
* @details:        
*                 
*    
*    
* @comment           
*******************************************************************************/
#ifndef _ES_LIST_H_
#define _ES_LIST_H_
#include <es_common.h>
/*
 * Simple doubly linked es_list implementation.
 *
 * Some of the internal functions ("__xxx") are useful when
 * manipulating whole es_lists rather than single entries, as
 * sometimes we already know the next/prev entries and we can
 * generate better code by using them directly rather than
 * using the generic single-entry routines.
 */

struct es_list_head {
	struct es_list_head *next, *prev;
};

#define ES_LIST_HEAD_INIT(name) { &(name), &(name) }

#define ES_LIST_HEAD(name) \
	struct es_list_head name = ES_LIST_HEAD_INIT(name)

static inline void INIT_ES_LIST_HEAD(struct es_list_head *es_list)
{
	es_list->next = es_list;
	es_list->prev = es_list;
}

/*
 * Insert a new entry between two known consecutive entries.
 *
 * This is only for internal es_list manipulation where we know
 * the prev/next entries already!
 */
static inline void __es_list_add(struct es_list_head *new,
			      struct es_list_head *prev,
			      struct es_list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}


/**
 * es_list_add - add a new entry
 * @new: new entry to be added
 * @head: es_list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
static inline void es_list_add(struct es_list_head *new, struct es_list_head *head)
{
	__es_list_add(new, head, head->next);
}


/**
 * es_list_add_tail - add a new entry
 * @new: new entry to be added
 * @head: es_list head to add it before
 *
 * Insert a new entry before the specified head.
 * This is useful for implementing queues.
 */
static inline void es_list_add_tail(struct es_list_head *new, struct es_list_head *head)
{
	__es_list_add(new, head->prev, head);
}

/*
 * Delete a es_list entry by making the prev/next entries
 * point to each other.
 *
 * This is only for internal es_list manipulation where we know
 * the prev/next entries already!
 */
static inline void __es_list_del(struct es_list_head * prev, struct es_list_head * next)
{
	next->prev = prev;
	prev->next = next;
}

/**
 * es_list_del - deletes entry from es_list.
 * @entry: the element to delete from the es_list.
 * Note: es_list_empty() on entry does not return true after this, the entry is
 * in an undefined state.
 */
static inline void es_list_del(struct es_list_head *entry)
{
	__es_list_del(entry->prev, entry->next);
	entry->next = NULL;
	entry->prev = NULL;
}


/**
 * es_list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void es_list_replace(struct es_list_head *old,
				struct es_list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void es_list_replace_init(struct es_list_head *old,
					struct es_list_head *new)
{
	es_list_replace(old, new);
	INIT_ES_LIST_HEAD(old);
}

/**
 * es_list_del_init - deletes entry from es_list and reinitialize it.
 * @entry: the element to delete from the es_list.
 */
static inline void es_list_del_init(struct es_list_head *entry)
{
	__es_list_del(entry->prev, entry->next);
	INIT_ES_LIST_HEAD(entry);
}

/**
 * es_list_move - delete from one es_list and add as another's head
 * @es_list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void es_list_move(struct es_list_head *es_list, struct es_list_head *head)
{
	__es_list_del(es_list->prev, es_list->next);
	es_list_add(es_list, head);
}

/**
 * es_list_move_tail - delete from one es_list and add as another's tail
 * @es_list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void es_list_move_tail(struct es_list_head *es_list,
				  struct es_list_head *head)
{
	__es_list_del(es_list->prev, es_list->next);
	es_list_add_tail(es_list, head);
}

/**
 * es_list_is_last - tests whether @es_list is the last entry in es_list @head
 * @es_list: the entry to test
 * @head: the head of the es_list
 */
static inline int es_list_is_last(const struct es_list_head *es_list,
				const struct es_list_head *head)
{
	return es_list->next == head;
}

/**
 * es_list_empty - tests whether a es_list is empty
 * @head: the es_list to test.
 */
static inline int es_list_empty(const struct es_list_head *head)
{
	return head->next == head;
}

/**
 * es_list_empty_careful - tests whether a es_list is empty and not being modified
 * @head: the es_list to test
 *
 * Description:
 * tests whether a es_list is empty _and_ checks that no other CPU might be
 * in the process of modifying either member (next or prev)
 *
 * NOTE: using es_list_empty_careful() without synchronization
 * can only be safe if the only activity that can happen
 * to the es_list entry is es_list_del_init(). Eg. it cannot be used
 * if another CPU could re-es_list_add() it.
 */
static inline int es_list_empty_careful(const struct es_list_head *head)
{
	struct es_list_head *next = head->next;
	return (next == head) && (next == head->prev);
}

/**
 * es_list_rotate_left - rotate the es_list to the left
 * @head: the head of the es_list
 */
static inline void es_list_rotate_left(struct es_list_head *head)
{
	struct es_list_head *first;

	if (!es_list_empty(head)) {
		first = head->next;
		es_list_move_tail(first, head);
	}
}

/**
 * es_list_is_singular - tests whether a es_list has just one entry.
 * @head: the es_list to test.
 */
static inline int es_list_is_singular(const struct es_list_head *head)
{
	return !es_list_empty(head) && (head->next == head->prev);
}

static inline void __es_list_cut_position(struct es_list_head *es_list,
		struct es_list_head *head, struct es_list_head *entry)
{
	struct es_list_head *new_first = entry->next;
	es_list->next = head->next;
	es_list->next->prev = es_list;
	es_list->prev = entry;
	entry->next = es_list;
	head->next = new_first;
	new_first->prev = head;
}

/**
 * es_list_cut_position - cut a es_list into two
 * @es_list: a new es_list to add all removed entries
 * @head: a es_list with entries
 * @entry: an entry within head, could be the head itself
 *	and if so we won't cut the es_list
 *
 * This helper moves the initial part of @head, up to and
 * including @entry, from @head to @es_list. You should
 * pass on @entry an element you know is on @head. @es_list
 * should be an empty es_list or a es_list you do not care about
 * losing its data.
 *
 */
static inline void es_list_cut_position(struct es_list_head *es_list,
		struct es_list_head *head, struct es_list_head *entry)
{
	if (es_list_empty(head))
		return;
	if (es_list_is_singular(head) &&
		(head->next != entry && head != entry))
		return;
	if (entry == head)
		INIT_ES_LIST_HEAD(es_list);
	else
		__es_list_cut_position(es_list, head, entry);
}

static inline void __es_list_splice(const struct es_list_head *es_list,
				 struct es_list_head *prev,
				 struct es_list_head *next)
{
	struct es_list_head *first = es_list->next;
	struct es_list_head *last = es_list->prev;

	first->prev = prev;
	prev->next = first;

	last->next = next;
	next->prev = last;
}

/**
 * es_list_splice - join two es_lists, this is designed for stacks
 * @es_list: the new es_list to add.
 * @head: the place to add it in the first es_list.
 */
static inline void es_list_splice(const struct es_list_head *es_list,
				struct es_list_head *head)
{
	if (!es_list_empty(es_list))
		__es_list_splice(es_list, head, head->next);
}

/**
 * es_list_splice_tail - join two es_lists, each es_list being a queue
 * @es_list: the new es_list to add.
 * @head: the place to add it in the first es_list.
 */
static inline void es_list_splice_tail(struct es_list_head *es_list,
				struct es_list_head *head)
{
	if (!es_list_empty(es_list))
		__es_list_splice(es_list, head->prev, head);
}

/**
 * es_list_splice_init - join two es_lists and reinitialise the emptied es_list.
 * @es_list: the new es_list to add.
 * @head: the place to add it in the first es_list.
 *
 * The es_list at @es_list is reinitialised
 */
static inline void es_list_splice_init(struct es_list_head *es_list,
				    struct es_list_head *head)
{
	if (!es_list_empty(es_list)) {
		__es_list_splice(es_list, head, head->next);
		INIT_ES_LIST_HEAD(es_list);
	}
}

/**
 * es_list_splice_tail_init - join two es_lists and reinitialise the emptied es_list
 * @es_list: the new es_list to add.
 * @head: the place to add it in the first es_list.
 *
 * Each of the es_lists is a queue.
 * The es_list at @es_list is reinitialised
 */
static inline void es_list_splice_tail_init(struct es_list_head *es_list,
					 struct es_list_head *head)
{
	if (!es_list_empty(es_list)) {
		__es_list_splice(es_list, head->prev, head);
		INIT_ES_LIST_HEAD(es_list);
	}
}


/**
 * es_list_entry - get the struct for this entry
 * @ptr:	the &struct es_list_head pointer.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the es_list_struct within the struct.
 */
#define es_list_entry(ptr, type, member) \
	container_of(ptr, type, member)

/**
 * es_list_first_entry - get the first element from a es_list
 * @ptr:	the es_list head to take the element from.
 * @type:	the type of the struct this is embedded in.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Note, that es_list is expected to be not empty.
 */
#define es_list_first_entry(ptr, type, member) \
	es_list_entry((ptr)->next, type, member)

/**
 * es_list_for_each	-	iterate over a es_list
 * @pos:	the &struct es_list_head to use as a loop cursor.
 * @head:	the head for your es_list.
 */
#define es_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); \
        	pos = pos->next)

/**
 * __es_list_for_each	-	iterate over a es_list
 * @pos:	the &struct es_list_head to use as a loop cursor.
 * @head:	the head for your es_list.
 *
 * This variant differs from es_list_for_each() in that it's the
 * simplest possible es_list iteration code, no prefetching is done.
 * Use this for code that knows the es_list to be very short (empty
 * or 1 entry) most of the time.
 */
#define __es_list_for_each(pos, head) \
	for (pos = (head)->next; pos != (head); pos = pos->next)

/**
 * es_list_for_each_prev	-	iterate over a es_list backwards
 * @pos:	the &struct es_list_head to use as a loop cursor.
 * @head:	the head for your es_list.
 */
#define es_list_for_each_prev(pos, head) \
	for (pos = (head)->prev;  pos != (head); \
        	pos = pos->prev)

/**
 * es_list_for_each_safe - iterate over a es_list safe against removal of es_list entry
 * @pos:	the &struct es_list_head to use as a loop cursor.
 * @n:		another &struct es_list_head to use as temporary storage
 * @head:	the head for your es_list.
 */
#define es_list_for_each_safe(pos, n, head) \
	for (pos = (head)->next, n = pos->next; pos != (head); \
		pos = n, n = pos->next)

/**
 * es_list_for_each_prev_safe - iterate over a es_list backwards safe against removal of es_list entry
 * @pos:	the &struct es_list_head to use as a loop cursor.
 * @n:		another &struct es_list_head to use as temporary storage
 * @head:	the head for your es_list.
 */
#define es_list_for_each_prev_safe(pos, n, head) \
	for (pos = (head)->prev, n = pos->prev; \
	     pos != (head); \
	     pos = n, n = pos->prev)

/**
 * es_list_for_each_entry	-	iterate over es_list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 */
#define es_list_for_each_entry(pos, head, member)				\
	for (pos = es_list_entry((head)->next, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = es_list_entry(pos->member.next, typeof(*pos), member))

/**
 * es_list_for_each_entry_reverse - iterate backwards over es_list of given type.
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 */
#define es_list_for_each_entry_reverse(pos, head, member)			\
	for (pos = es_list_entry((head)->prev, typeof(*pos), member);	\
	     &pos->member != (head); 	\
	     pos = es_list_entry(pos->member.prev, typeof(*pos), member))

/**
 * es_list_prepare_entry - prepare a pos entry for use in es_list_for_each_entry_continue()
 * @pos:	the type * to use as a start point
 * @head:	the head of the es_list
 * @member:	the name of the es_list_struct within the struct.
 *
 * Prepares a pos entry for use as a start point in es_list_for_each_entry_continue().
 */
#define es_list_prepare_entry(pos, head, member) \
	((pos) ? : es_list_entry(head, typeof(*pos), member))

/**
 * es_list_for_each_entry_continue - continue iteration over es_list of given type
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Continue to iterate over es_list of given type, continuing after
 * the current position.
 */
#define es_list_for_each_entry_continue(pos, head, member) 		\
	for (pos = es_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = es_list_entry(pos->member.next, typeof(*pos), member))

/**
 * es_list_for_each_entry_continue_reverse - iterate backwards from the given point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Start to iterate over es_list of given type backwards, continuing after
 * the current position.
 */
#define es_list_for_each_entry_continue_reverse(pos, head, member)		\
	for (pos = es_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head);	\
	     pos = es_list_entry(pos->member.prev, typeof(*pos), member))

/**
 * es_list_for_each_entry_from - iterate over es_list of given type from the current point
 * @pos:	the type * to use as a loop cursor.
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Iterate over es_list of given type, continuing from current position.
 */
#define es_list_for_each_entry_from(pos, head, member) 			\
	for (; &pos->member != (head);	\
	     pos = es_list_entry(pos->member.next, typeof(*pos), member))

/**
 * es_list_for_each_entry_safe - iterate over es_list of given type safe against removal of es_list entry
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 */
#define es_list_for_each_entry_safe(pos, n, head, member)			\
	for (pos = es_list_entry((head)->next, typeof(*pos), member),	\
		n = es_list_entry(pos->member.next, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = es_list_entry(n->member.next, typeof(*n), member))

/**
 * es_list_for_each_entry_safe_continue - continue es_list iteration safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Iterate over es_list of given type, continuing after current point,
 * safe against removal of es_list entry.
 */
#define es_list_for_each_entry_safe_continue(pos, n, head, member) 		\
	for (pos = es_list_entry(pos->member.next, typeof(*pos), member), 		\
		n = es_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = es_list_entry(n->member.next, typeof(*n), member))

/**
 * es_list_for_each_entry_safe_from - iterate over es_list from current point safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Iterate over es_list of given type from current point, safe against
 * removal of es_list entry.
 */
#define es_list_for_each_entry_safe_from(pos, n, head, member) 			\
	for (n = es_list_entry(pos->member.next, typeof(*pos), member);		\
	     &pos->member != (head);						\
	     pos = n, n = es_list_entry(n->member.next, typeof(*n), member))

/**
 * es_list_for_each_entry_safe_reverse - iterate backwards over es_list safe against removal
 * @pos:	the type * to use as a loop cursor.
 * @n:		another type * to use as temporary storage
 * @head:	the head for your es_list.
 * @member:	the name of the es_list_struct within the struct.
 *
 * Iterate backwards over es_list of given type, safe against removal
 * of es_list entry.
 */
#define es_list_for_each_entry_safe_reverse(pos, n, head, member)		\
	for (pos = es_list_entry((head)->prev, typeof(*pos), member),	\
		n = es_list_entry(pos->member.prev, typeof(*pos), member);	\
	     &pos->member != (head); 					\
	     pos = n, n = es_list_entry(n->member.prev, typeof(*n), member))
	     
#endif /* ifndef _ES_LIST_H_.2016-10-17 22:12:50 zcz */

