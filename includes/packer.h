/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:15:26 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/20 21:11:05 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PACKER_H
# define PACKER_H

# include <stdlib.h>
# include <stdio.h>
# include <fcntl.h>
# include <sys/stat.h>
# include <sys/mman.h>
# include <sys/types.h>
# include <unistd.h>
# include <elf.h>

/*
** DEFINE
*/

# define SUCCESS 0
# define ERROR -1

# define ENABLED 0
# define DISABLED -1

# define SHELLSIZE		101
# define SHELLSIZE_32	48
# define PAGESIZE		4096
# define OUTPUT_NAME	"woody"
# define KEY_SIZE		16
# define KEY_VALUES		"0123456789abcdef"

# define SET_SIGNATURE	DISABLED
# define SIGNATURE		0xbabababa

/*
** STRUCTURES
*/

typedef struct		s_packer
{
	int				fd;
	off_t			size;
	char			*map;
	uint8_t			key[KEY_SIZE];
}					t_packer;

typedef struct		s_bdata
{
	uint64_t	p_size;
	Elf64_Addr	p_vaddr;
	Elf64_Off	p_offset;

	uint64_t	s_size;
	Elf64_Addr	s_addr;
	Elf64_Off	s_offset;

	Elf64_Addr	original_entrypoint;
	Elf64_Addr	payload_entrypoint;
	size_t		payload_size;

	/* old */
	unsigned int	p_vaddr2;
	unsigned int	end_of_text;
	unsigned int	old_entry;
}					t_bdata;

/*
** FUNCTIONS
*/
int				packer(int ac, char **av);
int				infect_x32(t_packer *pack);
int				infect_x64(t_packer *pack);

Elf32_Shdr		*next_section_x32(t_packer *pack, Elf32_Ehdr *e_hdr, size_t count);
Elf64_Shdr		*next_section_x64(t_packer *pack, Elf64_Ehdr *e_hdr, size_t count);

Elf32_Phdr		*next_segment_x32(t_packer *pack, Elf32_Ehdr *e_hdr, size_t count);
Elf64_Phdr		*next_segment_x64(t_packer *pack, Elf64_Ehdr *e_hdr, size_t count);

uint8_t			*get_shellcode_x64(uint8_t *dst, uint8_t *key, t_bdata bdata);
uint8_t			*get_shellcode_x32(uint8_t *dst, t_bdata bdata);
unsigned int    get_payload_size64(void);

/* lib */
void			*ft_memcpy(void *dst, const void *src, size_t n);
size_t			ft_strlen(const char *src);
int				ft_isalnum(int c);
int				generate_key(uint8_t *key, char **input, int ac);


#endif
