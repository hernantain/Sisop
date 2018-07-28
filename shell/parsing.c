#include "parsing.h"

// parses an argument of the command stream input
static char* get_token(char* buf, int idx) {

	char* tok;
	int i;
	
	tok = (char*)calloc(ARGSIZE, sizeof(char));
	i = 0;

	while (buf[idx] != SPACE && buf[idx] != END_STRING) {
		tok[i] = buf[idx];
		i++; idx++;
	}

	return tok;
}

// parses and changes stdin/out/err if needed
static bool parse_redir_flow(struct execcmd* c, char* arg) {

	int inIdx, outIdx;

	// flow redirection for output
	if ((outIdx = block_contains(arg, '>')) >= 0) {
		switch (outIdx) {
			// stdout redir
			case 0: {
				strcpy(c->out_file, arg + 1);
				break;
			}
			// stderr redir
			case 1: {
				strcpy(c->err_file, &arg[outIdx + 1]);
				break;
			}
		}
		
		free(arg);
		c->type = REDIR;
		
		return true;
	}
	
	// flow redirection for input
	if ((inIdx = block_contains(arg, '<')) >= 0) {
		// stdin redir
		strcpy(c->in_file, arg + 1);
		
		c->type = REDIR;
		free(arg);
		
		return true;
	}
	
	return false;
}

// parses and sets a pair KEY=VALUE
// environment variable
static bool parse_environ_var(struct execcmd* c, char* arg) {

	// sets environment variables apart from the
	// ones defined in the global variable "environ"
	if (block_contains(arg, '=') > 0) {

		// checks if the KEY part of the pair
		// does not contain a '-' char which means
		// that it is not a environ var, but also
		// an argument of the program to be executed
		// (For example:
		// 	./prog -arg=value
		// 	./prog --arg=value
		// )
		if (block_contains(arg, '-') < 0) {
			c->eargv[c->eargc++] = arg;
			return true;
		}
	}
	
	return false;
}

// this function will be called for every token, and it should
// expand environment variables. In other words, if the token
// happens to start with '$', the correct substitution with the
// environment value should be performed. Otherwise the same
// token is returned.
//
// Hints:
// - check if the first byte of the argument
// 	contains the '$'
// - expand it and copy the value
// 	 to 'arg' 
static char* expand_environ_var(char* arg) {
	if (arg[0] == '$')
		return strcpy(arg,getenv(arg+1));

	return arg;
}

// parses one single command having into account:
// - the arguments passed to the program
// - stdin/stdout/stderr flow changes
// - environment variables (expand and set)
static struct cmd* parse_exec(char* buf_cmd) {

	struct execcmd* c;
	char* tok;
	int idx = 0, argc = 0, eargc = 0;
	
	c = (struct execcmd*)exec_cmd_create(buf_cmd);
	c->eargc = eargc;

	while (buf_cmd[idx] != END_STRING) {
	
		tok = get_token(buf_cmd, idx);
		idx = idx + strlen(tok);
		
		if (buf_cmd[idx] != END_STRING)
			idx++;
		
		tok = expand_environ_var(tok);
		
		if (parse_redir_flow(c, tok))
			continue;
		
		if (parse_environ_var(c, tok))
			continue;

		c->argv[argc++] = tok;
	}
	
	//printf("eargc = %s\n", c->eargc);
	c->argv[argc] = (char*)NULL;
	c->argc = argc;

	return (struct cmd*)c;
}

// parses a command knowing that it contains
// the '&' char
static struct cmd* parse_back(char* buf_cmd) {

	int i = 0;
	struct cmd* e;

	while (buf_cmd[i] != '&')
		i++;
	
	buf_cmd[i] = END_STRING;
	
	e = parse_exec(buf_cmd);

	return back_cmd_create(e);
}

// parses a command and checks if it contains
// the '&' (background process) character
static struct cmd* parse_cmd(char* buf_cmd) {

	if (strlen(buf_cmd) == 0)
		return NULL;
		
	int idx;

	// checks if the background symbol is after
	// a redir symbol, in which case
	// it does not have to run in in the 'back'
	if ((idx = block_contains(buf_cmd, '&')) >= 0 &&
			buf_cmd[idx - 1] != '>')
		return parse_back(buf_cmd);
		
	return parse_exec(buf_cmd);
}


struct cmd* aux_func(char* buf, char* right) {

	struct cmd * pipeexecs[20];
	char* pipeargs[20];	
	char* a_right;
	int i = 0, j, idx;
	pipeargs[i] = buf;
	
	while((idx = block_contains(right, '|' )) > 0){
		i++;
		a_right = split_line(right, '|' );
		pipeargs[i] = right;
		right = a_right;
	}

	i++;
	pipeargs[i] = right;

	for( j = 0; j <= i; j++ ) {
		pipeexecs[j] = parse_cmd(pipeargs[j]);		
	}
	pipeexecs[j++] = NULL;

	return multipipe_cmd_create( pipeexecs, j-1 );
}

// parses the command line
// looking for the pipe character '|'
struct cmd* parse_line(char* buf) {
	
	struct cmd *r, *l;
	char* right = split_line(buf, '|');
	int idx;
	
	if((idx = block_contains(right, '|')) > 0)
		return aux_func(buf,right);

	l = parse_cmd(buf);
	r = parse_cmd(right);
	
	return pipe_cmd_create(l, r);
}

