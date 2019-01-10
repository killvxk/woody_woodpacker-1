/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infection_x64.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:56:51 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/10 20:29:23 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

// printf("hello world!\n");
//unsigned char shellcode[] = "\xe8\x0e\x00\x00\x00\x48\x65\x6c\x6c\x6f\x2c\x20\x57\x6f\x72\x6c\x64\x21\x0a\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0e\x00\x00\x00\x0f\x05";

// printf("surprise!\n");
//unsigned char shellcode[] = "\xe8\x0a\x00\x00\x00\x73\x75\x72\x70\x72\x69\x73\x65\x21\x0a\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0a\x00\x00\x00\x0f\x05";
// suprise v2
//unsigned char shellcode[] = "\xe8\x0a\x00\x00\x00\x73\x75\x72\x70\x72\x69\x73\x65\x21\x0a\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0a\x00\x00\x00\x0f\x05\x00\x6a\x00\x00\x00\x00\xc3";
// surprise v3
//unsigned char shellcode[] = "\x50\x57\x52\xe8\x0a\x00\x00\x00\x73\x75\x72\x70\x72\x69\x73\x65\x21\x0a\x5e\x66\xb8\x01\x00\x66\xbf\x01\x00\xb2\x0a\x0f\x05\x5a\x5f\x58\x6a\x00\xc3";

unsigned char shellcode[] = "\x50\x57\x52\xe8\x0a\x00\x00\x00\x73\x75\x72\x70\x72\x69\x73\x65\x21\x0a\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0a\x00\x00\x00\x0f\x05\x5a\x5f\x58";

//print2
//unsigned char shellcode[] = "\xe8\x0e\x00\x00\x00\x48\x65\x6c\x6c\x6f\x2c\x20\x57\x6f\x72\x6c\x64\x21\x0a\x5e\xb8\x01\x00\x00\x00\xbf\x01\x00\x00\x00\xba\x0e\x00\x00\x00\x0f\x05\xe9\xfc";

// bin/sh
//unsigned char shellcode[] = "\x31\xc0\x48\xbb\xd1\x9d\x96\x91\xd0\x8c\x97\xff\x48\xf7\xdb\x53\x54\x5f\x99\x52\x57\x54\x5e\xb0\x3b\x0f\x05";

unsigned char shellsize = sizeof(shellcode) + 7;

#define CALL_SC	(*(void (*)()) shellcode)();

size_t pagesize = 4096;

int		modify_segment(Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	int			inc;
	Elf64_Phdr *p_hdr;

	inc = 0;
	p_hdr = (Elf64_Phdr*)((char*)e_hdr + e_hdr->e_phoff);
	for (int i = -1 ; ++i < e_hdr->e_phnum ; p_hdr++)
	{
		if (inc == 1)
			p_hdr->p_offset += pagesize;
		else if (p_hdr->p_flags & PT_LOAD && (p_hdr->p_flags == (PF_X | PF_R)))
		{
			printf("\tText segment found\n");

			/* parasite v_addr aka new entrypoint */
			bdata->p_vaddr = p_hdr->p_vaddr + p_hdr->p_filesz;
			bdata->old_entry = e_hdr->e_entry;
			e_hdr->e_entry = bdata->p_vaddr;

			/* get end of our text section */
			bdata->p_vaddr = p_hdr->p_offset + p_hdr->p_filesz;
			printf("\tEnd of text section set to: 0x%lx\n", bdata->p_vaddr);

			p_hdr->p_filesz += shellsize;
			p_hdr->p_memsz +=shellsize;
			inc = 1;
		}
	}
	return (SUCCESS);
}

int		modify_section(Elf64_Ehdr *e_hdr, t_bdata *bdata)
{
	Elf64_Shdr	*s_hdr;

	s_hdr = (Elf64_Shdr*)((char*)e_hdr + e_hdr->e_shoff);
	for (int i = -1; ++i < e_hdr->e_shnum; s_hdr++)
	{
		if (s_hdr->sh_offset >= bdata->p_vaddr)
			s_hdr->sh_offset += pagesize;
		else
		{
			if (s_hdr->sh_size + s_hdr->sh_addr == bdata->p_vaddr)
				s_hdr->sh_size += shellsize;
		}
	}
	return (0);
}

int		create_binary(t_packer *infected, t_packer *pack)
{
	infected->fd = open("infected", O_RDWR | O_CREAT | O_TRUNC, 0777);
	if (infected->fd == -1)
	{
		perror("fd: ");
		return (ERROR);
	}
	if ((infected->mapped = mmap(NULL, pack->st.st_size + pagesize, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_SHARED, infected->fd, 0)) == MAP_FAILED)
	{
		perror("mmap: ");
		close(infected->fd);
		return (ERROR);
	}
	return (SUCCESS);
}

int		insert_shellcode(t_packer *pack, t_packer infected, t_bdata bdata)
{
	/* populate infected binary from begining of file to text segment */
	char test[4096] = {0}; /* populate new texte segment */
	uint32_t fuck = bdata.old_entry - bdata.p_vaddr - shellsize;
	memset(test, 'B', pagesize);

	printf("\t\twrite %lu bytes\n", bdata.p_vaddr);
	write(infected.fd, pack->mapped, bdata.p_vaddr);

//	\xe9\xfc\xff\xff\xff -> jmp 0x0000000 ->
// http://www.c-jump.com/CIS77/reference/Instructions_by_Opcode.html
	char jump[7] = {'\xeB', '\x00', '\x00', '\x00', '\x00', '\xc3', 0};
	memcpy(&jump[1], &fuck, 4);

//	*(unsigned long *) &jump[1] = bdata.old_entry;
	(void)fuck;

	printf("\t\tshellcode is %u sized\n", shellsize);
	printf("\t\twrite %lu sized\n", pagesize);
	memcpy(test, shellcode, shellsize - 7);
	memcpy(&test[shellsize - 7], jump, 7);

	/* inject shellcode */
	write(infected.fd, test, pagesize);

	printf("\t\twrite %lu bytes\n", pack->st.st_size - bdata.p_vaddr);
	write(infected.fd, &pack->mapped[bdata.p_vaddr], pack->st.st_size - bdata.p_vaddr + pagesize);
	return (SUCCESS);
}

int		infect_x64(t_packer *pack)
{
	t_bdata		bdata;
	t_packer	infected;
	Elf64_Ehdr	*e_hdr;

	e_hdr = (Elf64_Ehdr*)pack->mapped;

	/* write new binary */
	printf("Create new binary :\n");
	if (create_binary(&infected, pack) == ERROR)
		return (ERROR);

	/* Modify segment offsets */
	printf("Modify segment :\n");
	if (modify_segment(e_hdr, &bdata) == ERROR)
		return (ERROR);

	/* Modify section offsets */
	printf("Modify section :\n");
	if (modify_section(e_hdr, &bdata) == ERROR)
		return (ERROR);

	printf("Increasing section entry header by pagesize :\n");
	e_hdr->e_shoff += pagesize;

	if (insert_shellcode(pack, infected, bdata) == ERROR)
		return (ERROR);

	munmap(infected.mapped, infected.st.st_size + pagesize);

	close(infected.fd);
	return (SUCCESS);
}
