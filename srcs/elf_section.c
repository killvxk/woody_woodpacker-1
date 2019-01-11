/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   elf_section.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/11 15:56:00 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/11 17:46:50 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

Elf32_Shdr	*next_section_x32(t_packer *pack, Elf32_Ehdr *e_hdr, size_t count)
{
	Elf32_Shdr *ret;

	ret = (Elf32_Shdr*)((char*)pack->mapped + (e_hdr->e_shoff + sizeof(Elf32_Shdr) * count));
	if (!ret || ((void*)ret >= (void*)pack->mapped + pack->st.st_size))
	{
		ft_putendl("Error: malformated file");
		return (NULL);
	}
	return (ret);
}

Elf64_Shdr	*next_section_x64(t_packer *pack, Elf64_Ehdr *e_hdr, size_t count)
{
	Elf64_Shdr *ret;

	ret = (Elf64_Shdr*)((char*)pack->mapped + (e_hdr->e_shoff + sizeof(Elf64_Shdr) * count));
	if (!ret || ((void*)ret >= (void*)pack->mapped + pack->st.st_size))
	{
		ft_putendl("Error: malformated file");
		return (NULL);
	}
	return (ret);
}
