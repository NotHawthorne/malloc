/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 03:12:59 by alkozma           #+#    #+#             */
/*   Updated: 2020/02/18 19:47:12 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MALLOC_H
# define MALLOC_H

# include <sys/mman.h>
# include <stdio.h>
# include <unistd.h>
# include <sys/types.h>
# include <string.h>
# include <errno.h>
# include <pthread.h>
# include <signal.h>
# include <stdarg.h>

# define TINY 192
# define SMALL 1088
# define MIN_ALLOCATIONS 100

typedef struct s_block	t_block;

struct			s_block
{
	int			free;
	size_t		size;
	size_t		alloc_size;
	void		*ptr;
	int			alloc_num;
	t_block		*next;
	t_block		*prev;
	t_block		*tail;
};

struct			s_malloc
{
	t_block		*tiny;
	t_block		*small;
	t_block		*large;
	size_t		allocations;
	size_t		allocated;
	int			page_size;
	void		*last_request;
	int			last_op;
};

struct s_malloc	*g_malloc;

void			*malloc(size_t size);
void			free(void *ptr);
void			*realloc(void *ptr, size_t size);

void			show_alloc_mem(void);

void			*ft_memcpy(void *dst, const void *str, size_t n);
void			*ft_memset(void *b, int c, size_t len);

void			init_malloc(void);
struct s_block	*fetch_block(size_t size);
struct s_block	*extend_zone(size_t size, struct s_block *zone);
struct s_block	*new_zone(size_t page, size_t allocation_size,
							size_t allocations, size_t cur_allocs);
size_t			count_pages(size_t block_size, int allocations);
size_t			count_allocs(struct s_block *zone);
struct s_block	*search_zone(struct s_block *zone, void *ptr);
struct s_block	*search_zones(void *ptr);
int				malloc_printf(const char *fmt, ...);
size_t			malloc_calc(struct s_block *block);

#endif
