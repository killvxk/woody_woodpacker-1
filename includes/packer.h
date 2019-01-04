/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   packer.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/04 11:15:26 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/04 11:58:23 by ddinaut          ###   ########.fr       */
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

typedef struct	s_packer
{
  int			fd;
  unsigned int	offset;
  unsigned int	filesize;
  void			*mapped;
  void			*oep;	// original entrypoint
  void			*end;
}				t_packer;

int				packer(int ac, char **av);
int				packer_core(t_packer *pack);
int				raise_error(int ret, const char *error);
int				get_arch(t_packer *arch);
void			*search_dataseg(t_packer *pack);

int				infect_x32(t_packer *pack);
int				infect_x64(t_packer *pack);

#endif
