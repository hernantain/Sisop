#include "exec.h"

// sets the "key" argument with the key part of
// the "arg" argument and null-terminates it
static void get_environ_key(char* arg, char* key) {

	int i;
	for (i = 0; arg[i] != '='; i++)
		key[i] = arg[i];

	key[i] = END_STRING;
}

// sets the "value" argument with the value part of
// the "arg" argument and null-terminates it
static void get_environ_value(char* arg, char* value, int idx) {

	int i, j;
	for (i = (idx + 1), j = 0; i < strlen(arg); i++, j++)
		value[j] = arg[i];

	value[j] = END_STRING;
}

// sets the environment variables passed
// in the command line
//
// Hints:
// - use 'block_contains()' to
// 	get the index where the '=' is
// - 'get_environ_*()' can be useful here
static void set_environ_vars(char** eargv, int eargc) {

	if (eargc == 0)
		return;	

	int idx,status;
	for(int i = 0; i < eargc; i++){
		idx = block_contains( eargv[i],'=' );
		char* key = malloc( sizeof(char) * ( idx + 1 ));
		get_environ_key( eargv[i], key );
		char* value = split_line( eargv[i], '=' );
		
		status = setenv( key, value, 1 );
		
		free(key);
	}
}

// opens the file in which the stdin/stdout or
// stderr flow will be redirected, and returns
// the file descriptor
// 
// Find out what permissions it needs.
// Does it have to be closed after the execve(2) call?
//
// Hints:
// - if O_CREAT is used, add S_IWUSR and S_IRUSR
// 	to make it a readable normal file
static int open_redir_fd(char* file) {

	int fd = open(file, O_CREAT | O_RDWR, 0664 );
	if (fd < 0 ){
		return -1;
	}
	return fd;
}

int multipipe_func (int in, int out, struct command *cmd) {
  	
  	struct execcmd* e;
  	pid_t pid;

  	if ((pid = fork ()) == 0){
    	
    	if (in != 0) {
          	dup2(in, 0);
          	close(in);
		}

      	if (out != 1) {
          	dup2(out, 1);
          	close(out);
       	}

       	e = (struct execcmd*) cmd;

      	return execvp(e->argv[0], e->argv);
    }

  	return pid;
}



// executes a command - does not return
//
// Hint:
// - check how the 'cmd' structs are defined
// 	in types.h
void exec_cmd(struct cmd* cmd) {

	int fd_in,fd_out,fd_err;
	struct pipecmd* p;
	struct backcmd* b;
	struct execcmd* c,*r;
	switch (cmd->type) {

		case EXEC: {
			c = (struct execcmd*) cmd;
			
			set_environ_vars(c->eargv,c->eargc);
			if(c->argc == 0)
				break;

			if((execvp(c->argv[0], c->argv)) < 0)
				_exit(-1);

			break;
		}

		case BACK: {
			b = (struct backcmd* ) cmd;
			exec_cmd(b->c);
			_exit(-1);
			break;
		}

		case REDIR: {
			
			r = (struct execcmd*) cmd;
			
			if((fd_out = open_redir_fd(r->out_file)) > 0){
				dup2(fd_out, 1);
				close(fd_out);
			}

			if((fd_in = open_redir_fd(r->in_file)) > 0) {
				dup2(fd_in, 0);
				close(fd_in);
			}

			if((fd_err = open_redir_fd(r->err_file)) > 0) {
				dup2(fd_err, 2);
				close(fd_err);
			}
			
			if ((fd_out < 0) && (fd_in < 0) && (fd_err < 0))
				_exit(-1);

			execvp(r->argv[0],r->argv);			
			break;
		}
		
		case PIPE: {

			int pipefd[2];	

			p = (struct pipecmd*) cmd;

			if ( (pipe(pipefd)) < 0)
				_exit(-1);

			c = (struct execcmd*) p->leftcmd;
			r = (struct execcmd*) p->rightcmd;

			if (fork() == 0) {
				if((dup2(pipefd[WRITE], WRITE)) < 0 )
					_exit(-1);

				close(pipefd[1]);
				
				execvp(c->argv[0],c->argv);
    		}

			if (fork() == 0) {
				if ((dup2(pipefd[READ], READ)) < 0)
					_exit(-1);

				close(pipefd[0]);
				execvp(r->argv[0],r->argv);
    		}

			// free the memory allocated
			// for the pipe tree structure
			free_command(parsed_pipe);

			break;
		}

		case MULTIPIPE: {

			struct multipipecmd* mp = (struct multipipecmd*) cmd;

			int i;
			pid_t pid;
			int in = 0, fd[2];

			for(i = 0; i < mp->argc - 1; i++){
				pipe(fd);
				multipipe_func( in,fd[WRITE], mp->commands[i] );
				close(fd[1]);
				in = fd[0];
			}

			if(in != 0)
				dup2(in,0);

			c = (struct execmd*) mp->commands[i];
			execvp(c->argv[0],c->argv);
		}
	}
}