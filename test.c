/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: alkozma <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/01/27 09:40:56 by alkozma           #+#    #+#             */
/*   Updated: 2020/01/27 17:53:08 by alkozma          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>

struct s_block
{
	void			*mem;
	int				free;
	size_t			size;
	size_t			used;
	struct s_block	*left;
	struct s_block	*right;
};

struct s_zone
{
	struct s_block	*blocks;
	size_t			allocations;
	size_t			size;
	size_t			used;
};

struct s_malloc
{
	struct s_zone	*tiny;
	struct s_zone	*small;
	struct s_zone	*large;
	size_t			allocations;
	size_t			size;
	size_t			used;
};

struct s_malloc	*g_malloc;

void	insert(struct s_block *tree, struct s_block *new)
{
	printf("going deep\n");
	if (!tree || !new)
		return ;
	if (!tree->left)
		printf("EMPTY LEFT\n");
	if (!tree->left && !tree->right)
		tree->left = new;
	else if (!tree->right && tree->left && tree->size == new->size)
	{
		printf("new r\n");
		tree->right = new;
	}
	else if (!tree->left && tree->right && tree->size == new->size)
	{
		printf("new l\n");
		tree->left = new;
	}
	else
		insert(new->size >= tree->size ? tree->right : tree->left, new);
}

size_t	pages(size_t page_size, size_t block_size, int allocs)
{
	int	i;
	size_t	ret;

	i = 0;
	ret = 0;
	while (i < allocs)
	{
		ret++;
		i = ret * page_size / block_size;
	}
	return (ret + 1);
}

void	zone_init(struct s_zone *zone, size_t blocksize)
{
	size_t	offset;
	struct s_block	*tmp;
	struct s_block	*tree;

	offset = sizeof(struct s_zone);
	zone->allocations = 0;
	zone->blocks = (struct s_block*)((void*)zone + sizeof(struct s_zone));
	tmp = NULL;
	tree = NULL;
	while (zone->allocations != 100)
	{
		if (tree == NULL)
			tree = (void*)zone + offset;
		tmp = (void*)zone + offset;
		tmp->mem = (void*)zone + offset + sizeof(struct s_block);
		tmp->free = 1;
		tmp->size = blocksize;
		tmp->used = 0;
		tmp->left = 0;
		tmp->right = 0;
		if (tree != tmp)
		{
			printf("inserting\n");
			insert(tree, tmp);
		}
		else
			printf("not inserting\n");
		offset += blocksize;
		zone->allocations++;
	}
	zone->used = sizeof(struct s_zone) + (100 * sizeof(struct s_block));
}

void	print_tree(struct s_block *zone, int depth)
{
	char	s[] = "                                                                                                                            ";
	if (!zone)
		return ;
	print_tree(zone->left, depth + 1);
	printf("%.*s SZ: %zu | FREE: %d\n", depth * 2, s, zone->size, zone->free);
	print_tree(zone->right, depth + 1);
}

void	*large_alloc(size_t size)
{
	void			*mem;
	size_t			page_ct;
	struct s_block	*tmp;
	int				initd_zone;

	page_ct = pages(getpagesize(), size, 1);
	printf("init %zu %d pages\n", page_ct, getpagesize());
	mem = mmap(0, page_ct + 1 * getpagesize(), PROT_READ | PROT_WRITE,
				MAP_ANON | MAP_PRIVATE, -1, 0);
	initd_zone = 0;
	if (!g_malloc->large)
	{
		g_malloc->large = mem;
		g_malloc->large->allocations = 1;
		g_malloc->large->size = size;
		g_malloc->large->used = sizeof(struct s_block) + sizeof(struct s_zone);
		g_malloc->large->blocks = NULL;
		initd_zone = sizeof(struct s_zone);
	}
	tmp = mem + initd_zone;
	tmp->mem = mem + sizeof(struct s_block) + initd_zone;
	tmp->free = 0;
	tmp->size = size;
	tmp->used = sizeof(struct s_block) + initd_zone;
	tmp->left = NULL;
	tmp->right = NULL;
	if (initd_zone)
		g_malloc->large->blocks = tmp;
	else
		insert(g_malloc->large->blocks, tmp);
	return (tmp);
}

void	malloc_init(void)
{
	void	*tiny;
	size_t	page_ct;
	size_t	small_page;

	page_ct = pages(getpagesize(), 1024, 100);
	small_page = pages(getpagesize(), 1024 * 32, 100);
	// get all the memory for zones at once
	tiny = mmap(0, (page_ct + small_page) * getpagesize(), 
				PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	g_malloc = tiny;
	g_malloc->tiny = tiny + sizeof(struct s_malloc);
	g_malloc->small = tiny + sizeof(struct s_malloc) + (page_ct * getpagesize());
	g_malloc->large = NULL;
	g_malloc->allocations = 200;
	g_malloc->size = (page_ct + small_page) * getpagesize();
	g_malloc->used = sizeof(struct s_malloc) + (2 * sizeof(struct s_zone));
	zone_init(g_malloc->tiny, 1024);
	zone_init(g_malloc->small, 1024 * 32);
}

void	*get_unused_block(size_t size, struct s_block *block)
{
	struct s_block	*tmp;

	if (!block)
		return (NULL);
	tmp = block;
	if (tmp && tmp->free == 0)
	{
		if (tmp->size > size)
		{
			tmp->free = 0;
			tmp->size = size;
			return (tmp);
		}
		else
			return (get_unused_block(size, block->left));
	}
	else
		return (get_unused_block(size, size >= block->size
					? block->left : block->right));
	return (0);
}

void	*malloc(size_t size)
{
	struct s_block	*ret;
	static int	malloc_initd;

	if (!malloc_initd++)
		malloc_init();
	ret = NULL;
	if (size < 1024)
		ret = get_unused_block(size, g_malloc->tiny->blocks);
	else if (size < 1024 * 32)
		ret = get_unused_block(size, g_malloc->small->blocks);
	else
		ret = large_alloc(size);
	return (ret ? ret->mem : 0);
}

void	free(void *ptr)
{
	if (!g_malloc || !ptr)
		return ;
}

int		main(void)
{
	char	*test = malloc(2);
	char	*test2 = malloc(1024 * 31);

	char	**testarr;

	if (test == NULL)
		return (printf("error\n"));
	test[0] = 'H';
	test[1] = 'i';
	test[2] = ' ';
	test[3] = '!';
	test[4] = '!';
	test[5] = '?';
	test2[0] = 'n';
	test2[1] = 'o';
	printf("[%s] [%s]\n", test, test2);
	testarr = malloc(sizeof(char*) * 10000);
	int	i = 0;
	while (i < 10)
	{
		printf("%d\n", i);
		if (!(testarr[i++] = malloc(32)))
			printf("fail\n");
	}
	//testarr[1][0] = 'a';
	//printf("[%s]\n", testarr[i - 1]);
	print_tree(g_malloc->tiny->blocks, 0);
}
