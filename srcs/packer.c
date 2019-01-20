/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:52:01 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/20 23:20:18 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

static int	valid_magic(char *map)
{
	if ((*((unsigned char*)map + EI_MAG0) != ELFMAG0) || \
		(*((unsigned char*)map + EI_MAG1) != ELFMAG1) || \
		(*((unsigned char*)map + EI_MAG2) != ELFMAG2) || \
		(*((unsigned char*)map + EI_MAG3) != ELFMAG3))
		return (ERROR);

	if (SET_SIGNATURE == ENABLED && \
		*((uint32_t *)&map[EI_PAD]) == SIGNATURE)
	{
		printf("binary already infected ;_;\n");
		return (ERROR);
	}
	return (SUCCESS);
}

static void	begin(t_packer *pack, char **input, int ac)
{
	struct stat st;

	if ((pack->fd = open(input[1], O_RDONLY)) < 0)
	{
		perror("open: ");
		exit(ERROR);
	}
	if (fstat(pack->fd, &st) != 0)
    {
		perror("fstat: ");
		close(pack->fd);
		exit(ERROR);
    }

	if ((pack->map = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, pack->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(pack->fd);
		exit(ERROR);
	}
	pack->size = st.st_size;
	(void)ac;
//	generate_key(pack->key, input, ac);
}

static int	launching(t_packer *pack)
{
	if (valid_magic(pack->map) != SUCCESS)
	{
		printf("error: input is not an elf file\n");
		return (ERROR);
	}
	if (*((unsigned char*)pack->map + EI_CLASS) == ELFCLASS32)
		return (infect_x32(pack));
	else if (*((unsigned char*)pack->map + EI_CLASS) == ELFCLASS64)
		return (infect_x64(pack));
	printf("error: unknow architecture.\n");
	return (ERROR);
}

static int	ending(t_packer *pack)
{
	int ret;

	ret = close(pack->fd);
	if (ret == -1)
		perror("close: ");
	ret = munmap(pack->map, pack->size);
	if (ret == -1)
		perror("munmap: ");
	return (ret);
}

int		main(int ac, char **av)
{
	int			ret;
	t_packer	pack;

	ret = ERROR;
	if (ac == 2 || ac == 3)
	{
		begin(&pack, av, ac);
		ret = launching(&pack);
		ret = ending(&pack);
	}
	else
		printf("Wrong argument number.\n\tUsage : %s <target> <key>\n", *av);
	return (ret);
}
