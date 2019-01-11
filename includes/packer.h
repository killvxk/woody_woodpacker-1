/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:15:26 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/11 23:29:22 by ddinaut          ###   ########.fr       */
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

/*
** DEFINE
*/

# define SHELLSIZE		48
# define SHELLSIZE_32	48
# define PAGESIZE		4096
# define OUTPUT_NAME	"woody"

/*
** STRUCTURES
*/
typedef struct		s_packer
{
	int				fd;
	char			*mapped;
	void			*end;
	struct stat		st;
}					t_packer;

typedef struct		s_bdata
{
	unsigned int	p_vaddr;
	unsigned int	end_of_text;
	unsigned int	old_entry;
}					t_bdata;

/*
** FUNCTIONS
*/
int				packer(int ac, char **av);
int				raise_error(int ret, const char *error);
int				infect_x32(t_packer *pack);
int				infect_x64(t_packer *pack);

Elf32_Shdr		*next_section_x32(t_packer *pack, Elf32_Ehdr *e_hdr, size_t count);
Elf64_Shdr		*next_section_x64(t_packer *pack, Elf64_Ehdr *e_hdr, size_t count);
Elf32_Phdr		*next_segment_x32(t_packer *pack, Elf32_Ehdr *e_hdr, size_t count);
Elf64_Phdr		*next_segment_x64(t_packer *pack, Elf64_Ehdr *e_hdr, size_t count);

uint8_t			*get_shellcode_x64(uint8_t *dst, t_bdata bdata);
uint8_t			*get_shellcode_x32(uint8_t *dst, t_bdata bdata);

#endif
