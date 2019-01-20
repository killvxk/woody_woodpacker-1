/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_x64.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:56:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/20 23:51:04 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

static int		is_entry_segment(Elf64_Ehdr *e_hdr, Elf64_Phdr *p_hdr)
{
	if ((e_hdr->e_entry > p_hdr->p_vaddr) && \
		e_hdr->e_entry < (p_hdr->p_vaddr + p_hdr->p_filesz))
		return (SUCCESS);
	return (ERROR);
}

static int		is_entry_section(t_bdata *bdata, Elf64_Shdr *s_hdr)
{
	if ((bdata->original_entrypoint >= s_hdr->sh_addr) && \
		(bdata->original_entrypoint < s_hdr->sh_addr + s_hdr->sh_size))
		return (SUCCESS);
	return (ERROR);
}

static int		modify_segment(t_packer *pack, Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	int			inc;
	Elf64_Phdr *p_hdr;

	inc = 0;
	for (int i = 0 ; i < e_hdr->e_phnum ; i++)
	{
		if ((p_hdr = next_segment_x64(pack, e_hdr, i)) == NULL)
			return (ERROR);
		if (inc == 1)
			p_hdr->p_offset += PAGESIZE;
		if (is_entry_segment(e_hdr, p_hdr) == SUCCESS)
		{
			bdata->p_size = p_hdr->p_filesz;
			bdata->p_vaddr = p_hdr->p_vaddr;
			bdata->p_offset = p_hdr->p_offset;

			p_hdr->p_flags |= PF_W;
			bdata->original_entrypoint = e_hdr->e_entry;
			bdata->payload_entrypoint = p_hdr->p_vaddr + p_hdr->p_filesz;

			p_hdr->p_filesz += bdata->payload_size;
			p_hdr->p_memsz += bdata->payload_size;

			// debug
			printf("segment %d -> .text :\n", i);
			printf("\told_entry:\t0x%lx\n\tnew_entry:\t0x%lx\n", e_hdr->e_entry, bdata->payload_entrypoint);
			printf("\tp_size:\t\t0x%lx\n", bdata->p_size);
			printf("\tp_offset:\t0x%lx\n", bdata->p_offset);
			printf("\tp_vaddr:\t0x%lx\n\n", bdata->p_vaddr);

			e_hdr->e_entry = bdata->payload_entrypoint;
			inc = 1;
		}
	}
	return (SUCCESS);
}

static int		modify_section(t_packer *pack, Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	int			inc;
	Elf64_Shdr	*s_hdr;

	inc = 0;
	s_hdr = (Elf64_Shdr*)((char*)e_hdr + e_hdr->e_shoff);
	for (int i = 0; i < e_hdr->e_shnum; i++)
	{
		if (!(s_hdr = next_section_x64(pack, e_hdr, i)))
			return (ERROR);

		if (inc == 1)
			s_hdr->sh_offset += PAGESIZE;
		if (is_entry_section(bdata, s_hdr) == SUCCESS)
		{
			bdata->s_size = s_hdr->sh_size;
			bdata->s_addr = s_hdr->sh_addr;
			bdata->s_offset = s_hdr->sh_offset;

			// debug
			printf("section %d -> .text :\n", i);
			printf("\ts_size:\t\t0x%lx\n", bdata->s_size);
			printf("\ts_offset:\t0x%lx\n", bdata->s_offset);
			printf("\ts_addr:\t\t0x%lx\n\n", bdata->s_addr);
		}
		if ((s_hdr->sh_offset + s_hdr->sh_size) == (bdata->p_offset + bdata->p_size))
		{
			printf("last section section found %d:\n", i);
			s_hdr->sh_size += bdata->payload_size;
			inc = 1;
		}
	}
	return (SUCCESS);
}

static int	create_binary(t_packer *infected, t_packer *pack)
{
	infected->fd = open(OUTPUT_NAME, O_RDWR | O_CREAT | O_TRUNC, 0777);

	if (infected->fd == -1)
	{
		perror("fd: ");
		return (ERROR);
	}
	if ((infected->map = mmap(NULL, pack->size + PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_SHARED, infected->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(infected->fd);
		return (ERROR);
	}
	lseek(infected->fd, pack->size + PAGESIZE, SEEK_SET);
	write(infected->fd, "0x0", 1);
	close(infected->fd);
	return (SUCCESS);
}

static void	insert_shellcode(t_packer *pack, t_packer infected, t_bdata bdata)
{
	uint8_t fake_page[4096] = {0};

	size_t encrypt_begin = bdata.s_offset + (bdata.original_entrypoint - bdata.s_addr);
	size_t encrypt_end = bdata.s_offset + bdata.s_size;
	size_t payload_begin = bdata.p_offset + bdata.p_size;
	size_t payload_end = payload_begin + PAGESIZE;
	size_t end_of_file = pack->size + PAGESIZE;

	printf("Adresses:\n");
	printf("\tencrypt_begin:\t0x%lx -> %lu\n", encrypt_begin, encrypt_begin);
	printf("\tencrypt_end:\t0x%lx -> %lu\n", encrypt_end, encrypt_end);
	printf("\tpayload_begin:\t0x%lx -> %lu\n", payload_begin, payload_begin);
	printf("\tpayload_end:\t0x%lx -> %lu\n", payload_end, payload_end);
	printf("\tend_of_file:\t0x%lx -> %lu\n\n", end_of_file, end_of_file);

	size_t offset_dst = 0;
	size_t offset_src = 0;

	while (offset_dst < encrypt_begin)
	{
		infected.map[offset_dst] = pack->map[offset_src];
		offset_dst++;
		offset_src++;
	}

	while (offset_dst < encrypt_end)
	{
		infected.map[offset_dst] = pack->map[offset_src] ^ 0x61;
		offset_dst++;
		offset_src++;
	}

	while (offset_dst < payload_begin)
	{
		infected.map[offset_dst] = pack->map[offset_src];
		offset_dst++;
		offset_src++;
	}

	int i = 0;
	get_shellcode_x64(fake_page, pack->key, bdata);
	while (offset_dst < payload_end)
	{
		infected.map[offset_dst] = fake_page[i];
		offset_dst++;
		i++;
	}

	while (offset_dst < end_of_file)
	{
		infected.map[offset_dst] = pack->map[offset_src];
		offset_dst++;
		offset_src++;
	}

}

void	insert_signature(t_packer *pack)
{
	if (SET_SIGNATURE == ENABLED)
	{
		printf("\t\tsignature set : %s\n", &pack->map[EI_PAD]);
		*(uint32_t *)&pack->map[EI_PAD] = SIGNATURE;
	}
}

int		infect_x64(t_packer *pack)
{
	int			ret;
	t_bdata		bdata;
	t_packer	infected;
	Elf64_Ehdr	*e_hdr;

	ret = ERROR;
	e_hdr = (Elf64_Ehdr*)pack->map;
	bdata.payload_size = get_payload_size64();
	if (create_binary(&infected, pack) == SUCCESS)
		if (modify_segment(pack, e_hdr, &bdata) == SUCCESS)
			if (modify_section(pack, e_hdr, &bdata) == SUCCESS)
			{
				e_hdr->e_shoff += PAGESIZE;
				insert_signature(pack);
				insert_shellcode(pack, infected, bdata);
				ret = SUCCESS;
			}
	munmap(infected.map, pack->size + PAGESIZE);
	return (ret);
}
