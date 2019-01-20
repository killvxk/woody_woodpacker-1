/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   misc.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/20 20:23:29 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/20 21:11:23 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

void	*ft_memcpy(void *dst, const void *src, size_t n)
{
	char	*s1;
	char	*s2;
	size_t	count;

	s1 = (char*)dst;
	s2 = (char*)src;
	count = -1;
	while (++count < n)
		s1[count] = s2[count];
	return (s1);
}

int	ft_isalnum(int c)
{
	if (c >= 65 && c <= 90)
		return (1);
	else if (c >= 97 && c <= 122)
		return (1);
	else if (c >= 48 && c <= 57)
		return (1);
	else
		return (0);
}

size_t	ft_strlen(const char *src)
{
	size_t count;

	count = 0;
	while (src[count])
		count++;
	return (count);
}

int		valid_key(char **input, int ac)
{
	if (ac == 3 && ft_strlen(input[2]) == KEY_SIZE)
			return (SUCCESS);
	return (ERROR);
}

int		generate_key(uint8_t *key, char **input, int ac)
{
	int		fd;

	if (valid_key(input, ac) == SUCCESS)
		ft_memcpy(key, input[2], KEY_SIZE);
	else
	{
		fd = open("/dev/random", O_RDONLY);
		if (fd < 0)
		{
			perror("open :");
			return (ERROR);
		}
		for (int i = 0; i < KEY_SIZE; i++)
		{
			while (ft_isalnum(key[i]) == 0)
			{
				if (read(fd, &key[i], 1) == -1)
				{
					perror("read: ");
					close(fd);
					return (ERROR);
				}
			}
		}
		close(fd);
	}
	return (SUCCESS);
}
