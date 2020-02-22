/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 03:17:11 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/17 22:42:34 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"
#include <stdlib.h>

void	*malloc(size_t size)
{
	static int		init;
	struct s_block	*ret;

	ret = NULL;
	if (!init)
	{
		init++;
		init_malloc();
	}
	g_malloc->last_op = 1;
	ret = fetch_block(size);
	g_malloc->last_request = ret->ptr;
	return ((g_malloc->last_request = ret->ptr));
}

void	*calloc(size_t count, size_t size)
{
	void	*ret;

	if (count == 0 || size == 0)
		return (NULL);
	ret = malloc(count * size);
	ft_memset(ret, 0, count * size);
	return (ret);
}
