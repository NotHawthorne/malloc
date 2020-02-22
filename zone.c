/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 03:37:10 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/17 22:20:58 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

struct s_block		*block_helper(struct s_block *block, struct s_block *head,
										size_t alloc_num, size_t size)
{
	struct s_block	*tmp;

	tmp = block;
	if (head->tail)
		head->tail->next = tmp;
	tmp->alloc_num = alloc_num;
	tmp->size = size;
	tmp->prev = head->tail;
	tmp->alloc_size = 0;
	tmp->next = 0;
	head->tail = tmp;
	tmp->tail = NULL;
	tmp->free = 1;
	tmp->ptr = tmp + 1;
	return (tmp);
}

struct s_block		*new_zone(size_t pages, size_t allocation_size,
							size_t allocations, size_t cur_allocs)
{
	struct s_block	*tmp;
	struct s_block	*head;
	size_t			i;
	struct s_block	*prev;

	head = mmap(0, pages * g_malloc->page_size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	i = -1;
	tmp = head;
	head->tail = 0;
	prev = 0;
	if (allocations != 1)
		allocations = (pages * g_malloc->page_size) / (64 + allocation_size);
	else
		allocation_size = (pages * g_malloc->page_size) - 64;
	while (++i < allocations)
	{
		tmp = block_helper(tmp, head, cur_allocs + i, allocation_size);
		tmp->prev = prev;
		if (prev)
			prev->next = tmp;
		prev = tmp;
		tmp += allocation_size / sizeof(struct s_block) + 1;
	}
	return (head);
}

struct s_block		*extend_zone(size_t size, struct s_block *zone)
{
	struct s_block	*new;
	size_t			pages;
	size_t			new_size;
	struct s_block	*tmp;

	tmp = NULL;
	new_size = size;
	new_size = size <= TINY ? TINY : new_size;
	new_size = size <= SMALL ? SMALL : new_size;
	pages = count_pages(new_size, size <= SMALL ? MIN_ALLOCATIONS : 1);
	new = new_zone(pages, new_size, size <= SMALL ? MIN_ALLOCATIONS : 1,
					zone ? count_allocs(zone) + 1 : 0);
	if (!zone && size > SMALL)
		g_malloc->large = new;
	else
	{
		tmp = zone;
		while (tmp && tmp->next)
			tmp = tmp->next;
		new->prev = tmp;
		tmp->next = new;
	}
	return (new);
}

struct s_block		*fetch_block(size_t size)
{
	struct s_block	*search;
	struct s_block	*head;
	struct s_block	*ret;

	search = NULL;
	if (size <= TINY)
		search = g_malloc->tiny;
	else if (size <= SMALL)
		search = g_malloc->small;
	else
		search = g_malloc->large;
	head = search;
	while (search && (search->size < size || !search->free))
		search = search->next;
	ret = search ? search : extend_zone(size, head);
	ret->alloc_size = size;
	ret->free = 0;
	return (ret);
}

void				init_malloc(void)
{
	g_malloc = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	g_malloc->page_size = getpagesize();
	g_malloc->tiny = new_zone(
			count_pages(TINY, MIN_ALLOCATIONS), TINY, MIN_ALLOCATIONS, 0);
	g_malloc->small = new_zone(
			count_pages(SMALL, MIN_ALLOCATIONS), SMALL, MIN_ALLOCATIONS, 0);
	g_malloc->large = 0;
	g_malloc->allocations = 200;
	g_malloc->allocated = 0;
	g_malloc->last_request = 0;
	g_malloc->last_op = -1;
}
