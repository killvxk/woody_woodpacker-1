/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:15:26 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/10 18:05:09 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PACKER_H
# define PACKER_H

# include "libft.h"

# include <stdio.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <unistd.h>
# include <elf.h>

# define SUCCESS 0
# define ERROR -1

typedef struct		s_packer
{
	int				fd;
	char			*mapped;
	struct stat		st;
}					t_packer;

typedef struct		s_bdata
{
	uint64_t		p_vaddr;
	uint64_t	old_entry;
}					t_bdata;


int				packer(int ac, char **av);
int				packer_core(t_packer *pack);
int				raise_error(int ret, const char *error);

int				infect_x32(t_packer *pack);
int				infect_x64(t_packer *pack);

/*
** elf funcs
*/
int				check_elf_header(void *mapped);
Elf64_Shdr		*get_elf_section(Elf64_Ehdr *hdr);
Elf64_Shdr		*get_elf_section_id(Elf64_Ehdr *hdr, int id);
char    *elf_str_table(Elf64_Ehdr *hdr);

#endif
