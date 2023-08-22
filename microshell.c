/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emlamoth <emlamoth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:54:21 by emlamoth          #+#    #+#             */
/*   Updated: 2023/08/22 17:18:59 by emlamoth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct s_data
{
	char	***cmd;
	int		cmd_i;
	int		fd_in;
	int		fd_out;
	int		fd_next_in;
	int		*pid;
	int		pid_i;
}			t_data;

void	ft_putstr_fd(int fd, char *str)
{
	while(*str)
		write(fd, str++, 1);
}

int		is_token(char *argv)
{
	if(strncmp(argv, "|", 2) == 0 || strncmp(argv, ";", 2) == 0)
		return(1);
	return(0);
}

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
	// printf("cmd len : %d\n", len);
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
	// printf("arg len : %d\n", i);
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
	if (!a)
		return (NULL);
	b = (char *)a;
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
	if(!new)
		return (NULL);
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
	if(!command)
		return (NULL);
	while(argv[index])
	{
		j = 0;
		command[i] = (char **)ft_calloc(sizeof(char *), (count_arg(argv + index) + 1));
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
			command[i] = ft_calloc(sizeof(char *), 2);
			command[i][j] = ft_strdup(argv[index]);
			index++;
			i++;
		}
	}
	return (command);
}

int	ft_pipe(t_data *data)
{
	int fd[2];

	if(pipe(fd) == -1)
	{
		ft_putstr_fd(STDERR_FILENO, "error: fatal\n");
		return(-1);
	}
	data->fd_next_in = fd[0];
	data->fd_out = fd[1];
	return (0);
	
}

void	setio(t_data *data)
{
	if(data->cmd[data->cmd_i + 1] && strncmp(data->cmd[data->cmd_i + 1][0], "|", 2) == 0)
		dup2(data->fd_out, STDOUT_FILENO);
	if(data->cmd[data->cmd_i - 1] && strncmp(data->cmd[data->cmd_i - 1][0], "|", 2) == 0)
		dup2(data->fd_in, STDIN_FILENO);
	if(data->fd_out > 2)
		close(data->fd_out);
	if(data->fd_in > 2)
		close(data->fd_in);
	if(data->fd_next_in > 2)
		close(data->fd_next_in);
}

int	forking(t_data *data, char **argv, char **envp)
{
	
	data->pid[data->pid_i++] = fork();
	if(data->pid[data->pid_i] == 0)
	{
		setio(data);
		if(execve(argv[0], argv, envp) == -1)
		{
			ft_putstr_fd(STDERR_FILENO, "error: cannot execute ");
			ft_putstr_fd(STDERR_FILENO, argv[0]);
			ft_putstr_fd(STDERR_FILENO, "\n");
		}
	}
	else
	{
		if(data->fd_out > 2)
			close(data->fd_out);
		if(data->fd_in > 2)
			close(data->fd_in);
		data->fd_in = data->fd_next_in;
		dup2(STDIN_FILENO, 0);
		dup2(STDOUT_FILENO, 1);
	}
	return (0);
}


int	micro_cd(char **argv)
{
	int i;
	
	i = 0;
	while(argv[i])
		i++;
	if(i != 2)
	{
		ft_putstr_fd(STDERR_FILENO, "error: cd: bad arguments\n");
		return (-1);
	}	
	if(chdir(argv[1]) == -1)
	{
		ft_putstr_fd(STDERR_FILENO, "error: cd: ");
		ft_putstr_fd(STDERR_FILENO, argv[1]);
		ft_putstr_fd(STDERR_FILENO, "\n");
		return(-1);
	}
	return (0);
}

int	executer(t_data *data, char **envp)
{
	while(data->cmd[data->cmd_i])
	{
		if(data->cmd[data->cmd_i] && strncmp(data->cmd[data->cmd_i][0], "cd", 3) == 0)
		{
			if(micro_cd(data->cmd[data->cmd_i]) == -1)
				return (-1);
			data->cmd_i++;
		}
		else
		{
			if(data->cmd[data->cmd_i + 1] && strncmp(data->cmd[data->cmd_i + 1][0], "|", 2) == 0)
				if(ft_pipe(data) == -1)
					return (-1);
			if(forking(data, data->cmd[data->cmd_i], envp))
				break ;
			data->cmd_i++;
			if(data->cmd[data->cmd_i] && is_token(data->cmd[data->cmd_i][0]))
				data->cmd_i++;
		}
		
	}
	while(--data->pid_i)
	{
		waitpid(data->pid[data->pid_i], NULL, 0);
	}	
	return (0);
}

void	free3d(char ***ar)
{
	int i;
	int j;
	
	i = 0;
	while(ar[i])
	{
		j = 0;
		while(ar[i][j])
		{
			free(ar[i][j]);
			j++;
		}
		free(ar[i]);
		i++;
	}
	free(ar);
}
void	init_data(t_data *data, char **argv)
{
	data->cmd = NULL;
	data->cmd_i = 0;
	data->fd_out = 0;
	data->fd_next_in = 0;
	data->fd_in = 0;
	data->pid_i = 0;
	data->pid = ft_calloc(count_command(argv), sizeof(int));
}
void	print3d(t_data *data)
{
	int i;
	int j;

	i = 0;
	j = 0;
	while(data->cmd[i])
	{
		j = 0;
		while(data->cmd[i][j])
		{
			printf("%s, ", data->cmd[i][j]);
			j++;
		}
		printf("\n");
		i++;
	}
}


int main(int argc, char **argv, char **envp)
{
	t_data data;

	if(argc < 2)
		return (1);
	init_data(&data, argv + 1);
	data.cmd = make_arg(argv + 1);
	print3d(&data);
	executer(&data, envp);
	free3d(data.cmd);
	free(data.pid);
}
