/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer_core.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 16:51:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/03 23:24:56 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int	packer_core(t_packer *pack)
{
    void	*new;
    int		fd;

	get_arch(pack);
	return (0);

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
}
