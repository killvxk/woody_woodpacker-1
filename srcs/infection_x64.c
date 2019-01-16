/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_x64.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:56:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/16 12:23:42 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

static int		modify_segment(t_packer *pack, Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	int			inc;
	Elf64_Phdr *p_hdr;

	inc = 0;
	p_hdr = (Elf64_Phdr*)((char*)e_hdr + e_hdr->e_phoff);
	for (int i = 0 ; i < e_hdr->e_phnum ; i++)
	{
		if (!p_hdr)
			return (ERROR);
		if (inc == 1)
			p_hdr->p_offset += PAGESIZE;
		else if (p_hdr->p_type == PT_LOAD && (p_hdr->p_flags == (PF_X | PF_R)))
		{

			bdata->p_size = p_hdr->p_filesz;
			bdata->p_vaddr = p_hdr->p_vaddr;
			bdata->p_offset = p_hdr->p_offset;

			bdata->original_entrypoint = e_hdr->e_entry;
			bdata->payload_entrypoint = p_hdr->p_vaddr + p_hdr->p_filesz;

			p_hdr->p_filesz += bdata->payload_size;
			p_hdr->p_memsz += bdata->payload_size;

			// debug
			printf("segment %d -> .text :\n", i);
			printf("\told_entry:\t0x%lx\n\tnew_entry:\t0x%lx\n", e_hdr->e_entry, bdata->payload_entrypoint);
			e_hdr->e_entry = bdata->payload_entrypoint;
			printf("\tp_size:\t\t0x%lx\n\tp_offset:\t0x%lx\n\tp_vaddr:\t0x%lx\n\n", bdata->p_size, bdata->p_offset, bdata->p_vaddr);

			inc = 1;
		   }
		p_hdr = next_segment_x64(pack, e_hdr, i);
	}
	return (SUCCESS);
}

static int		modify_section(t_packer *pack, Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	Elf64_Shdr	*s_hdr;

	s_hdr = (Elf64_Shdr*)((char*)e_hdr + e_hdr->e_shoff);
	for (int i = 0; i < e_hdr->e_shnum; i++)
	{
		if (!s_hdr)
			return (ERROR);
		if (s_hdr->sh_offset >= bdata->p_offset + bdata->p_size)
			s_hdr->sh_offset += PAGESIZE;
		else if (s_hdr->sh_addr == bdata->original_entrypoint)
		{
			bdata->s_size = s_hdr->sh_size;
			bdata->s_addr = s_hdr->sh_addr;
			bdata->s_offset = s_hdr->sh_offset;

			// debug
//			printf("section %d -> .text :\n", i);
//			printf("\ts_size:\t\t0x%lx\n\ts_offset:\t0x%lx\n\ts_addr:\t\t0x%lx\n", bdata->s_size, bdata->s_offset, bdata->s_addr);
			s_hdr->sh_size += bdata->payload_size;
		}
		/*
		if ((s_hdr->sh_offset + s_hdr->sh_size) == (bdata->p_offset + bdata->p_size))
		{
			bdata->fake_section_offset = e_hdr->e_shoff + (sizeof(*s_hdr) * (i + 1));
			printf("\tfake_section:\t0x%lx\n\n", bdata->fake_section_offset);
		}
		*/
		s_hdr = next_section_x64(pack, e_hdr, i);
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
	if ((infected->mapped = mmap(NULL, pack->st.st_size + PAGESIZE, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED, infected->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(infected->fd);
		return (ERROR);
	}
	lseek(infected->fd, pack->st.st_size + PAGESIZE, SEEK_SET);
	write(infected->fd, "0x0", 1);
	close(infected->fd);
	return (SUCCESS);
}

Elf64_Shdr		get_fake_section(t_packer *pack, t_bdata bdata)
{
	Elf64_Shdr	fake;
//	Elf64_Shdr	*s_hdr;
//	Elf64_Ehdr	*e_hdr = (Elf64_Ehdr*)pack->mapped;

	/*
	s_hdr = (Elf64_Shdr*)((char*)e_hdr + e_hdr->e_shoff - PAGESIZE);
	for (int i = 0; i < e_hdr->e_shnum; i++)
	{
		printf("\t\tsh_name:\t%u\n", s_hdr->sh_name);
		printf("\t\tsh_type:\t%u\n", s_hdr->sh_type);
		printf("\t\tsh_flags:\t%lu\n", s_hdr->sh_flags);
		printf("\t\tsh_addr:\t0x%lu\n", s_hdr->sh_addr);
		printf("\t\tsh_offset:\t%lu\n", s_hdr->sh_offset);
		printf("\t\tsh_size:\t%lu\n", s_hdr->sh_size);
		printf("\t\tsh_link:\t%u\n", s_hdr->sh_link);
		printf("\t\tsh_info:\t%u\n", s_hdr->sh_info);
		printf("\t\tsh_addralign:\t%lu\n", s_hdr->sh_addralign);
		printf("\t\tsh_entsize:\t%lu\n", s_hdr->sh_entsize);
		s_hdr = next_section_x64(pack, e_hdr, i);
	}
	*/
	bzero(&fake, sizeof(fake));
	fake.sh_name = 0x0;
	fake.sh_flags = SHF_ALLOC | SHF_EXECINSTR;
	fake.sh_type = SHT_PROGBITS;
	fake.sh_size = sizeof(bdata.payload_size);
	fake.sh_link = 0x0;
	fake.sh_addralign = 0x1;
	fake.sh_entsize = 0x0;
	(void)pack;
	return (fake);
}

static void	insert_shellcode(t_packer *pack, t_packer infected, t_bdata bdata)
{
	uint8_t fake_page[PAGESIZE] = {0};

	/* vital informations */
	size_t encrypt_begin = bdata.s_offset;
	size_t encrypt_end = bdata.s_offset + bdata.s_size;
	size_t payload_begin = bdata.p_offset + bdata.p_size;
	size_t payload_end = bdata.p_offset + bdata.p_size + PAGESIZE;
	size_t end_of_file = pack->st.st_size + PAGESIZE;

	printf("Adresses:\n");
	printf("\tencrypt_begin:\t0x%lx\n", encrypt_begin);
	printf("\tencrypt_end:\t0x%lx\n", encrypt_end);
	printf("\tpayload_begin:\t0x%lx\n", payload_begin);
	printf("\tpayload_end:\t0x%lx\n\n", payload_end);

	get_shellcode_x64(fake_page, bdata);
	size_t offset_dst = 0;
	size_t offset_src = 0;

	// from begin to payload
	while (offset_dst < payload_begin)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	// insert fake_page aka payload
	int i = 0;
	while (offset_dst < payload_end)
	{
		infected.mapped[offset_dst] = fake_page[i];
		offset_dst++;
		i++;
	}

	// go to end of file
	while (offset_dst < end_of_file)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	/*
	size_t offset_dst = 0;
	size_t offset_src = 0;

	printf("\n\tfrom %ld to %ld - encrypt_begin\n", offset_dst, encrypt_begin);
	while (offset_dst < encrypt_begin)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	printf("\tfrom %ld to %ld - encrypt_end\n", offset_dst, encrypt_end);
	while (offset_dst < encrypt_end)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	printf("\tfrom %ld to %ld- payload_begin\n", offset_dst, payload_begin);
	while (offset_dst < payload_begin)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	get_shellcode_x64(fake_page, bdata);
	ft_memcpy(&infected.mapped[offset_dst], fake_page, PAGESIZE);
	text_encrypt(fake_page, PAGESIZE);
	printf("\tfrom %ld to ", offset_dst);
	offset_dst += PAGESIZE;
	printf("%ld (insert fake_page)\n", offset_dst);

	printf("\tfrom %ld to %ld - payload_end\n", offset_dst, payload_end);
	while (offset_dst < payload_end)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	printf("\tfrom %ld to %ld - fake_section_offset \n", offset_dst, bdata.fake_section_offset);
	while (offset_dst < bdata.fake_section_offset)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}

	Elf64_Shdr fake_section = get_fake_section(pack, bdata);
	ft_memcpy(&infected.mapped[offset_dst], &fake_section, sizeof(Elf64_Shdr));
	printf("\tfrom %ld to ", offset_dst);
	offset_dst += sizeof(Elf64_Shdr);
	printf("%ld (insert struct)\n", offset_dst);

	printf("\tfrom %ld to %ld - end of file\n", offset_dst, end_of_file);
	while (offset_dst < end_of_file)
	{
		infected.mapped[offset_dst] = pack->mapped[offset_src];
		offset_dst++;
		offset_src++;
	}
	*/

}

int		infect_x64(t_packer *pack)
{
	int			ret;
	t_bdata		bdata;
	t_packer	infected;
	Elf64_Ehdr	*e_hdr;

	ret = ERROR;
	e_hdr = (Elf64_Ehdr*)pack->mapped;
	bdata.payload_size = get_payload_size64();
	if (create_binary(&infected, pack) == SUCCESS)
		if (modify_segment(pack, e_hdr, &bdata) == SUCCESS)
			if (modify_section(pack, e_hdr, &bdata) == SUCCESS)
			{
//				e_hdr->e_shnum += 1;
				e_hdr->e_shoff += PAGESIZE;
				insert_shellcode(pack, infected, bdata);
				ret = SUCCESS;
			}
	return (ret);
}
