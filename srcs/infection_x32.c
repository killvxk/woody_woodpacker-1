/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_x32.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:56:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/15 18:47:22 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

static int		modify_segment(t_packer *pack, Elf32_Ehdr *e_hdr, t_bdata *bdata)
{
	int			inc;
	Elf32_Phdr *p_hdr;

	inc = 0;
	p_hdr = (Elf32_Phdr*)((char*)e_hdr + e_hdr->e_phoff);
	for (int i = 0 ; i < e_hdr->e_phnum ; i++)
	{
		if (inc == 1)
			p_hdr->p_offset += PAGESIZE;
		else if (p_hdr->p_type == PT_LOAD && (p_hdr->p_flags == (PF_X | PF_R)))
		{
			bdata->p_vaddr2 = p_hdr->p_vaddr + p_hdr->p_filesz;
			bdata->old_entry = e_hdr->e_entry;
			e_hdr->e_entry = bdata->p_vaddr2;
			bdata->end_of_text = p_hdr->p_offset + p_hdr->p_filesz;
			p_hdr->p_filesz += SHELLSIZE_32;
			p_hdr->p_memsz += SHELLSIZE_32;
			inc = 1;
		}
		p_hdr = next_segment_x32(pack, e_hdr, i);
		if (p_hdr == NULL)
			return (ERROR);
	}
	return (SUCCESS);
}

static int		modify_section(t_packer *pack, Elf32_Ehdr *e_hdr, t_bdata *bdata)
{
	Elf32_Shdr	*s_hdr;

	s_hdr = (Elf32_Shdr*)((char*)e_hdr + e_hdr->e_shoff);
	for (int i = 0; i < e_hdr->e_shnum; i++)
	{
		if (s_hdr->sh_offset >= bdata->end_of_text)
			s_hdr->sh_offset += PAGESIZE;
		else if ((s_hdr->sh_size + s_hdr->sh_addr) == bdata->p_vaddr2)
			s_hdr->sh_size += SHELLSIZE_32;
		s_hdr = next_section_x32(pack, e_hdr, i);
		if (s_hdr == NULL)
			return (ERROR);
	}
	return (SUCCESS);
}

static int		create_binary(t_packer *infected, t_packer *pack)
{

	infected->fd = open(OUTPUT_NAME, O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (infected->fd == -1)
	{
		perror("fd: ");
		return (ERROR);
	}
	if ((infected->mapped = mmap(NULL, pack->st.st_size + PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_SHARED, infected->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(infected->fd);
		return (ERROR);
	}
	return (SUCCESS);
}

static void	insert_data(t_packer *pack, t_packer infected, t_bdata bdata)
{
	uint8_t fake_page[PAGESIZE] = {0};

	get_shellcode_x32(fake_page, bdata);
	write(infected.fd, pack->mapped, bdata.end_of_text);
	write(infected.fd, fake_page, PAGESIZE);
	write(infected.fd, &pack->mapped[bdata.end_of_text], pack->st.st_size - bdata.end_of_text);
}

int		infect_x32(t_packer *pack)
{
	int			ret;
	t_bdata		bdata;
	t_packer	infected;
	Elf32_Ehdr	*e_hdr;

	ret = ERROR;
	e_hdr = (Elf32_Ehdr*)pack->mapped;
	if (create_binary(&infected, pack) == SUCCESS)
		if (modify_segment(pack, e_hdr, &bdata) == SUCCESS)
			if (modify_section(pack, e_hdr, &bdata) == SUCCESS)
			{
				e_hdr->e_shoff += PAGESIZE;
				insert_data(pack, infected, bdata);
				ret = SUCCESS;
			}
	close(infected.fd);
	munmap(infected.mapped, infected.st.st_size + PAGESIZE);
	return (ret);
}
