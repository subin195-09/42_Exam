#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define SIDE_OUT	0
#define SIDE_IN		1

#define STDIN		0
#define STDOUT		1
#define	STDERR		2

#define TYPE_END	0
#define TYPE_PIPE	1
#define TYPE_BREAK	2

typedef	struct		s_list
{
	struct s_list	*prev;
	struct s_list	*next;
	char			**args;
	int				type;
	int				len;
	int				pipes[2];
}					t_list;

//1
int		ft_strlen(char *s)
{
	int i = 0;
	while (s[i])
		i++;
	return (i);
}

//2
int		show_error(char	*s)
{
	if (s)
		write(STDERR, s, ft_strlen(s));
	return (1);
}

//3
void	exit_fatal(void)
{
	show_error("error: fatal\n");
	exit(1);
}

//4
char	*ft_strdup(char *s)
{
	char *rt;

	if (!(rt = malloc(sizeof(char) * (ft_strlen(s) + 1))))
		exit_fatal();
	int i = -1;
	while (s[++i])
		rt[i] = s[i];
	rt[i] = 0;
	return (rt);
}

//5
void	add_arg(t_list *cmd, char *arg)
{
	char **tmp;

	if (!(tmp = malloc(sizeof(char *) * (cmd->len + 2))))
		exit_fatal();
	int i = 0;
	for (i = 0; i < cmd->len; i++)
		tmp[i] = cmd->args[i];
	tmp[i++] = ft_strdup(arg);
	tmp[i] = 0;
	if (cmd->len > 0)
		free(cmd->args);
	cmd->args = tmp;
	cmd->len++;
}

//6
void	list_push(t_list **cmd, char *arg)
{
	t_list	*new;

	if (!(new = malloc(sizeof(t_list))))
		exit_fatal();
	new->prev = 0;
	new->next = 0;
	new->args = 0;
	new->type = TYPE_END;
	new->len = 0;
	if (*cmd)
	{
		(*cmd)->next = new;
		new->prev = *cmd;
	}
	add_arg(new, arg);
	*cmd = new;
}

//7
void	parse_arg(t_list **cmd, char *arg)
{
	if (!(*cmd))
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

//8
void	list_rewind(t_list **cmd)
{
	while (*cmd && (*cmd)->prev)
		*cmd = (*cmd)->prev;
}

//9
void	list_clear(t_list **cmd)
{
	t_list	*tmp;

	list_rewind(cmd);
	while (*cmd)
	{
		tmp = (*cmd)->next;
		for (int i = 0; i < (*cmd)->len; i++)
			free((*cmd)->args[i]);
		free((*cmd)->args);
		free(*cmd);
		*cmd = tmp;
	}
	*cmd = 0;
}

//10
int		exec_cmd(t_list *cmd, char **env)
{
	pid_t	pid;
	int		pipe_open = 0;
	int		status;
	int		ret = 1;

	if (cmd->type == TYPE_PIPE || (cmd->prev && cmd->prev->type == TYPE_PIPE))
	{
		pipe_open = 1;
		if (pipe(cmd->pipes))
			exit_fatal();
	}
	pid = fork();
	if (pid < 0)
		exit_fatal();
	else if (pid == 0)
	{
		if (cmd->type == TYPE_PIPE && dup2(cmd->pipes[SIDE_IN], STDOUT) < 0)
			exit_fatal();
		if (cmd->prev && cmd->prev->type == TYPE_PIPE && dup2(cmd->prev->pipes[SIDE_OUT], STDIN) < 0)
			exit_fatal();
		if ((ret = execve(cmd->args[0], cmd->args, env)) < 0)
		{
			ret = show_error("error: cannot execute ");
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

//11
int		exec_cmds(t_list **cmd, char **env)
{
	int ret = 0;
	while (*cmd)
	{
		if (strcmp("cd", (*cmd)->args[0]) == 0)
		{
			if ((*cmd)->len < 2)
				ret = show_error("error: cd: bad arguments\n");
			else if (chdir((*cmd)->args[1]))
			{
				ret = show_error("error: cd: cannot change directory to ");
				show_error((*cmd)->args[1]);
				show_error("\n");
			}
		}
		else
			ret = exec_cmd(*cmd, env);
		if (!(*cmd)->next)
			break ;
		*cmd = (*cmd)->next;
	}
	return ret;
}

//12
int		main(int ac, char *av[], char *env[])
{
	int		ret = 0;
	t_list	*cmd;

	for (int i = 1; i < ac; i++)
		parse_arg(&cmd, av[i]);
	list_rewind(&cmd);
	if (cmd)
		exec_cmds(&cmd, env);
	list_rewind(&cmd);
	list_clear(&cmd);
//	system("leaks a.out");
	return (ret);
}
		
