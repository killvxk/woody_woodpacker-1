/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:51:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/08 13:32:35 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int		get_arch(t_packer *pack)
{
	if ((*((unsigned char*)pack->mapped + EI_MAG0) != ELFMAG0) || \
		(*((unsigned char*)pack->mapped + EI_MAG1) != ELFMAG1) || \
		(*((unsigned char*)pack->mapped + EI_MAG2) != ELFMAG2) || \
		(*((unsigned char*)pack->mapped + EI_MAG3) != ELFMAG3))
		return (raise_error(ERROR, "target isn't well formated"));
	if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS32)
		return (infect_x32(pack));
	else if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS64)
		return (infect_x64(pack));
	return (raise_error(ERROR, "unknow architecture, abort"));
}

int		packer_core(t_packer *pack)
{
	int		ret;
    void	*new;
    int		fd;

	ret = get_arch(pack);
	(void)new;
	(void)fd;
	return (ret);

/*
**** --- CREATE INFECTED BINARY --- *****

    fd = open("infected", O_RDWR | O_CREAT, 0777);
	if (!fd)
		return (raise_error(ERROR, "can't create infected binary"));
    lseek(fd, pack->filesize, SEEK_SET);
    write(fd, "\0x0", 1);
    if ((new = mmap(NULL, pack->filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
		close(fd);
		return (raise_error(ERROR, "can't allocate memory to populate infected binary"));
    }
    memcpy(new, pack->mapped, pack->filesize);
    munmap(new, pack->filesize);
    close(fd);
    return (0);
*/
}
