#include <iostream>
#include <curses.h>
#include <time.h>
#include <unistd.h>
#include <cstdlib>
/* ------------------------------------------------------ */
/* ------------------------------------------------------ */
using namespace std;
/* ------------------------------------------------------ */
/* trida reprezentujici jeden radek vcetne vsech metod */
/* ------------------------------------------------------ */
class row {
  public:
    row();
    char gen_rand();
    void print_one();
    void remove_one();
    void run();
    void check_pos(int& size);
    void init(int size, WINDOW * w, int delay);
  private:
    int size, cols, rows, delay, shift;
    int size_print, size_remove, start_x, start_y;
    int par_size, par_delay;
    WINDOW * w;
};
/* ------------------------------------------------------ */
/* trida reprezentujici celou matici radku */
/* ------------------------------------------------------ */
class matrix {
  public:
    matrix(int count, int size, int delay, WINDOW * w);
    ~matrix();
    void show();
  private:
    row * r;
    int count;
};
/* ------------------------------------------------------ */
/* konstruktor */
/* ------------------------------------------------------ */
matrix::matrix(int count, int size, int delay, WINDOW * w)
{
  this->count = count;
  r = new row[count]; // vytvorime dany pocet radku

  for(int i = 0; i < count; i++)
    r[i].init(size, w, delay);
}
/* ------------------------------------------------------ */
matrix::~matrix()
{
  delete r; // vsechny radky zase smazeme 
}
/* ------------------------------------------------------ */
void matrix::show()
{
  for(int i = 0; i < count; i++)  
    r[i].run();
}
/* ------------------------------------------------------ */
/* bezparametricky konstruktor */
/* ------------------------------------------------------ */
row::row()
{
   
}
/* ------------------------------------------------------ */
/* ------------------------------------------------------ */
void row::init(int size, WINDOW * w, int delay)
{
  cols = getmaxx(w);
  rows = getmaxy(w);

  start_x = rand() % cols;
  start_y = rand() % rows;
  
  par_size = size;
  this->size = rand() % par_size;
  
  while(this->size < 5) // velikost alespon 5
    this->size = rand() % par_size;

  this->delay = delay;
  shift = 0;    // posun od horni casti obrazovky

  par_delay = delay;
  this->w = w;
  size_print = 0;
  size_remove = 0;
}
/* ------------------------------------------------------ */
/* funkce pro kontrolu pozice
 */
/* ------------------------------------------------------ */
void row::check_pos(int& size)
{
  if(size == 0) {
    move(start_y, start_x); // chceme spravne souradnice
    size++;
  }
  
  else {
    if(start_y + size >= rows)
      move(shift++, start_x);
    else
      move(start_y + size, start_x);
    size++;
  }
}
/* ------------------------------------------------------ */
/* hlavni funkce tridy row, na zaklade promenne print_size
 * bud tiskne nebo maze
 */
/* ------------------------------------------------------ */
void row::run()
{
  if(size_print < size)
    print_one();
  
  else
    remove_one();
}
/* ------------------------------------------------------ */
/* v kazdem volani metody je nutne spravne nastavit pozici ! */
/* ------------------------------------------------------ */
void row::print_one()
{
  check_pos(size_print);

  printw("%c", gen_rand());
  refresh();
  usleep(delay);
  
  if(size_print == size)
    shift = 0;          // vynulovani pro remove
}
/* ------------------------------------------------------ */
char row::gen_rand()
{
  char c = rand() % 127;
  while(c <= 32)    // avoid using special characters
    c = rand() % 127;
  
  return c; 
}
/* ------------------------------------------------------ */
/* v kazdem volani metody je nutne spravne nastavit pozici ! */
/* ------------------------------------------------------ */
void row::remove_one()
{
  check_pos(size_remove); 

  printw(" ");
  refresh();
  usleep(delay);

  if(size_remove == size) {
    start_x = rand() % cols;
    start_y = rand() % rows;
    this->size = rand() % par_size;
    
    while(this->size < 5)   // velikost alespon 5
      this->size = rand() % par_size;
    
    shift = 0;    // posun od horni casti obrazovky
    size_print = 0;
    size_remove = 0;
  }
}
/* ------------------------------------------------------ */
void set_param_value(int & var, char * param)
{
  char * err_check;
  int val = strtol(param, &err_check, 10);

  if(err_check == param && val == 0)		// conversion failed - probably bad param -> use default value
    return;
  else
    var = val;
}
/* ------------------------------------------------------ */
int main(int argc, char * argv[])
{
  WINDOW * w;
  int columns = 150, len = 30, delay = 500;
  bool wide_chars = false;
  int c;
  
  w = initscr();    // init okna
  cbreak();         // okamzity vstup pro program
  noecho();         // nechceme vypisovat uzivatelsky vstup ?
  srand((unsigned)time(NULL));  // generator nahodnych cisel
  nodelay(w, true); // neblokujici vstup
  curs_set(0);      // neviditelny kurzor

  start_color();
  init_pair(1, COLOR_GREEN, COLOR_BLACK);
  attron(COLOR_PAIR(1));

  while ((c = getopt (argc, argv, "c:l:d:w")) != -1) {
    switch(c) {
      case 'c':
	set_param_value(columns, optarg);
      break;
      case 'l':
	set_param_value(len, optarg);
      break;
      case 'd':
	set_param_value(delay, optarg);
      break;
      case 'w':
	wide_chars = true;
      break;
    }
  }

  matrix m(columns, len, delay, w);

  while(1) {
    m.show();

    if(getch() != ERR)
      break; // byl zadan vstup
  }

  attroff(COLOR_PAIR(1));
  endwin();

  printf("usage: \n");
  printf("./ncurses_matrix [-cldw]\n");
  printf("-c count 	column count\n");
  printf("-l length 	column length\n");
  printf("-d delay 	delay in ms\n");
  printf("-w 		use unicode characters\n");
  printf("source code available at https://github.com/lager1/ncruses_matrix\n");

  return 0;  
}
