/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   shellcode.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/11 17:50:52 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/20 23:50:54 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

uint8_t shellcode_x32[] =
{

	0xe8, 0x0e, 0x00, 0x00, 0x00, 0x2e, 0x2e, 0x2e,
	0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e, 0x2e,
	0x2e, 0x2e, 0x0a, 0x59, 0x53, 0x50, 0x52, 0xbb,
	0x01, 0x00, 0x00, 0x00, 0xb8, 0x04, 0x00, 0x00,
	0x00, 0xba, 0x0e, 0x00, 0x00, 0x00, 0xcd, 0x80,
	0x5a, 0x58, 0x5b, 0xe9, 0xd0, 0x6f, 0xfb, 0xf7

};

uint8_t payload[] =
{
	// xored 'a'
	0x50, 0x57, 0x56, 0x52, 0x41, 0x50, 0xe8, 0x0c, 0x00, 0x00,
	0x00, 0x2e, 0x2e, 0x2e, 0x57, 0x4f, 0x4f, 0x44, 0x59, 0x2e,
	0x2e, 0x2e, 0x0a, 0xbf, 0x01, 0x00, 0x00, 0x00, 0x5e, 0xba,
	0x0c, 0x00, 0x00, 0x00, 0xb8, 0x01, 0x00, 0x00, 0x00, 0x0f,
	0x05, 0x4d, 0x31, 0xc0, 0x48, 0x31, 0xc0, 0xb8, 0x42, 0x00,
	0x00, 0x00, 0x4c, 0x8d, 0x05, 0x00, 0x00, 0x00, 0x00, 0x83,
	0xf8, 0x00, 0x74, 0x13, 0x48, 0x31, 0xd2, 0x41, 0x8a, 0x10,
	0x80, 0xf2, 0x61, 0x41, 0x88, 0x10, 0xff, 0xc8, 0x49, 0xff,
	0xc0, 0xeb, 0xe8, 0x41, 0x58, 0x5a, 0x5e, 0x5f, 0x58, 0xe9,
	0xba, 0xba, 0xfe, 0xca,
};

unsigned int	get_payload_size64(void)
{
	return (sizeof(payload));
}

unsigned int	get_payload_size32(void)
{
	return (sizeof(shellcode_x32));
}

uint8_t	*get_shellcode_x64(uint8_t *dst, uint8_t *key, t_bdata bdata)
{
	printf("\tsizeof shellcode: %lu\n", bdata.payload_size);

	Elf64_Off v1;
	v1 =  bdata.s_size - (bdata.original_entrypoint - bdata.s_addr);
	printf("\t1 size insert:\t0x%lx\n", v1);
	ft_memcpy(&payload[48], &v1, sizeof(int));

	Elf64_Off v2;
	v2 = bdata.original_entrypoint - (bdata.p_vaddr + bdata.p_size) - 55 - 4;
	printf("\t2 addr insert:\t0x%lx\n", v2);
	ft_memcpy(&payload[55], &v2, sizeof(int));

	Elf64_Addr v3;
 	v3 = (bdata.original_entrypoint - (bdata.p_vaddr + bdata.p_size)) - bdata.payload_size;
	printf("\toriginal_entrypoint insert:\t0x%lx\n", v3);
	ft_memcpy(&payload[bdata.payload_size - sizeof(int)], &v3, sizeof(int));

	(void)key;
	for (size_t i = 0 ; i < bdata.payload_size ; i++)
		dst[i] = payload[i];

	return (dst);
}

uint8_t	*get_shellcode_x32(uint8_t *dst, t_bdata bdata)
{
//	modify_payload(shellcode_x32, SHELLSIZE, bdata.old_entry, bdata.p_vaddr2);
	(void)bdata;
	for (int i = 0 ; i < SHELLSIZE ; i++)
		dst[i] = shellcode_x32[i];
	return (dst);
}
