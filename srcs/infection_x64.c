/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_x64.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:56:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/04 12:29:02 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

/*
  step by step
  * Increase p_shoff by PAGE_SIZE in the ELF header
  * Patch the insertion code (parasite) to jump to the entry point
	(original)
  * Locate the text segment program header
	  * Modify the entry point of the ELF header to point to the new
		code (p_vaddr + p_filesz)
	  * Increase p_filesz by account for the new code (parasite)
	  * Increase p_memsz to account for the new code (parasite)
  * For each phdr who's segment is after the insertion (text segment)
	  * increase p_offset by PAGE_SIZE
  * For the last shdr in the text segment
	  * increase sh_len by the parasite length
  * For each shdr who's section resides after the insertion
	  * Increase sh_offset by PAGE_SIZE
  * Physically insert the new code (parasite) and pad to PAGE_SIZE, into
	the file - text segment p_offset + p_filesz (original)
*/

int		infect_x64(t_packer *pack)
{
	Elf64_Ehdr	*header;

	printf("x64 binary located.\n");
	header = pack->mapped + pack->offset;
	(void)header;
	return (0);
}
