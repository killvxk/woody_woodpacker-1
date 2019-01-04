/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 22:05:24 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/04 11:11:39 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int		get_arch(t_packer *pack)
{
	int ret;

	ret = ERROR;
	if ((*((unsigned char*)pack->mapped + EI_MAG0) != ELFMAG0) || \
		(*((unsigned char*)pack->mapped + EI_MAG1) != ELFMAG1) || \
		(*((unsigned char*)pack->mapped + EI_MAG2) != ELFMAG2) || \
		(*((unsigned char*)pack->mapped + EI_MAG3) != ELFMAG3))
		return (raise_error(ret, "target isn't well formated"));
	if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS32)
		ret = infect_x32(pack);
	else if (*((unsigned char*)pack->mapped + EI_CLASS) == ELFCLASS64)
		ret = infect_x64(pack);
	return (ret);
}

void	*search_dataseg(t_packer *pack)
{
    Elf64_Ehdr	*header;
    Elf32_Phdr	*segment;

    header = pack->mapped + pack->offset;
    pack->offset = sizeof(*header);
    segment = pack->mapped + pack->offset;
    pack->offset += segment->p_offset;
    segment = pack->mapped + pack->offset;
	if (segment != NULL)
		return (segment);
    return (NULL);
}
