#include "packer.h"

int	packer_core(t_packer *pack)
{
  void	*new;
  int	fd;

  fd = open("woody", O_RDWR | O_CREAT, 0777);
  if (fd < 0)
    {
      perror("open : ");
      return (ERROR);
    }
  lseek(fd, pack->filesize, SEEK_SET);
  write(fd, "\0x0", 1);
  new = mmap(NULL, pack->filesize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (new == MAP_FAILED)
    {
      perror("mmap: ");
      return (-1);
    }
  memcpy(new, pack->mapped, pack->filesize);
  munmap(new, pack->filesize);
  close(fd);
  return (0);
}

int	map_binary(const char *file)
{
  int			fd;
  struct stat		st;
  t_packer		pack;

  if ((fd = open(file, O_RDONLY)) < 0)
    {
      perror("open: ");
      return (ERROR);
    }
  if (fstat(fd, &st) != 0)
    {    
      close(fd);
      perror("fstat: ");
      return (ERROR);
    }
  ft_bzero(&pack, sizeof(pack));
  if ((pack.mapped = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
    {
      perror("mmap1: ");
      return (ERROR);
    }
  pack.end = pack.mapped + st.st_size;
  pack.filesize = st.st_size;
  packer_core(&pack);
  close(fd);
  if (munmap(pack.mapped, st.st_size) == -1)
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
  av++;
  while ((*av) != NULL)
    {
      if (map_binary(*av) == -1)
	  return (ERROR);
      av++;
    }
  return (SUCCESS);
}
