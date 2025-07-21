/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tchevrie <tchevrie@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/11 00:44:51 by tchevrie          #+#    #+#             */
/*   Updated: 2022/10/05 04:32:47 by tchevrie         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

int counter(char *str)
{
  int i;

  i = 0;
  while (str[i])
  {
    i++;
  }
  return(i);
}

int long_max(char *str)
{
  long  res = 9223372036854775807;
  int i = counter(str);

  if (i == 19 || i > 19)
  {
    while (i > 0)
    {
      if ((str[i - 1] - '0') > res % 10)
        return(2);
      res = res / 10;
      i--;
    }
  }
  return(0);
}

int long_min(char *str, bool *flags)
{  
  long  res;
  int i;

  res = 922337203685477580;
  i = counter(str);
  if ((str[i - 1] - '0') > 8)
    return(2);
  i--;
  if (i == 19 || i > 19)
  {
    while (i > 0)
    {
      if ((str[i - 1] - '0') > res % 10)
        return(2);
      res = res / 10;
      i--;
    }
  }
  *flags = true;
  return(0);
}

int handle_exit_status(char *str, int sign, bool  *flags_sign)
{
  int ret_val;

  if (sign == 1)
  {
    ret_val = long_max(str);
    return(ret_val);
  }
  if (sign == -1)
  {
    ret_val = long_min(str, flags_sign);
    return(ret_val);
  }
  return(0);
}

int	ft_atoi(char *str)
{
	long		nbr;
	int		sign;
	size_t	i;
  bool  flags_sign = false;

	nbr = 0;
	sign = 1;
	i = 0;
	while (str[0] == '0' || str[i] == ' ' || ('\t' <= str[i] && str[i] <= '\r'))
		i++;
	if (str[i] == '+')
  {
		i++;
  }
	else if (str[i] == '-')
	{
		sign *= -1;
		i++;
	}
  if (str[i] == '9' || counter(&str[i]) > 19)
  {
    int size = handle_exit_status(&str[i], sign, &flags_sign);
    if (size != 0)
    {
      if (flags_sign)
        return(0);
      printf("minishell: exit: %s: numeric argument required\n", str);
      return(size);
    }
  }
	while ('0' <= str[i] && str[i] <= '9')
	{
		nbr = nbr * 10 + str[i] - '0';
		i++;
	}
	return (nbr * sign);
}
