/*
 * file:        part-2.c
 * description: Part 2, CS5600 Project 2, Fall 2023
 */

/* NO OTHER INCLUDE FILES */
#include "elf64.h"
#include "sysdefs.h"

extern void *vector[];
char** g_argv;
int g_argc;

/* ---------- */

/* write these functions
*/

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
}/* https://man7.org/linux/man-pages/man2/write.2.html */


void exit(int err){
	/* add your code here*/

	syscall(__NR_exit, err);
} /* https://man7.org/linux/man-pages/man2/exit.2.html */

int open(char *path, int flags){
	return syscall(__NR_open, path, flags);
}

int close(int fd){
	return syscall(__NR_close, fd);
}

/* Used to change the location of the read/write pointer of a file descipter.
Argument `flag` defines where to add the offset to the file descripter. */
int lseek(int fd, int offset, int flag) {
	return syscall(__NR_lseek, fd, offset, flag);
} /* https://man7.org/linux/man-pages/man2/lseek.2.html */


void *mmap(void *addr, int len, int prot, int flags, int fd, int offset){
	return (void*)syscall(__NR_mmap, addr, len, prot, flags, fd, offset);
};


int munmap(void *addr, int len){
	if (len < 0) {
		return -1;
	}
	return syscall(__NR_munmap, addr, len);
};

/* ---------- */

/* Write the three 'system call' functions - do_readline, do_print, do_getarg
 * Adjust the functions readline and print-and-clean functions written in part 1, to obtain
 * the 'system call' functions do_readline and do_print
 * hints:
 *  - read() or write() one byte at a time. It's OK to be slow.
 *  - stdin is file desc. 0, stdout is file descriptor 1
 *  - use global variables for getarg
 */

/* your code here */
void do_readline(char *buf, int len) {
	// initialize position to track the position of the buffer
	int position = 0;
	// temporarily hold the character read from the input
	char temp = buf[position];

	while (position < len - 1) {
		// call read() function to read one byte from input each time
		// stdin is file descriptor 0
 		int read_return_val = read(0, &temp, 1);
 		// error occurred during reading
  		if (read_return_val < 0) {
    			exit(1);
    		// stop reading into buf[] when reach the end of input or end of line
		} else if (read_return_val == 0 || temp == '\n') {
    			break;
    		// read 1 byte each time into buf[]
 		} else {
			position++;
			temp = buf[position];
  		}
	}
	// terminate the string
	buf[position] = '\0';
}


void do_print(char *buf) {
	if (buf == NULL) {
		exit(1);
	}

	// Initialize position as the starter point in buffer.
	int position = 0;

	// If `temp` points to '\0', means reaching the end of print line, stop.
	while (buf[position] != '\0') {
		// Call write() to print 1 byte at a time.
		// Pass file descriptor 1 (used for stdout) to write() function.
		int written_return_val = write(1, &buf[position], 1);
		// If write() function returns -1 or (< 0), means an error occured in
		// writing a character to stdout, exit to terminate the process.
		if (written_return_val < 0) {
			// exit(1) means exit with error.
			exit(1);
		} else {
			// Otherwise, write sucessfully.
			// Increment the position by 1.
			position++;
		}
	}
}


char *do_getarg(int i) {
	if (i + 1 > g_argc) return NULL;
	return g_argv[i];
};

/* ---------- */



/* simple function to split a line:
 *   char buffer[200];
 *   <read line into 'buffer'>
 *   char *argv[10];
 *   int argc = split(argv, 10, buffer);
 *   ... pointers to words are in argv[0], ... argv[argc-1]
 */
int split(char **argv, int max_argc, char *line)
{
	int i = 0;
	char *p = line;

	while (i < max_argc) {
		while (*p != 0 && (*p == ' ' || *p == '\t' || *p == '\n'))
			*p++ = 0;
		if (*p == 0)
			return i;
		argv[i++] = p;
		while (*p != 0 && *p != ' ' && *p != '\t' && *p != '\n')
			p++;
	}
	return i;
}

/* ---------- */

/* This is where you write the details of the function exec(char* filename) called by main()
* Follow instructions listed in project description.
* the guts of part 2
*   read the ELF header
*   for each segment, if b_type == PT_LOAD:
*     create mmap region
*     read from file into region
*   function call to hdr.e_entry
*   munmap each mmap'ed region so we don't crash the 2nd time
*
*   don't forget to define offset, and add it to virtual addresses read from ELF file
*
*               your code here
*/
void exec(char* filename) {
	// Define file descripter.
	int fd;

	// Open the file specified by filename and store its file descriptor to fd.
	// If we cannot open this file, exit with failure code 1.
	if ((fd = open(filename, O_RDONLY)) < 0) {
		do_print("file open failed.\n");
		exit(1);
	}

	// Read the ELF main header. This struct is defined in the elf64.h file.
	struct elf64_ehdr hdr;
	read(fd, &hdr, sizeof(hdr));
	// do_print((char *)hdr.e_entry);

	// // Read the program header.
	int n = hdr.e_phnum;
	struct elf64_phdr phdrs[n];
	lseek(fd, hdr.e_phoff, SEEK_SET);
	read(fd, phdrs, sizeof(phdrs));

	// Define offset value. This is equal to the base register pointer. EBP.
	unsigned int offset = 0x80000000;

	// Define two lists to store the allocated memory addresses and theri lengths.
	void **allocated_addr[n];
	int allocated_len[n];

	// Read loadable program segment and load it to memory
	for(int i = 0; i < hdr.e_phnum; i++) {
		if (phdrs[i].p_type == PT_LOAD) {
			int len = ROUND_UP(phdrs[i].p_memsz, 4096);
			// Use mmap(void *addr, int len, int prot, int flags, int fd, int offset)
			// to allocate memory for each segment
			void *buf = mmap(phdrs[i].p_vaddr + offset, len,
				PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
			if (buf == MAP_FAILED) {
				do_print("mmap failed\n");
				exit(1);
			}
			allocated_addr[i] = buf;
			allocated_len[i] = len;
			// phdrs[i].p_offset is the offset of the segment in the executable.
			// read segment.  hdr.e_phoff + phdrs[i].p_offset
			lseek(fd, (int)phdrs[i].p_offset, SEEK_SET);
			read(fd, buf, (int)phdrs[i].p_filesz);
		}
	}

	// function call to hdr.e_entry
	void (*f)();
	f = hdr.e_entry + offset;
	f();

	// munmap each mmap'ed region so we don't crash the 2nd time
	for (int i = 0; i < hdr.e_phnum; i++) {
		munmap(allocated_addr[i], allocated_len[i]);
	}

	close(fd);
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

/* ---------- */
void main(void)
{   // The vector array is defined as a global array. It plays the role of a system call vector table
	// (similar to the interrupt vector table seen in class). Each entry in this array/table holds the address
	// of the corresponding system function. Check out call-vector.S and Makefile to see how the vector table is built.

	vector[0] = do_readline;
	vector[1] = do_print;
	vector[2] = do_getarg;

	/* YOUR CODE HERE AS DESCRIBED IN THE FILE DESCRIPTION*/
	/* When the user enters an executable_file, the main function should call exec(executable_file) */

	// // Test do_print
	// char msg[] = "test\n";
	// do_print(msg);

	char buffer[200];
	do_readline(buffer, 200);
	do_print(buffer);

	// while(1) {
		// print out user prompt
		// char input_intro[] = "> ";
		// do_print(input_intro);
		// buffer to read line
		// char buffer[] = "";
		// do_readline(buffer, 200);
		// local argv to store split reasult
		//char *local_argv[10];
		//g_argc = split(local_argv, 10, buffer);
		// assign local to global
		//g_argv = local_argv;
		// compare first argument to quit
		//char* filename = do_getarg(0);
		//int flag = compare(filename, "quit");
		// if (flag == 0) {
		// 	exit(2);
		// } else {
		// 	// exec(filename);
		// 	do_print(filename);
		// }
	// }

	// exec("hello");
	exit(0);
}
