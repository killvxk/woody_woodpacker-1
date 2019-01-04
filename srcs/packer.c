/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:52:01 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/03 21:11:52 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int		init_struct(t_packer *pack, const char *file)
{
	struct stat		st;

	if ((pack->fd = open(file, O_RDONLY)) < 0)
		return (raise_error(ERROR, "can't open given input."));
	if (fstat(pack->fd, &st) != 0)
    {
		close(pack->fd);
		return (raise_error(ERROR, "can't get file information."));
    }
	if ((pack->mapped = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, pack->fd, 0)) == MAP_FAILED)
		return (raise_error(ERROR, "can't allocate memory."));
	pack->offset = 0;
	pack->filesize = st.st_size;
	pack->end = pack->mapped + st.st_size;
	return (SUCCESS);
}

int		map_binary(const char *file)
{
	t_packer	pack;

	ft_bzero(&pack, sizeof(pack));
	init_struct(&pack, file);
	packer_core(&pack);
	if (munmap(pack.mapped, pack.filesize) == -1)
		return (raise_error(ERROR, "can't free the memory"));
	return (SUCCESS);
}

int		main(int ac, char **av)
{
    if (ac < 2)
    {
		printf("Missing argument. usage : ./%s [target]\n", *av);
		return (-1);
    }
    while (++av && (*av) != NULL)
    {
		if (map_binary(*av) == -1)
			return (ERROR);
    }
    return (SUCCESS);
}
