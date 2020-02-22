/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   realloc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/29 12:13:31 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/17 22:39:10 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

struct s_block	*search_zones(void *ptr)
{
	struct s_block	*ret;

	if (!ptr)
		return (NULL);
	ret = search_zone(g_malloc->tiny, ptr);
	ret = ret ? ret : search_zone(g_malloc->small, ptr);
	ret = ret ? ret : search_zone(g_malloc->large, ptr);
	return (ret);
}

void			*realloc(void *ptr, size_t size)
{
	struct s_block	*cur;
	void			*retptr;
	size_t			new_size;

	retptr = NULL;
	cur = search_zones(ptr);
	if (cur)
	{
		new_size = cur->size;
		if (size > cur->size)
		{
			while (new_size < size)
				new_size *= 1.5;
			retptr = malloc(cur->size <= SMALL ? size : new_size);
			ft_memcpy(retptr, ptr, cur->alloc_size);
			free(ptr);
			return ((g_malloc->last_request = retptr));
		}
		else
		{
			cur->alloc_size = size;
			return ((g_malloc->last_request = cur->ptr));
		}
	}
	return (ptr == NULL ? malloc(size) : NULL);
}
