/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 19:07:24 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/14 22:24:27 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "malloc.h"

int		ft_putc(int n)
{
	static char t[16] = "0123456789abcdef";

	write(1, &t[n], 1);
	return (1);
}

int		ft_putn(size_t n, size_t base)
{
	if (n >= base)
		ft_putn(n / base, base);
	ft_putc(n % base);
	return (1);
}

int		malloc_printf(const char *fmt, ...)
{
	va_list	vargs;
	size_t	a;

	va_start(vargs, (char*)fmt);
	while (*fmt)
	{
		if (*fmt == '%')
		{
			fmt++;
			if (*fmt == 'p')
				write(1, "0x", 2);
			a = va_arg(vargs, size_t);
			ft_putn(a, *fmt == 'p' ? 16 : 10);
			if (*fmt == 'z')
				fmt += 1;
			fmt += 1 + *fmt == 'p' ? 0 : 1;
		}
		else
			write(1, fmt++, 1);
	}
	return (1);
}

void	print_zone(size_t type)
{
	struct s_block	*start;

	start = g_malloc->large;
	if (type == TINY)
	{
		write(1, "TINY:\n", 7);
		start = g_malloc->tiny;
	}
	else if (type == SMALL)
	{
		write(1, "SMALL:\n", 8);
		start = g_malloc->small;
	}
	else
		write(1, "LARGE:\n", 8);
	while (start)
	{
		if (!start->free)
			malloc_printf("%p - %p | %zu bytes\n", start->ptr,
					start->ptr + start->alloc_size, start->alloc_size);
		start = start->next;
	}
}

void	show_alloc_mem(void)
{
	print_zone(TINY);
	print_zone(SMALL);
	print_zone(0);
}
