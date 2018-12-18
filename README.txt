Javier Mayol

Mini Shell program.

Assignment description:
Simple shell program that supports basic I/O redirection line the unix shell.
$ command Run command, with stdin and stdout connected to their usual files. 
  When command finishes, the parent should output pid:%d status:%d\n 
  (with the proper relevant values inserted into the format string), 
  and then print the prompt again and wait for more input.
$ command > filename Run as in #1, but redirects the output of the command  
  to filename. The existing contents of filename are overwritten.
$ command < filename Run as in #1, but redirect the input of the command from filename.
$ command1 ; command2 Run command1 as in #1, wait for it to finish, and then run command 

command examples are:
 /bin/ls 
 /bin/ls -l
 /bin/ps
 /bin/ps -u

  An implementataion of a small shell program
  that prints a prompt and asks the user for 
  a command. Supports redirection of I/O unix.

  Performs as follows:

  1. Allocation of memory for needed char * and char**
	arrays.
  2. Infinite loop prints prompt CS361> and waits
     for user to type a command.
  3. 
  parse_str()
 	Parses the string inputed by the user.
	Takes a char *buffer and a char **array.
  4.
  flags : if user inputs any of the operators
          for redirection ";", " >", " <", the flag
          stores values 1, 2, or 3 correspondingly.
	  0 if no redirection is requested.	
  5.
  exeq()
	takes argument char **argsarray
      	calls fork, after a child is reaped
  	executes execv(argsarray[0], argsarray)
  6.	
  r_parsing()
	Search for tokens ";", ">", "<" and change elements to null.
  7.
  split()
	Takes the array populated with the input by user
	as aparameter. Returns array with the second command
	for redirection.
  8.
  r_exeq()
	parameters are input array and splitted array. Executes
	redirection command ; command.
  9
  r_file()
	parameters are input array and splitted array. Executes
	redirection command > command or command < command.
  10.
  shell()
	Driver. initiallizes arrays, declares an endless loop and frees
	memmory allocated. 
   The only builtin command for this shell is "exit" which exits the program. 
