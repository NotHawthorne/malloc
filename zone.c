/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   zone.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 03:37:10 by alkozma           #+#    #+#             */
/*   Updated: 2020/01/27 20:21:58 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

size_t	count_pages(size_t block_size, int allocations)
{
	size_t	ret;
	size_t	space;
	int		i;

	i = 0;
	ret = 0;
	space = 0;
	while (i < allocations)
	{
		ret = (((i * block_size) + (i * sizeof(struct s_block))) / g_malloc->page_size) + 1;
		i++;
	}
	return (ret);
}

struct s_block	*new_zone(size_t pages, size_t allocation_size, size_t allocations)
{
	void			*mem;
	struct s_block	*tmp;
	struct s_block	*head;
	size_t			i;

	mem = mmap(0, pages * g_malloc->page_size, PROT_READ | PROT_WRITE,
				MAP_PRIVATE | MAP_ANONYMOUS, -1, g_malloc->page_size);
	i = 0;
	tmp = mem;
	head = mem;
	allocations = allocations ? allocations : MIN_ALLOCATIONS;
	while (i < allocations)
	{
		tmp->free = 1;
		tmp->size = allocation_size;
		tmp->ptr = mem + ((i * allocation_size) + ((i + 1) * sizeof(struct s_block)));
		tmp->next = 0;
		tmp->prev = head->tail;
		if (head->tail)
			head->tail->next = tmp;
		head->tail = tmp;
		tmp->tail = tmp;
		tmp = tmp->ptr + allocation_size;
		i++;
	}
	return (head);
}

struct s_block	*extend_zone(size_t size, struct s_block *zone)
{
	struct s_block	*new;
	size_t			pages;
	size_t			new_size;

	if (size < TINY)
		new_size = TINY;
	else if (size < SMALL)
		new_size = SMALL;
	else
		new_size = size;
	pages = count_pages(new_size, size < SMALL ? MIN_ALLOCATIONS : 1);
	new = new_zone(pages, new_size, size < SMALL ? MIN_ALLOCATIONS : 1); 
	zone->tail->next = new;
	zone->tail = new->tail;
	return (new);
}

struct s_block	*fetch_block(size_t size)
{
	struct s_block	*search;
	struct s_block	*head;
	struct s_block	*ret;

	if (size < TINY)
		search = g_malloc->tiny;
	else if (size < SMALL)
		search = g_malloc->small;
	else
		search = g_malloc->large;
	head = search;
	while (search && (search->size < size || search->free == 0))
		search = search->next;
	ret = search ? search : extend_zone(size, head);
	ret->free = 0;
	return (ret);
}

void	init_malloc(void)
{
	printf("init\n");
	g_malloc = mmap(0, getpagesize(), PROT_READ | PROT_WRITE,
					MAP_PRIVATE | MAP_ANONYMOUS, -1, getpagesize());
	g_malloc->page_size = getpagesize();
	g_malloc->tiny =
		new_zone(count_pages(TINY, MIN_ALLOCATIONS), TINY, MIN_ALLOCATIONS);
	g_malloc->small =
		new_zone(count_pages(SMALL, MIN_ALLOCATIONS), SMALL, MIN_ALLOCATIONS);
	g_malloc->large = 0;
	g_malloc->allocations = 200;
	g_malloc->allocated = 0;
}
