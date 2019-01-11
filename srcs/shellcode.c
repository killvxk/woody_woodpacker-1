/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shellcode.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut.student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/11 17:50:52 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/11 23:25:39 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

uint8_t shellcode_x64[] =
{
	0xe8, 0x0e, 0x00, 0x00, 0x00, 0x2e, 0x2e, 0x2e,
	0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e,
	0x2e, 0x2e, 0x0a, 0x5e, 0x50, 0x57, 0x52, 0xb8,
	0x01, 0x00, 0x00, 0x00, 0xbf, 0x01, 0x00, 0x00,
	0x00, 0xba, 0x0e, 0x00, 0x00, 0x00, 0x0f, 0x05,
	0x5a, 0x5f, 0x58, 0xe9, 0xfc, 0xff, 0xff, 0xff
};

uint8_t shellcode_x32[] =
{
	0xe8, 0x0e, 0x00, 0x00, 0x00, 0x2e, 0x2e, 0x2e,
	0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e,
	0x2e, 0x2e, 0x0a, 0x59, 0x53, 0x50, 0x52, 0xbb,
	0x01, 0x00, 0x00, 0x00, 0xb8, 0x04, 0x00, 0x00,
	0x00, 0xba, 0x0e, 0x00, 0x00, 0x00, 0xcd, 0x80,
	0x5a, 0x58, 0x5b, 0xe9, 0xd0, 0x6f, 0xfb, 0xf7
};

static void	modify_payload(uint8_t *shellcode, int shellsize, unsigned int old_entry, unsigned int new_entry)
{
	int addr;

	addr = (old_entry - new_entry) - shellsize;
	ft_memcpy(&shellcode[shellsize - sizeof(int)], &addr, sizeof(int));
}

uint8_t	*get_shellcode_x64(uint8_t *dst, t_bdata bdata)
{
	modify_payload(shellcode_x64, SHELLSIZE, bdata.old_entry, bdata.p_vaddr);
	for (int i = 0 ; i < SHELLSIZE ; i++)
		dst[i] = shellcode_x64[i];
	return (dst);
}

uint8_t	*get_shellcode_x32(uint8_t *dst, t_bdata bdata)
{
	modify_payload(shellcode_x32, SHELLSIZE, bdata.old_entry, bdata.p_vaddr);
	for (int i = 0 ; i < SHELLSIZE ; i++)
		dst[i] = shellcode_x32[i];
	return (dst);
}
