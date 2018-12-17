#include "packer.h"

int	init_struct(t_packer *pack, const char *file)
{
  struct stat		st;

  if ((pack->fd = open(file, O_RDONLY)) < 0)
    {
      perror("open: ");
      return (ERROR);
    }
  if (fstat(pack->fd, &st) != 0)
    {
      close(pack->fd);
      perror("fstat: ");
      return (ERROR);
    }
  if ((pack->mapped = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, pack->fd, 0)) == MAP_FAILED)
    {
      perror("mmap1: ");
      return (ERROR);
    }
  pack->offset = 0;
  pack->filesize = st.st_size;
  pack->end = pack->mapped + st.st_size;
  return (SUCCESS);
}

int	map_binary(const char *file)
{
  t_packer	pack;

  ft_bzero(&pack, sizeof(pack));
  init_struct(&pack, file);
  packer_core(&pack);
  if (munmap(pack.mapped, pack.filesize) == -1)
    {
      perror("munmap: ");
      return (ERROR);
    }
  return (SUCCESS);
}

int	main(int ac, char **av)
{
  if (ac < 2)
    {
      printf("error: missing argument. usage : ./%s [binary...]\n", *av);
      return (-1);
    }
  while (++av && (*av) != NULL)
    {
      if (map_binary(*av) == -1)
	  return (ERROR);
    }
  return (SUCCESS);
}
