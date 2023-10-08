/*
 * file:        part-1.c
 * description: Part 1, CS5600 Project 2, Fall 2023
 */

/* THE ONLY INCLUDE FILE */
#include "sysdefs.h"


/* write these functions */
int read(int fd, void *ptr, int len);
int write(int fd, void *ptr, int len);
void exit(int err);
 

/*The read function is given to you in the starter code, by analogy write the
  write() function and the exit() function. 
  __NR_read is defined in file sysdefs.h, it is the system call number 
  for the system function that performs reading. 
    A link is provided after each function, to read about this system call function
  in the Linux manual page */

int read(int fd, void *ptr, int len) {
	if (len < 0) {
		return -1;
	}

	return syscall(__NR_read, fd, ptr, len);
} /*https://man7.org/linux/man-pages/man2/read.2.html */


/* __NR_write is defined in file sysdefs.h, it is the system call number
		for the system function that performs writing.*/
int write(int fd, void *ptr, int len){
	/* add your code here*/
	if (len < 0) {
		return -1;
	}

	return syscall(__NR_write, fd, ptr, len);
} /* https://man7.org/linux/man-pages/man2/write.2.html */



void exit(int err){
	/* add your code here*/

	syscall(__NR_exit, err);
} /* https://man7.org/linux/man-pages/man2/exit.2.html */



/* Factor, factor! Don't put all your code in main()!
   Write new functions using the functions you just wrote
*/


/* read one line from stdin (file descriptor 0) into a buffer pointed to by ptr. : */
void readline(int fd, void *ptr, int max_len) {
	/*reads a line from the keyboard by repeatedly (for each byte) calling function
	read() that you just wrote. 
	You will call read() to read 1 byte at a time, and sticking it in a buffer (pointed to by ptr); 
	if it is ‘\n’ then add a '\0' on the end and you’re done)
    You can assign a (void *) pointer to a (char *) by using proper casting if needed.*/

	/* add your code here*/
	char *buffer = (char *)ptr; // cast void pointer to character pointer buffer
    	char temp; // temporarily hold the character read from the input
    	int position = 0; // track the number of characters have been read

    	while (position < max_len - 1) {
        	int n = read(fd, &temp, 1); // read one character from fd each time
        	if (n < 0) {
            		exit(1); // error occurred during reading
        	} else if (n == 0 || temp == '\n') {
            		break; // end of input or end of line
        	} else {
            		buffer[position] = temp;
            		position++;
        	}
    	}	

    	buffer[position] = '\0'; // terminate the string
}

	
/* print a string to stdout (file descriptor 1) */
void print_and_clean(int fd, void *ptr, int max_len) {
	/* This function prints a string pointed to by ptr to stdout by repeatedly calling
	function write() you wrote above.
	You will call write to print 1 byte at a time.
	After you print a character, clean the corresponding buffer location by replacing
	the character printed by 0 */

	/* add your code here*/
	// Cast the void* pointer to char* pointer, to increment it by 1 byte later.
	char *temp = (void *)ptr;

	for(int i = 0; i < max_len; i++) {
		// Call write() to print 1 byte at a time.
		int written = write(fd, temp, 1);
		// If write() function return -1, means error occurs when writing a char
		// to stdout, exit to terminate the process. exit(1) means exit with error.
		if (written == -1) {
			exit(1);
		}
		// Clean the buffer location. Replacing the printed character by "\0".
		*temp = "\0";
		// Increment the char* pointer by 1 byte.
		temp++;
	}
}


int compare(char* x, char* y) {
	int flag = 0;
	while (*x != '\0' || *y != '\0') {
		if (*x == *y) {
			x++;
			y++;
		} 
		else if ((*x == '\0' && *y != '\0')
			|| (*x != '\0' && *y == '\0')
			|| *x != *y) {
			flag = 1;
			break;
		}
	}
	return flag;
}



void main(void)
{
	/* reads a line of input from standard input (file descriptor 0)
	   if the line starts with ‘q’, ‘u’, ‘i’, ‘t’ then exit
	   otherwise print the line to standard output (file descriptor 1) and loop.
	   You can assume that input lines are never more than 200 bytes long. 
	   Remember that you can’t return from the main function – you have to call exit.
	   Remember that you can’t use any functions other than the ones that you write 
	   yourself. That means no printf – use GDB for your debugging (and no strcmp for
	   string comparisons, or malloc for allocating memory).   */

	/* add your code here */
	/* 
	char *welcome_msg = "Hello, type lines of input, or 'quit'\n";
	int max_len = 200;
	print_and_clean(1, welcome_msg, max_len);
	
	while (true) {
		char* input = "";
		readline(0, input, max_len);
		int flag = compare(input, "quit");
		if (flag == 0) {
			exit(2);
		} else {
			print_and_clean(1, input, max_len);
		}
	}
	*/
	
}
