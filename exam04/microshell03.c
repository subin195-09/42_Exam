#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIDE_OUT	0
#define SIDE_IN		1

#define	STDIN		0
#define STDOUT		1
#define	STDERR		2

#define	TYPE_END	0
#define	TYPE_PIPE	1
#define	TYPE_BREAK	2

typedef	struct		s_list
{
	struct s_list	*prev;
	struct s_list	*next;
	char			**args;
	int				len;
	int				type;
	int				pipes[2];
}					t_list;

int		ft_strlen(char *str)
{
	int	i = 0;
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

void	exit_fatal(void)
{
	show_error("error: fatal\n");
	exit(1);
}

char	*ft_strdup(char *str)
{
	char	*ret;

	if (!(ret = malloc(sizeof(char) * (ft_strlen(str) + 1))))
		exit_fatal();
	int i = -1;
	while (str[++i])
		ret[i] = str[i];
	ret[i] = 0;
	return (ret);
}

void	add_arg(t_list *cmd, char *arg)
{
	char **tmp;

	if (!(tmp = malloc(sizeof(char *) * (cmd->len + 2))))
		exit_fatal();
	int i;
	for (i = 0; i < cmd->len; i++)
		tmp[i] = cmd->args[i];
	tmp[i++] = ft_strdup(arg);
	tmp[i] = 0;
	free(cmd->args);
	cmd->args = tmp;
	cmd->len++;
}

void	list_push(t_list **cmd, char *arg)
{
	t_list	*new;

	if (!(new = malloc(sizeof(t_list))))
		exit_fatal();
	new->args = 0;
	new->len = 0;
	new->type = TYPE_END;
	new->prev = 0;
	new->next = 0;
	if (*cmd)
	{
		(*cmd)->next = new;
		new->prev = *cmd;
	}
	add_arg(new, arg);
	*cmd = new;
}

void	list_rewind(t_list **cmd)
{
	while (*cmd && (*cmd)->prev)
		*cmd = (*cmd)->prev;
}

void	list_clear(t_list **cmd)
{
	t_list	*tmp;
	int		i;

	while (*cmd)
	{
		tmp = (*cmd)->next;
		for (i = 0; i < (*cmd)->len; i++)
			free((*cmd)->args[i]);
		free((*cmd)->args);
		free(*cmd);
		*cmd = tmp;
	}
	*cmd = 0;
}

void	parse_arg(t_list **cmd, char *arg)
{
	if	(!(*cmd))
	{
		if (strcmp(";", arg) != 0)
			list_push(cmd, arg);
	}
	else
	{
		if (strcmp(";", arg) == 0)
			(*cmd)->type = TYPE_BREAK;
		else if (strcmp("|", arg) == 0)
			(*cmd)->type = TYPE_PIPE;
		else if ((*cmd)->type > TYPE_END)
			list_push(cmd, arg);
		else
			add_arg(*cmd, arg);
	}
}

int		exec_cmd(t_list	**cmd, char **env)
{
	int		pid;
	int		pipe_open;
	int		ret = 1;
	int		status;

	if ((*cmd)->type == TYPE_PIPE || ((*cmd)->prev && (*cmd)->prev->type == TYPE_PIPE))
	{
		if (pipe((*cmd)->pipes))
			exit_fatal();
		pipe_open = 1;
	}
	pid = fork();
	if (pid < 0)
		exit_fatal();
	else if (pid == 0)
	{
		if ((*cmd)->type == TYPE_PIPE && dup2((*cmd)->pipes[SIDE_IN], STDOUT) < 0)
			exit_fatal();
		if ((*cmd)->prev && (*cmd)->prev->type == TYPE_PIPE && dup2((*cmd)->prev->pipes[SIDE_OUT], STDIN) < 0)
			exit_fatal();
		if ((ret = execve((*cmd)->args[0], (*cmd)->args, env)) < 0)
		{
			show_error("error: cannot execute ");
			show_error((*cmd)->args[0]);
			show_error("\n");
		}
		exit(ret);
	}
	else
	{
		waitpid(pid, &status, 0);
		if (pipe_open)
		{
			close((*cmd)->pipes[SIDE_IN]);
			if (!(*cmd)->next || (*cmd)->type != TYPE_PIPE)
				close((*cmd)->pipes[SIDE_OUT]);
			if ((*cmd)->prev && (*cmd)->prev->type == TYPE_PIPE)
				close((*cmd)->prev->pipes[SIDE_OUT]);
		}
		if (WIFEXITED(status))
			ret = WEXITSTATUS(status);
	}
	return (ret);
}

int		exec_cmds(t_list **cmd, char **env)
{
	int	ret = 0;

	while (*cmd)
	{
		if (strcmp("cd", (*cmd)->args[0]) == 0)
		{
			if ((*cmd)->len < 2)
				ret = show_error("error: cd: bad arguments\n");
			else if (chdir((*cmd)->args[1]))
			{
				ret = show_error("error: cd: cannot change directory ");
				show_error((*cmd)->args[1]);
				show_error("\n");
			}
		}
		else
			ret = exec_cmd(cmd, env);
		if (!(*cmd)->next)
			break ;
		(*cmd) = (*cmd)->next;
	}
	return ret;
}

int		main(int ac, char *av[], char *env[])
{
	int	ret = 0;
	t_list	*cmd;
	int	i;

	for (i = 1; i < ac; i++)
		parse_arg(&cmd, av[i]);
	list_rewind(&cmd);
	if (cmd)
		ret = exec_cmds(&cmd, env);
	list_rewind(&cmd);
	list_clear(&cmd);
	//system("leaks a.out");
	return (ret);
}