/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 19:31:08 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/18 19:47:25 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

struct s_block			*search_zone(struct s_block *zone, void *ptr)
{
	struct s_block	*cur;

	if ((cur = zone))
	{
		while (cur)
		{
			if ((cur->ptr == ptr ||
					(ptr < cur->ptr + cur->size && ptr > cur->ptr))
					&& !cur->free)
				return (cur);
			cur = cur->next;
		}
		return (NULL);
	}
	return (NULL);
}

static int				is_page_free(struct s_block *block)
{
	size_t			sz;
	struct s_block	*tmp;
	int				b;

	sz = 0;
	tmp = block;
	b = block->size <= SMALL ? 1 : 0;
	if ((unsigned)block->next - (unsigned)block > (unsigned)getpagesize())
		return (2);
	while (sz < (size_t)g_malloc->page_size)
	{
		if (!tmp || !tmp->next)
			return (2);
		if (!tmp->free)
			return (0);
		tmp += block->size / sizeof(struct s_block) + 1;
		sz += sizeof(struct s_block) + block->size;
	}
	return (block->next ? 1 : 2);
}

static int				free_node(struct s_block *block, void *ptr)
{
	struct s_block	*tmp;
	struct s_block	*tmp2;
	int				stat;
	int				min;
	size_t			sz;

	tmp = 0;
	if (!ptr || !block)
		return (-1);
	block->free = 1;
	block->alloc_size = 0;
	min = malloc_calc(block);
	sz = block->size;
	if (block->alloc_num > min && block->size <= SMALL)
	{
		tmp = (struct s_block*)(ptr - ((unsigned)ptr % getpagesize()));
		stat = is_page_free(tmp);
		tmp2 = ((void*)tmp) + getpagesize();
		if (stat == 1 && tmp && tmp->prev)
			tmp->prev->next = tmp2;
		if (stat == 1 && tmp && tmp2)
			tmp2->prev = tmp->prev;
		stat == 1 ? munmap(tmp, getpagesize()) : 0;
	}
	return (1);
}

void					free_help(struct s_block *cur)
{
	if (cur->next)
		cur->next->prev = cur->prev;
	if (cur->prev)
		cur->prev->next = cur->next;
	else
		g_malloc->large = cur->next;
}

void					free(void *ptr)
{
	struct s_block	*cur;
	int				z;

	if (!ptr)
		return ;
	cur = search_zones(ptr);
	z = -1;
	if (cur)
		z = cur->size <= SMALL ? 1 : 2;
	if (cur)
	{
		z <= 1 ? 0 : free_help(cur);
		z <= 1 ? free_node(cur, ptr) : 0;
		z <= 1 ? 0 : munmap(cur, cur->size + sizeof(struct s_block));
	}
}
