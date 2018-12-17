#ifndef PACKER_H
# define PACKER_H

# include "libft.h"
# include "printf.h"

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
  int		fd;
  unsigned int	offset;
  unsigned int	filesize;
  void		*mapped;
  void		*oep;	// original entrypoint
  void		*end;
}		t_packer;

int	packer(int ac, char **av);
int	packer_core(t_packer *pack);


#endif
