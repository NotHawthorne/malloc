/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   malloc.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/25 03:12:59 by alkozma           #+#    #+#             */
/*   Updated: 2020/01/27 20:11:31 by alkozma          ###   ########.fr       */
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

# define TINY 4096
# define SMALL (4096 * 32)
# define MIN_ALLOCATIONS 100

typedef struct	s_block	t_block;

struct	s_block
{
	int		free;
	size_t	size;
	void	*ptr;
	t_block	*next;
	t_block	*prev;
	t_block	*tail;
};

struct	s_malloc
{
	t_block	*tiny;
	t_block	*small;
	t_block	*large;
	size_t	allocations;
	size_t	allocated;
	int		page_size;
};

struct s_malloc	*g_malloc;

void			*malloc(size_t size);
void			free(void *ptr);
void			*realloc(void *ptr, size_t size);

void			init_malloc(void);
struct s_block	*fetch_block(size_t size);
struct s_block	*extend_zone(size_t size, struct s_block *zone);
struct s_block	*new_zone(size_t page, size_t allocation_size, size_t allocations);
size_t			count_pages(size_t block_size, int allocations);

#endif
