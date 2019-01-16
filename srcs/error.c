/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ddinaut <ddinaut@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2019/01/03 20:05:31 by ddinaut           #+#    #+#             */
/*   Updated: 2019/01/14 12:15:21 by ddinaut          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "packer.h"

int		raise_error(int ret, const char *error)
{
	if (error)
		printf("error: %s\n", error);
	else
		printf("error\n");
	return (ret);
}
