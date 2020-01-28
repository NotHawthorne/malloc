/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 19:07:24 by alkozma           #+#    #+#             */
/*   Updated: 2020/01/27 20:36:10 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

void	print_allocated_mem(size_t type)
{
	struct s_block	*start;

	if (type == TINY)
		start = g_malloc->tiny;
	else if (type == SMALL)
		start = g_malloc->small;
	else
		start = g_malloc->large;
	while (start)
	{
		printf("%p \"%s\" [size: %zu][free: %d]\n", start, start->ptr, start->size, start->free);
		start = start->next;
	}
}

void	print_zone(size_t type)
{
	struct s_block	*start;
	
	start = g_malloc->large;
	if (type == TINY)
	{
		printf("TINY:\n");
		start = g_malloc->tiny;
	}
	else if (type == SMALL)
	{
		printf("SMALL:\n");
		start = g_malloc->small;
	}
	else
		printf("LARGE:\n");
	while (start)
	{
		if (!start->free)
			printf("%p - %p : %zu bytes\n", start, start + start->size + sizeof(struct s_block), start->size);
		start = start->next;
	}
}

void	show_alloc_mem(void)
{
	print_zone(TINY);
	print_zone(SMALL);
	print_zone(0);
}

int		main(void)
{
	char	*data[10000];
	int		i;

	i = 0;
	printf("main test\n");
	while (++i < 10)
	{
		data[i - 1] = malloc(i * 100);
		data[i - 1][0] = 'x';
	}
	data[8][0] = 'X';
	printf("[%s]\n", data[2]);
	show_alloc_mem();
	free(data[2]);
	show_alloc_mem();
}
