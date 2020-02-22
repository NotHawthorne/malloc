/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 19:07:24 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/18 19:48:05 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

size_t				count_pages(size_t block_size, int allocations)
{
	size_t	ret;
	size_t	space;
	int		i;

	i = 0;
	ret = 0;
	space = 0;
	ret = (((allocations * block_size) + (allocations *
			sizeof(struct s_block))) / g_malloc->page_size) + 1;
	return (ret);
}

size_t				count_allocs(struct s_block *zone)
{
	struct s_block	*tmp;
	size_t			ret;

	tmp = zone;
	ret = 0;
	while (tmp)
	{
		tmp = tmp->next;
		ret++;
	}
	return (ret);
}

size_t				malloc_calc(struct s_block *block)
{
	return ((count_pages(block->size <= TINY ? TINY : SMALL, MIN_ALLOCATIONS) *
			g_malloc->page_size) / (64 + block->size <= TINY ? TINY : SMALL));
}
