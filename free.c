/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 19:31:08 by alkozma           #+#    #+#             */
/*   Updated: 2020/01/27 20:12:22 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void	free(void *ptr)
{
	int				ret;
	struct s_block	*cur;
	unsigned		os;

	if ((cur = g_malloc->tiny))
	{
		while (cur)
		{
			if (cur->ptr == ptr)
			{
				if (cur->prev)
					cur->prev->next = cur->next;
				printf("%ld\n", (unsigned long)ptr % g_malloc->page_size);
				os = (unsigned)ptr % g_malloc->page_size;
				ptr -= os;
				cur->free = 1;
				ret = munmap(ptr, cur->size);
				break ;
			}
			cur = cur->next;
		}
	}
	if ((cur = g_malloc->small))
	{
		while (cur)
		{
			if (cur->ptr == ptr)
			{
				cur->free = 1;
				ret = munmap(cur->ptr, cur->size);
				break ;
			}
			cur = cur->next;
		}
	}
	if ((cur = g_malloc->large))
	{
		while (cur)
		{
			if (cur->ptr == ptr)
			{
				cur->free = 1;
				ret = munmap(cur->ptr, cur->size);
				break ;
			}
		}
	}
	printf("free exited with %d\n", ret);
	if (errno)
		printf("[%s]\n", strerror(errno));
}
