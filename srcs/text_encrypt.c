/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   text_encrypt.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/12 11:17:44 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/15 12:30:23 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int	generate_key(uint8_t *key)
{
	int		fd;

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
	return (SUCCESS);
}

int text_encrypt(uint8_t *data, unsigned int len)
{
	uint8_t	key[KEY_SIZE] = {0};

	if (generate_key(key) != SUCCESS)
		ft_strncpy((char*)key, KEY_VALUES, KEY_SIZE - 1);
	printf("key used to encrypt: '%s'\n", key);
	for (unsigned int i = 0; i < len ; i++)
		if (data[i] != 0)
			data[i] ^= key[i % KEY_SIZE];
	return (SUCCESS);
}
