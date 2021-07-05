#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
	char			**arg;
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

int		exit_fatal(void)
{
	write(STDERR, "error: fatal\n", ft_strlen("error: fatal\n"));
	exit(1);
	return (1);
}

int		add_arg(t_list *cmd, char *av)
{
	char	**tmp;
	int		i;

	tmp = 0;
	if (!(tmp = (char **)malloc(sizeof(char *) * (cmd->len + 2)))
		return (exit_fatal());
	i = -1;
	while (++i < cmd->len)
		tmp[i] = cmd->av[i];
	tmp[++i] = ft_strdup(av);
	tmp[i] = 0;
	cmd->arg = tmp;
	cmd->len++;
	return (0);
}

int		list_push(t_list **list, char *av)
{
	t_list	*new;

	if (!(new = (t_list *)malloc(sizeof(t_list))))
		return (exit_fatal());
	new->arg = 0;
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
	return (add_arg(new, arg));
}

int		parse_arg(t_list **cmds, char *av)
{
	if (!*cmds)
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
		else if ((*cmds)->type > TYPE_END))
			return (list_push(cmds, av));
		else
			return (add_arg(*cmds, arg));
	}
	return (EXIT_SUCCESS);
}

int		main(int ac, char *av[], char *env)
{
	t_list	*cmds;
	int		i;
	int		ret;

	ret = EXIT_SUCCESS;
	cmds = 0;
	i = 0;
	while (++i < ac)
		parse_arg(&cmds, av[i]);
	if (cmds)
		ret = exec_cmds(&cmds, env);
	list_clear(&cmds);
	return (ret);
}