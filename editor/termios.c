//Roll.No: 20172063
//Roll.No: Goutham Bhatta

#include "editor.h"

int main(int argc, char *argv[]) {
  	
  	startRawMode();
  	clscr();
  	char c;
  	
  	char *fname, *file_char;
	file_char = (char*)malloc(sizeof(char));;
	int fd;

    fname = argv[1];

    fd = open(fname, O_RDONLY);

	if(fd != -1) {
		int row=0, col=0;

		while(read(fd, file_char, 1)) {

			 if(*file_char == '\r' || *file_char == '\n') {
			 	//sleep(5);
			 	//write(STDOUT_FILENO, file_char, 1);
			 	//write(STDOUT_FILENO, "\r", 1);
			 	//write(STDOUT_FILENO, "\r\n", 2);
			 	//fflush(NULL);
			 	//if(cnt==1) write(STDOUT_FILENO, "\x1b[2;0H", 6);
			 	col=0;
			 	if(*file_char == '\n') {
			 		read(fd, file_char, 1);
			 		insert(col, ++row, *file_char);	
			 		//write(STDOUT_FILENO, "\r\n", 2);
			 	} else {
			 		read(fd, file_char, 1);
			 		read(fd, file_char, 1);	
			 		insert(col, ++row, *file_char);
			 		//write(STDOUT_FILENO, "\r\n", 2);
			 	}
			 	
				//write(STDOUT_FILENO, file_char, 1);
				//row++;
				
				//sleep(2);
			 	//insert(col, ++row, *file_char);
				//insert(++col, row, '\n');
				
			} else {
				//col++;
				insert(col++, row, *file_char);		
				//sleep(1);
				//write(STDOUT_FILENO, file_char, 1);
			}
		// insert(row++, col, *file_char);
		//write(STDOUT_FILENO, file_char, 1);
		
	}
	//sleep(3);
	disp();
    write(STDOUT_FILENO, "\x1b[0;0H", 6);	
    
    free(file_char);
    close(fd);
	}

	

  	while(1) {

  		char *cmd_buff = (char*)malloc(1);
  		while(read(STDOUT_FILENO, &c, 1)!=1);

	  	switch((int)c) {

	  		case 'i':
	  			// get window size
	  			// display insert mode at the end of the file
	  			dispMode("---INSERT MODE---", 20);
	  			//printf("I am Here..\r\n");
	  			editMode();
	  			cmdMode(0);
	  			break;

	  		case ':':
	  			//printf("I am Here..\r\n");
	  			cmdMode(0);
	  			write(STDOUT_FILENO, ":", 1);
	  			//while(read(STDOUT_FILENO, &c, 1)!=1);
	  			if(getCmdKeyPress(cmd_buff)) {

	  				if(*cmd_buff == '!') {
	  					//printf("Terminal\r\n");
	  					terminal(cmd_buff);
	  				} else if(strcmp(cmd_buff, "q!")==0) {
	  					cmdMode(0);
	  					write(STDOUT_FILENO,"***Force Exit***", 16);
	  					exit(0);
	  				} else if(strcmp(cmd_buff, "wq")==0) {
              			file_write(fname);
              			cmdMode(0);
	  					write(STDOUT_FILENO,"***File Saved***", 16);
	  					exit(0);
	  				} else if(strcmp(cmd_buff, "q")==0) {
	  					cmdMode(0);
	  					write(STDOUT_FILENO,"***File Not Saved, Use q! to Override***", 39);

	  				}

	  			}
	  			break;

	  		default:
	  			cmdMode(1);
	  	}

  	}

  	clscr();
	return 0;
}

// Notes:
// a predefined structure which stores the terminal attributes
// struct termios term;

// a predefined function which reads the current terminal attributes in which the program is being executed.
// tcgetattr(STDIN_FILENO, &term);

// c_lflag is used to set the local flag of the terminal
// it stores attributes like echo, icanon

// generally attributes of c_oflag start with 'O',
// and c_iflag starts with I (ICANON is an exception though).

//c_iflag, etc are of int data type

// eneter key is returning 13 which ascii value of carraige return
// i.e \r, the line feed \n asci value is 10
