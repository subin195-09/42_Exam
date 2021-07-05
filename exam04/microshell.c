#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define SIDE_OUT	0
#define SIDE_IN		1

#define	STDIN		0
#define STDOUT		1
#define STDERR		2

#define	TYPE_END	0
#define	TYPE_PIPE	1
#define	TYPE_BREAK	2


typedef	struct	s_list
{
	char			**args;
	int				len;
	int				type;
	int				pipes[2];
	struct s_list	*prev;
	struct s_list	*next;
}				t_list;

int		ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return (i);
}

int		show_error(char *str)
{
	if (str)
		write(STDERR, str, ft_strlen(str));
	return (1);
}

int		exit_fatal(void)
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
	return (1);
}

char	*ft_strdup(char *str)
{
	char	*ret;
	int		i;

	if (!(ret = (char *)malloc(sizeof(char) * (ft_strlen(str) + 1))))
		exit_fatal();
	i = -1;
	while (str[++i])
		ret[i] = str[i];
	ret[i] = 0;
	return (ret);
}

int		add_arg(t_list *cmd, char *av)
{
	char	**tmp;
	int		i;

	tmp = 0;
	if (!(tmp = (char **)malloc(sizeof(char *) * (cmd->len + 2))))
		exit_fatal();
	i = -1;
	while (++i < cmd->len)
		tmp[i] = cmd->args[i];
	tmp[i++] = ft_strdup(av);
	tmp[i] = 0;
	cmd->args = tmp;
	cmd->len++;
	return (0);
}

int		list_push(t_list **list, char *av)
{
	t_list	*new;

	if (!(new = (t_list *)malloc(sizeof(t_list))))
		return (exit_fatal());
	new->args = 0;
	new->len = 0;
	new->type = TYPE_END;
	new->prev = 0;
	new->next = 0;
	if (*list)
	{
		(*list)->next = new;
		new->prev = *list;
	}
	*list = new;
	return (add_arg(new, av));
}

int		parse_arg(t_list **cmds, char *av)
{
	if (!(*cmds))
	{
		if (strcmp(";", av) == 0)
			return (EXIT_SUCCESS);
		else
			return (list_push(cmds, av));
	}
	else
	{
		if (strcmp(";", av) == 0)
			(*cmds)->type = TYPE_BREAK;
		else if (strcmp("|", av) == 0)
			(*cmds)->type = TYPE_PIPE;
		else if ((*cmds)->type > TYPE_END)
			return (list_push(cmds, av));
		else
			return (add_arg(*cmds, av));
	}
	return (EXIT_SUCCESS);
}

void	list_rewind(t_list **list)
{
	while (*list && (*list)->prev)
		*list = (*list)->prev;
}

void	list_clear(t_list **cmds)
{
	t_list *tmp;

	list_rewind(cmds);
	while (*cmds)
	{
		tmp = (*cmds)->next;
		for (int i = 0; i < (*cmds)->len; i++)
			free((*cmds)->args[i++]);
		free((*cmds)->args);
		free(*cmds);
		*cmds = tmp;
	}
	*cmds = 0;
}

int		exec_cmd(t_list *cmd, char **env)
{
	pid_t	pid;
	int		ret = 1;
	int		status;
	int		pipe_open = 0;

	if (cmd->type == TYPE_PIPE || (cmd->prev && cmd->prev->type == TYPE_PIPE))
	{
		pipe_open = 1;
		if (pipe(cmd->pipes))
			return (exit_fatal());
	}
	pid = fork();
	if (pid < 0)
		return (exit_fatal());
	else if (pid == 0)
	{
		if (cmd->type == TYPE_PIPE && (dup2(cmd->pipes[SIDE_IN], STDOUT) < 0))
			return (exit_fatal());
		if (cmd->prev && cmd->prev->type == TYPE_PIPE &&\
			(dup2(cmd->prev->pipes[SIDE_OUT], STDIN) < 0))
			return (exit_fatal());
		if ((ret = execve(cmd->args[0], cmd->args, env)) < 0)
		{
			show_error("error: cannot execute ");
			show_error(cmd->args[0]);
			show_error("\n");
		}
		exit(ret);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (pipe_open)
		{
			close(cmd->pipes[SIDE_IN]);
			if (!cmd->next || cmd->type != TYPE_PIPE)
				close(cmd->pipes[SIDE_OUT]);
			if (cmd->prev && cmd->prev->type == TYPE_PIPE)
				close(cmd->prev->pipes[SIDE_OUT]);
		}
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
	}
	return (ret);
}

int		exec_cmds(t_list **cmds, char **env)
{
	int	ret;

	ret = 0;
	while (*cmds)
	{
		if (strcmp("cd", (*cmds)->args[0]) == 0)
		{
			if ((*cmds)->len < 2)
				ret = show_error("error: cd: bad arguments\n");
			else if (chdir((*cmds)->args[1]))
			{
				ret = show_error("error: cd: cannot change directory to ");
				show_error((*cmds)->args[1]);
				show_error("\n");
			}
		}
		else
			ret = exec_cmd(*cmds, env);
		if (!(*cmds)->next)
			break ;
		*cmds = (*cmds)->next;
	}
	return (ret);
}

int		main(int ac, char *av[], char *env[])
{
	t_list	*cmds;
	int		i;
	int		ret;

	ret = EXIT_SUCCESS;
	cmds = 0;
	i = 0;
	while (++i < ac)
		parse_arg(&cmds, av[i]);
	list_rewind(&cmds);
	if (cmds)
		ret = exec_cmds(&cmds, env);
	list_clear(&cmds);
	return (ret);
}