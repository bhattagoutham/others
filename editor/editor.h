//Roll.No: 20172063
//Roll.No: Goutham Bhatta

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <string.h>
#include "ll.h"

struct termios original, edit;
int cx=0, cy=0;


void clscr() {
  write(STDOUT_FILENO, "\x1b[2J", 4);
  write(STDOUT_FILENO, "\x1b[H", 3);
}

void endRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
}


void startRawMode() {
  
  tcgetattr(STDIN_FILENO, &original);  
  atexit(endRawMode);

  edit = original;
  edit.c_iflag &= ~(ICRNL | IXON);
  edit.c_oflag &= ~(OPOST);
  edit.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  edit.c_cc[VMIN] = 0;
  edit.c_cc[VTIME] = 1;

  tcsetattr(STDIN_FILENO, TCSAFLUSH, &edit);
}




void moveCursor(char *esc_seq) {
	char c = esc_seq[1];
	switch (c) {

		case 'A':
			//printf("Up\r\n");
			write(STDOUT_FILENO, "\x1b[1A", 4);
			cy--;
			break;
		case 'B':
			//printf("Down\r\n");
			write(STDOUT_FILENO, "\x1b[1B", 4);
			cy++;
			break;
		case 'C':
			//printf("Right\r\n");
			write(STDOUT_FILENO, "\x1b[1C", 4);
			cx++;
			break;
		case 'D':
			//printf("Left\r\n");
			write(STDOUT_FILENO, "\x1b[1D", 4);
			 cx--;
			break;

	}

}






int getKeyPress() {

	char c='\0';
	int cnt=0;
  char esc_seq[2];
  //struct refresh *ref;

	if (read(STDIN_FILENO, &c, 1) == 1) {

    	if (iscntrl(c)) {

      		switch(c) {
      			// when backspace is pressed (ascii-127)
      			case 127:
      		
      				
              //cx--;
              
              //write(STDOUT_FILENO, "\x1b[1D", 4);
              insert(cx, cy, 127);

              char buff[6];
              sprintf(buff, "\x1b[%d;%dH", cy, cx);
              write(STDOUT_FILENO, buff, 6);
              if(cx > 0) cx--;
              
      				break;

      			// when enter key is pressed (ascii-13)
      			case 13:
      				cx=0;
      				cy++;
      				printf("\r\n");
              insert(cx, cy, 127);
      				break;
      			
      			// when an escape charecter or sequence is pressed 
      			case 27:

      				while(read(STDIN_FILENO, &c, 1)) {
      					esc_seq[cnt++] = c;
      				}

      				if(cnt == 2) {
      					moveCursor(esc_seq);
      				} else {
   
      					return -1;
      				}
      				break;
      		}

    	} else {
    		//printf("%c", c);
      		write(STDOUT_FILENO, &c, 1);
          insert(cx, cy, c);
          //write(STDOUT_FILENO, buff_refresh, *size);
      		cx++;
      		
    	}
  	}

    
    
  	return 0;
}


int getCmdKeyPress(char *buff) {

	char c='\0';
	int cnt=0;
  	int cbuf=0;
  	char esc_seq[2];
    int exit =0;

  	while (read(STDIN_FILENO, &c, 1) != 1);

	while(1) {



    	if (iscntrl(c)) {

      		switch(c) {

      			// when backspace is pressed (ascii-127)
      			case 127:
      				startRawMode();
      				write(STDOUT_FILENO, "\b \b", 3);
      				endRawMode();
      				break;

      			// when enter key is pressed (ascii-13)
      			case 13:
      				//return 1;
      				exit =1;
      				break;
      			
      			// when an escape charecter or sequence is pressed 
      			case 27:

      				while(read(STDIN_FILENO, &c, 1)) {
      					esc_seq[cnt++] = c;
      				}

      				if(cnt == 2) {

      					moveCursor(esc_seq);
      					cnt=0;

      				} else {

      					return -1;

      				}
      				break;
      		}

    	} else {
    		
    		write(STDOUT_FILENO, &c, 1);
            buff = (char*)realloc(buff, ++cbuf);
            buff[cbuf-1] = c;
      		
    	  }
    	  if(exit==1) return 1;
      while (read(STDIN_FILENO, &c, 1) != 1);
  }

  	return 0;
}


void editMode() {

	int mode=0;

	while(1) {

  		mode = getKeyPress();
  		if(mode == -1)
  			break;

  	}

}

void dispMode(char *c, int size) {

	struct winsize ws;
	char *edit="---INSERT MODE---", *cmd="---COMMAND MODE---";
	char buff[7];
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	//printf("%d, ")
	sprintf(buff, "\x1b[%d;%dH", ws.ws_row, 0);
	write(STDOUT_FILENO, buff, 7);
	if(strcmp(c, edit)==0) {
		write(STDOUT_FILENO, c, size);
		write(STDOUT_FILENO, "\x1b[0;0H", 6);
	} else if(strcmp(c, cmd)==0) {
		write(STDOUT_FILENO, "\x1b[0K", 4);

	}
	
	//write(STDOUT_FILENO, &buff, 7);
	//write(STDOUT_FILENO, &c, size);

}

void cmdMode(int status)  {
	dispMode("---COMMAND MODE---", 20);	
	if(status) {
		write(STDOUT_FILENO, "Invalid Command", 15);
	}
}

void terminal(char *cmd) {

  // clear screen
  // execute command
  // wait for key press
  // reload the data structure
  
  char *c;
  clscr();
  system(cmd+1);
  write(STDOUT_FILENO, "\r\n", 1);
  write(STDOUT_FILENO, "Press any key to return back to editor", 38);
  while (read(STDIN_FILENO, &c, 1) != 1);
  clscr();
  disp();
  write(STDOUT_FILENO, "\x1b[0;0H", 6); 


}
