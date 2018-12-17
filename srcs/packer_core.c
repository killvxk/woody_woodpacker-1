#include "packer.h"

void	*search_dataseg(t_packer *pack)
{
  Elf64_Ehdr	*header;
  Elf32_Phdr	*segment;

  header = pack->mapped + pack->offset;
  printf("name = %s\n", header->e_ident);

  pack->offset = sizeof(*header);
  segment = pack->mapped + pack->offset;
  //  printf("type = %u\n", segment->p_type);
  printf("%u %u %u\n", segment->p_offset, segment->p_filesz, segment->p_memsz);
  pack->offset += segment->p_offset;
  segment = pack->mapped + pack->offset;
  printf("%u %u %u\n", segment->p_offset, segment->p_filesz, segment->p_memsz);

  return (NULL);
}

int	packer_core(t_packer *pack)
{
  void	*new;
  void	*data_seg;
  int	fd;

  fd = open("woody", O_RDWR | O_CREAT, 0777);
  if (fd < 0)
    {
      perror("open : ");
      return (ERROR);
    }
  data_seg = search_dataseg(pack);

  close(fd);
  return (0);
  //  exit();
  (void)data_seg;
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
