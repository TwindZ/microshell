/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emlamoth <emlamoth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:54:21 by emlamoth          #+#    #+#             */
/*   Updated: 2023/08/21 17:18:38 by emlamoth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct s_data
{
	char ***command;
}			t_data;

int		is_token(char *argv)
{
	if(strncmp(argv, "|", 2) == 0 || strncmp(argv, ";", 2) == 0)
		return(1);
	return(0);
}

// void	executer(t_data *data, char **argv, char **envp)
// {
// 	int id;
	
// 	id = fork();
// 	if(id == 0)
// 	{
// 		execve(argv[0], argv, envp);
// 	}
// 	else
// 	{
// 		;
// 	}
// }
int		count_command(char **argv)
{
	int i;
	int len;

	len = 1;
	i = 0;
	while(argv[i])
	{
		if(is_token(argv[i]))
		{
			len++;
			i++;
			if(argv[i])
				len++;
		}	
		i++;
	}
	return (len);
}

int		count_arg(char **argv)
{
	int i;
	
	i = 0;
	while(argv[i] && is_token(argv[i]) == 0)
	{
		i++;
	}
	printf("len %d\n", i);
	return (i);
}

int	ft_strlen(char *str)
{
	int i;

	i = 0;
	while(str[i])
		i++;
	return (i);
}

void	*ft_calloc(int count, int size)
{
	void	*a;
	char	*b;
	int		i;

	i = 0;
	a = (malloc(count * size));
	b = (char *)a;
	if (!b)
		return (NULL);
	while (i < (count * size))
		b[i++] = '\0';
	return (a);
}

char	*ft_strdup(char *str)
{
	char *new;
	int i;
	
	i = 0;
	new = ft_calloc(sizeof(char), (ft_strlen(str) + 1));
	while(str[i])
	{
		new[i] = str[i];
		i++;
	}
	return (new);
}

char	***make_arg(char **argv)
{
	int i;
	int j;
	int index;
	char ***command;
	
	i = 0;
	j = 0;
	index = 0;
	command = (char ***)ft_calloc(sizeof(char **), (count_command(argv) + 1));
	while(argv[index])
	{
		j = 0;
		command[i] = (char **)ft_calloc(sizeof(char *), (count_arg(argv) + 1));
		while(argv[index] && is_token(argv[index]) == 0)
		{
			command[i][j] = ft_strdup(argv[index]);
			index++;
			j++;
		}
		j = 0;
		i++;
		if(argv[index] && is_token(argv[index]))
		{
			command[i] = calloc(sizeof(char *), 2);
			command[i][j] = ft_strdup(argv[index]);
			index++;
			i++;
		}
	}
	return (command);
}

int main(int argc, char **argv, char **envp)
{
	t_data data;
	int i;
	int j;
	(void)	argc;
	(void)	envp;
	i = 0;
	j = 0;
	if(argc < 2)
		return (1);
	data.command = make_arg(argv + 1);
	
	while(data.command[i])
	{
		j = 0;
		while(data.command[i][j])
		{
			printf("%s, ", data.command[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}

	// executer(&data, argv + 1, envp);
	// printf("%d\n", len);
}
