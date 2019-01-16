/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:52:01 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/14 12:22:04 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

static void	begin(t_packer *pack, const char *input)
{
	ft_bzero(pack, sizeof(t_packer));
	if ((pack->fd = open(input, O_RDONLY)) < 0)
	{
		perror("open: ");
		exit(ERROR);
	}
	if (fstat(pack->fd, &pack->st) != 0)
    {
		perror("fstat: ");
		close(pack->fd);
		exit(ERROR);
    }
	if ((pack->mapped = mmap(NULL, pack->st.st_size, PROT_READ | PROT_WRITE , MAP_PRIVATE, pack->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(pack->fd);
		exit(ERROR);
	}
}

static int	launching(t_packer *pack)
{
	if ((*((unsigned char*)pack->mapped + EI_MAG0) != ELFMAG0) || \
		(*((unsigned char*)pack->mapped + EI_MAG1) != ELFMAG1) || \
		(*((unsigned char*)pack->mapped + EI_MAG2) != ELFMAG2) || \
		(*((unsigned char*)pack->mapped + EI_MAG3) != ELFMAG3))
		return (raise_error(ERROR, "input file isn't well formated"));
	if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS32)
		return (infect_x32(pack));
	else if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS64)
		return (infect_x64(pack));
	return (raise_error(ERROR, "unknow architecture, abort"));
}

static int	ending(t_packer *pack)
{
	int ret;

	ret = close(pack->fd);
	if (ret == -1)
		perror("close: ");
	ret = munmap(pack->mapped, pack->st.st_size);
	if (ret == -1)
		perror("munmap: ");
	return (ret);
}

int		main(int ac, char **av)
{
	int			ret;
	t_packer	pack;

	ret = ERROR;
	if (ac == 2)
	{
		begin(&pack, av[1]);
		ret = launching(&pack);
		ret = ending(&pack);
	}
	else
		printf("Wrong argument number.\n\tUsage : %s <target>\n", *av);
	return (ret);
}
