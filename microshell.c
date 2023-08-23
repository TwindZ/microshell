/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: emlamoth <emlamoth@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/08/21 10:54:21 by emlamoth          #+#    #+#             */
/*   Updated: 2023/08/23 17:11:11 by emlamoth         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct s_io
{
	int in;
	int out;
	int next;
}			t_io;

void	ft_putstr_fd(int fd, char *str)
{
	while(*str)
		write(fd, str++, 1);
}

int	ft_pipe(t_io *io)
{
	int fd[2];

	if(pipe(fd) == -1)
	{
		ft_putstr_fd(STDERR_FILENO, "error: fatal\n");
		return(-1);
	}
	io->next = fd[0];
	io->out = fd[1];
	return (0);
	
}

void	micro_cd(char **argv)
{
	int i;

	i = 0;
	while(argv[i])
		i++;
	if(i != 2)
		ft_putstr_fd(STDERR_FILENO, "error: cd: bad arguments\n");
	else if(chdir(argv[1]) == -1)
	{
		ft_putstr_fd(STDERR_FILENO, "error: cd: ");
		ft_putstr_fd(STDERR_FILENO, argv[1]);
		ft_putstr_fd(STDERR_FILENO, "\n");
	}
}

void	setio(t_io *io)
{
	dup2(io->out, STDOUT_FILENO);
	dup2(io->in, STDIN_FILENO);
	if(io->in > 2)
		close(io->in);
	if(io->out > 2)
		close(io->out);
	if(io->next > 2)
		close(io->next);
}

void	exe(t_io *io, char **argv, char **envp)
{
	int id;
	id = fork();
	if(id == 0)
	{
		setio(io);
		if(execve(argv[0], argv, envp) == -1)
		{
			ft_putstr_fd(STDERR_FILENO, "error: cannot execute ");
			ft_putstr_fd(STDERR_FILENO, argv[0]);
			ft_putstr_fd(STDERR_FILENO, "\n");
		}
	}
	else
	{
		if(io->in > 2)
			close(io->in);
		if(io->out > 2)
			close(io->out);
		io->in = io->next;
	}
	waitpid(id, NULL, 0);
}

int main(int argc, char **argv, char **envp)
{
	int i;
	int start;
	t_io io;

	io.in = 0;
	io.out = 1;
	io.next = 0;
	start = 0;
	i = 1;
	if(argc < 2)
		return (1);
	while(argv[i])
	{
		start = i;
		if(io.out == 1)
			io.in = 0;
		while(argv[i] && strncmp(argv[i], "|", 2) && strncmp(argv[i], ";", 2))
			i++;
		if(argv[i])
		{
			if(strncmp(argv[i], "|", 2) == 0 && (argv[i + 1] 
				&& strncmp(argv[i + 1], "|", 2) && strncmp(argv[i + 1], ";", 2))
				&& (argv[i - 1] ))
			{
				if(ft_pipe(&io) == -1)
					return (-1);
			}	
			else
				io.out = 1;
			argv[i] = 0;
			i++;
		}
		if(argv[start])
		{
			if(strncmp(argv[start], "cd", 3) == 0)
				micro_cd(&argv[start]);
			else
				exe(&io, &argv[start], envp);
		}
	}
	return (0);
}
