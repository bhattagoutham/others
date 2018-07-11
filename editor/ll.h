//Roll.No: 20172063
//Roll.No: Goutham Bhatta

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
struct node *head = NULL;

struct node {

	int row;
	int row_size;
	char *col;
	struct node *next;
};


int insertCol(struct node *temp, int col, char data) {

	char *cln;
	cln = temp->col;

	if(col > temp->row_size-1) {
		temp->row_size = temp->row_size+1;
		cln = (char*)realloc(cln, temp->row_size);
	}

	*(cln+col) = data;
	temp->col = cln;
	
	if(data == 127) {
		//struct refresh *r;
		//r = (struct refresh*)malloc(sizeof(struct refresh));

		write(STDOUT_FILENO, "\x1b[2K", 4);
		char buff[6];
        sprintf(buff, "\x1b[%d;%dH", temp->row, 0);
		write(STDOUT_FILENO, buff, 6);
		write(STDOUT_FILENO, cln, temp->row_size);

		return 0;

	}
	//*ptr = cln;
	//*size = temp->row_size;

	 return 0;
}

int insertNewRow(int row, char data, struct node *prev) {

	
	struct node *temp;
	char *c;
	temp = (struct node*)malloc(sizeof(struct node));
	temp->row = row;
	temp->next = NULL;
	c = (char*)malloc(sizeof(char));
	*c = data;
	temp->col = c;
	temp->row_size = 1;	

	if(prev == NULL) {

		head = temp;

	} else {

		prev->next = temp;

	}

	return 1;

}


int insert(int col, int row, char data) {
	
	//printf("I am here.");
	struct node *temp;
	temp = head;
	
	if(temp == NULL) {

		insertNewRow(row, data, temp);
		
		
	} else {

		while(1) {
			
			if(temp->row == row) break;

			if(temp->next == NULL) break; 
			
			temp = temp->next;	
						

		}

		if(temp->row == row) {

			
			insertCol(temp, col, data);

		} else {
			

			insertNewRow(row, data, temp);
		}
		
	}

	return 0;
}


void disp() {
	struct node *temp;
	temp = head;
	if(head == NULL) return;
	//printf("%s\n", head->col);
	while(1) {
		printf("%s\r\n", temp->col);
		if(temp->next == NULL) break;
		temp = temp->next;
	}
}

void file_write(char *fname) {

	int fd = creat(fname, S_IRUSR | S_IWUSR);
	struct  node * temp;
	temp = head;

	if(head == NULL) return;
	
	while(1) {
		
		char *c;
		c = temp->col;

		for(int i=0;i < temp->row_size; i++) {
			
			if(*(c+i) != 127) {

				write(fd, c+i, 1);
			}
		}

		if(temp->next == NULL) break;
		temp = temp->next;
		write(fd, "\r\n", 2);
	}
	close(fd);
		
}


