#include <stdio.h>
#include <unistd.h>

#define SIZE 49

int field[SIZE][SIZE];
int new_field[SIZE][SIZE];

void initialize_field();
void print_field();
void print_cell(int c);
void overwrite_field(int **f);
int det_cell_status(int x, int y);
int living_neighbors(int x, int y);
void step();
void print_dashline();
void clear();

int main(int argc, const char *argv[])
{
	initialize_field(field);
	initialize_field(new_field);

	/* Pentadecathlon */
	field[11][7] = 1;
	field[11][8] = 1;
	field[11][10] = 1;
	field[11][11] = 1;
	field[11][12] = 1;
	field[11][13] = 1;
	field[11][15] = 1;
	field[11][16] = 1;
	field[10][9] = 1;
	field[10][14] = 1;
	field[12][9] = 1;
	field[12][14] = 1;

	/* glider */
	field[0][1] = 1;
	field[1][2] = 1;
	field[2][0] = 1;
	field[2][1] = 1;
	field[2][2] = 1;

	field[20][20] = 1;
	field[19][20] = 1;
	field[18][20] = 1;
	field[18][21] = 1;
	field[18][22] = 1;
	field[19][22] = 1;
	field[20][22] = 1;

	field[22][20] = 1;
	field[23][20] = 1;
	field[24][20] = 1;
	field[24][21] = 1;
	field[24][22] = 1;
	field[23][22] = 1;
	field[22][22] = 1;

	field[24][24] = 1;
	field[24][23] = 1;
	field[25][24] = 1;
	field[23][24] = 1;
	field[23][25] = 1;

	int it = 0;

	while (1) {
		clear();
		print_field();
		printf("Iteration: %d\n", ++it);
		step();
		usleep(100*1000);
	}

	return 0;
}

void initialize_field(int f[SIZE][SIZE])
{
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			f[i][j] = 0;
		}
	}
}

void print_field()
{
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			print_cell(field[i][j]);
		}
		printf("\n");
	}
}

void print_cell(int c)
{
	if (c) printf("o");
	else printf(" ");
}

void overwrite_field(int **f)
{
	for (int i = 0; i < SIZE; i++) {
		for (int j = 0; j < SIZE; j++) {
			field[i][j] = f[i][j];
		}
	}
}

int living_neighbors(int x, int y)
{
	int sum = 0;

	if (x == 0) { /* top */
		if (y == 0) { /* top left corner */
			if (field[0][1]) sum += 1;
			if (field[1][0]) sum += 1;
			if (field[1][1]) sum += 1;
		} else {
			if (y == SIZE-1) { /* top right corner */
				if (field[0][SIZE-2]) sum += 1;
				if (field[1][SIZE-1]) sum += 1;
				if (field[1][SIZE-2]) sum += 1;
			} else { /* somewhere in between */
				if (field[x][y-1]) sum += 1;
				if (field[x][y+1]) sum += 1;
				if (field[x+1][y-1]) sum += 1;
				if (field[x+1][y]) sum += 1;
				if (field[x+1][y+1]) sum += 1;
			}
		}
	}
	if (x == SIZE-1) { /* bottom */
		if (y == 0) { /* bottom left corner */
			if (field[x-1][0]) sum += 1;
			if (field[x-1][1]) sum += 1;
			if (field[x][1]) sum += 1;
		} else {
			if (y == SIZE-1) { /* bottom right corner */
				if (field[x][y-1]) sum += 1;
				if (field[x-1][y-1]) sum += 1;
				if (field[x-1][y]) sum += 1;
			} else { /* somewhere in between */
				if (field[x][y-1]) sum += 1;
				if (field[x][y+1]) sum += 1;
				if (field[x-1][y-1]) sum += 1;
				if (field[x-1][y]) sum += 1;
				if (field[x-1][y+1]) sum += 1;
			}
		}
	}
	if (y == 0) { /* left */
		if (x>0 && x<(SIZE-1)) { /* just somewhere in between, corners were handled earlier */
			if (field[x-1][y]) sum += 1;
			if (field[x-1][y+1]) sum += 1;
			if (field[x][y+1]) sum += 1;
			if (field[x+1][y+1]) sum += 1;
			if (field[x+1][y]) sum += 1;
		}
	}
	if (y == SIZE-1) { /* right */
		if (x>0 && x<(SIZE-1)) { /* again just somewhere in between, corners were handled earlier */
			if (field[x-1][y]) sum += 1;
			if (field[x-1][y-1]) sum += 1;
			if (field[x][y-1]) sum += 1;
			if (field[x+1][y-1]) sum += 1;
			if (field[x+1][y]) sum += 1;
		}
	}
	if (x>0 && x<(SIZE-1) && y>0 && y<(SIZE-1)) { /* somewhere in the field */
		if (field[x-1][y-1]) sum += 1;
		if (field[x-1][y]) sum += 1;
		if (field[x-1][y+1]) sum += 1;
		if (field[x][y-1]) sum += 1;
		if (field[x][y+1]) sum += 1;
		if (field[x+1][y-1]) sum += 1;
		if (field[x+1][y]) sum += 1;
		if (field[x+1][y+1]) sum += 1;
	}

	return sum;
}

int det_cell_status(int x, int y)
{
	if (field[x][y] == 0) {
		if (living_neighbors(x,y) == 3) /* new born */ return 1;
		else return 0;
	} else {
		if (living_neighbors(x,y) < 2) /* solitude */ return 0;
		else {
			if (living_neighbors(x,y) > 3) /* overpopulation */ return 0;
			else /* nothing happened */ return 1;
		}
	}
}

void step()
{
	int i,j;
	/* determine new field */
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			new_field[i][j] = det_cell_status(i,j);
		}
	}
	/* field <- new_field */
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			field[i][j] = new_field[i][j];
		}
	}
}

void print_dashline()
{
	for (int i = 0; i < SIZE; i++) {
		printf("-");
	}
	printf("\n");
}

void clear()
{
	printf("\e[1;1H\e[2J");
}
