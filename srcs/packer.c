/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:52:01 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/10 15:13:49 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int		init_struct(t_packer *pack, const char *target)
{
//	struct stat		st;

	if ((pack->fd = open(target, O_RDWR)) < 0)
	{
		perror("open: ");
		return (ERROR);
	}
	if (fstat(pack->fd, &pack->st) != 0)
    {
		perror("fstat: ");
		close(pack->fd);
		return (ERROR);
    }
	if ((pack->mapped = mmap(NULL, pack->st.st_size, PROT_READ | PROT_WRITE , MAP_PRIVATE, pack->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(pack->fd);
		return (ERROR);
	}
	return (SUCCESS);
}

int		map_target(const char *target)
{
	t_packer	pack;

	ft_bzero(&pack, sizeof(pack));
	if (init_struct(&pack, target) == ERROR)
		return (ERROR);
	printf("binary %s is loaded. begin infection\n", target);
	packer_core(&pack);
	close(pack.fd);
	if (munmap(pack.mapped, pack.st.st_size) == -1)
	{
		perror("munmap: ");
		return (ERROR);
	}
	return (SUCCESS);
}

int		main(int ac, char **av)
{
    if (ac < 2)
    {
		printf("Missing argument. usage : %s [target ...]\n", *av);
		return (-1);
    }
    while (++av && (*av) != NULL)
    {
		if (map_target(*av) == -1)
			return (ERROR);
    }
    return (SUCCESS);
}
