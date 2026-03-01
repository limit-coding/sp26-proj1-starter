#include "game.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "snake_utils.h"

/* Helper function definitions */
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch);
static bool is_tail(char c);
static bool is_head(char c);
static bool is_snake(char c);
static char body_to_tail(char c);
static char head_to_body(char c);
static unsigned int get_next_row(unsigned int cur_row, char c);
static unsigned int get_next_col(unsigned int cur_col, char c);
static void find_head(game_t *game, unsigned int snum);
static char next_square(game_t *game, unsigned int snum);
static void update_tail(game_t *game, unsigned int snum);
static void update_head(game_t *game, unsigned int snum);

/* Task 1 */
game_t *create_default_game() {
  // TODO: Implement this function.
  //task1要求：生成地图，初始化数据，生成蛇
  game_t* game=malloc(sizeof(game_t));
  if(game==NULL) return NULL;
  //初始化地图信息
  game->num_rows=18;
  //首先初始化指针序列，等待下次继续初始化
  game->board=malloc(sizeof(char*)*game->num_rows);
  //安全保障
  if(game->board==NULL) {
    free(game);
    return NULL;
  }
  //关于蛇的初始化
  game->num_snakes=1;
  game->snakes=malloc(sizeof(snake_t));
  char* default_layout[]={
    "####################\n",
    "#                  #\n",
    "# d>D    *         #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "#                  #\n",
    "####################\n"
    
  };
  //先申请内存，然后复制board信息
  for(int i=0;i<game->num_rows;++i){
    game->board[i]=malloc(sizeof(char)*22);
    strcpy(game->board[i],default_layout[i]);
  }
  //初始化蛇的位置
  game->snakes[0].head_col=4;
  game->snakes[0].head_row=2;
  game->snakes[0].tail_col=2;
  game->snakes[0].tail_row=2;
  game->snakes[0].live=true;

  
  
  return game;
}

/* Task 2 */
void free_game(game_t *game) {
  // TODO: Implement this function.
  if(game==NULL) return;
  for(int i=0;i<game->num_rows;++i){
    free(game->board[i]);
  }
  if(game->board!=NULL) free(game->board);
  if(game->snakes!=NULL) free(game->snakes);
  free(game);
  return;
}

/* Task 3 */
void print_board(game_t *game, FILE *fp) {
  // TODO: Implement this function.
  if(game==NULL || fp==NULL) return;
  for(int i=0;i<game->num_rows;++i){
    fprintf(fp,"%s",game->board[i]);
  }
  return;
}

/*
  Saves the current game into filename. Does not modify the game object.
  (already implemented for you).
*/
void save_board(game_t *game, char *filename) {
  FILE *f = fopen(filename, "w");
  print_board(game, f);
  fclose(f);
}

/* Task 4.1 */

/*
  Helper function to get a character from the board
  (already implemented for you).
*/
char get_board_at(game_t *game, unsigned int row, unsigned int col) { return game->board[row][col]; }

/*
  Helper function to set a character on the board
  (already implemented for you).
*/
static void set_board_at(game_t *game, unsigned int row, unsigned int col, char ch) {
  game->board[row][col] = ch;
}

/*
  Returns true if c is part of the snake's tail.
  The snake consists of these characters: "wasd"
  Returns false otherwise.
*/
static bool is_tail(char c) {
  // TODO: Implement this function.
  //wasd
  if(c!='w' && c!='a' && c!='s' && c!='d') return false;
  return true;
}

/*
  Returns true if c is part of the snake's head.
  The snake consists of these characters: "WASDx"
  Returns false otherwise.
*/
static bool is_head(char c) {
  // TODO: Implement this function.
  //WASDx
  if(c!='W' && c!='A' && c!='S' && c!='D' && c!='x') return false;
  return true;
}

/*
  Returns true if c is part of the snake.
  The snake consists of these characters: "wasd^<v>WASDx"
*/
static bool is_snake(char c) {
  // TODO: Implement this function.
  //wasd^<v>WASDx
  if(c!='w'&&c!='a'&& c!='s' && c!='d' && c!='^' && c!='<' && c!='v' && c!='>' && c!='W' && c!='A' && c!='S' && c!='D'&& c!='x') return false;
  return true;
}


/*
  Converts a character in the snake's head ("WASD")
  to the matching character representing the snake's
  body ("^<v>").
*/
static char head_to_body(char c) {
  // TODO: Implement this function.
  if(c=='W') return '^';
  if(c=='A') return '<';
  if(c=='S') return 'v';
  if(c=='D') return '>';
  return '?';
}

/*
  Returns cur_row + 1 if c is 'v' or 's' or 'S'.
  Returns cur_row - 1 if c is '^' or 'w' or 'W'.
  Returns cur_row otherwise.
*/
static unsigned int get_next_row(unsigned int cur_row, char c) {
  // TODO: Implement this function.
  if(c=='v' || c=='s' || c=='S') return cur_row+1;
  if(c=='^' || c=='w' || c=='W') {
    return (cur_row>0) ? cur_row-1:0;
  }
  return cur_row;
}

/*
  Returns cur_col + 1 if c is '>' or 'd' or 'D'.
  Returns cur_col - 1 if c is '<' or 'a' or 'A'.
  Returns cur_col otherwise.
*/
static unsigned int get_next_col(unsigned int cur_col, char c) {
  // TODO: Implement this function.
  if(c=='>' || c=='d' || c=='D') return cur_col+1;
  if(c=='<' || c=='a' || c=='A') {
    return (cur_col>0) ? cur_col-1:0;
  }
  return cur_col;
}

/*
  Task 4.2

  Helper function for update_game. Return the character in the cell the snake is moving into.

  This function should not modify anything.
*/
static char next_square(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  snake_t snake=game->snakes[snum];
  unsigned int cur_row=game->snakes->head_row;
  unsigned int cur_col=game->snakes->head_col;
  char head_char=get_board_at(game,cur_row,cur_col);
  unsigned int next_row=get_next_row(cur_row,head_char);
  unsigned int next_col=get_next_col(cur_col,head_char);
  return get_board_at(game,next_row,next_col);
}

/*
  Task 4.3

  Helper function for update_game. Update the head...

  ...on the board: add a character where the snake is moving

  ...in the snake struct: update the row and col of the head

  Note that this function ignores food, walls, and snake bodies when moving the head.
*/
static void update_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  //这里使用引用是必须的
  //创建引用
  //新的头 new_col new_row
  //新的身体 
  snake_t* snake=&(game->snakes[snum]);
  unsigned int cur_row=snake->head_row;
  unsigned int cur_col=snake->head_col;
  char head_char=get_board_at(game,cur_row,cur_col);

  int next_row=get_next_row(cur_row,head_char);
  int next_col=get_next_col(cur_col,head_char);
  char new_body=head_to_body(head_char);
  //snake仅仅是一个坐标，game才是整个图纸
  //更新头 头的字符是不变的，之前是D，之后还是D
  set_board_at(game, next_row, next_col , head_char) ;
  //更新身体  之前是S，
  set_board_at(game, cur_row, cur_col, new_body) ;

  snake->head_col=next_col;
  snake->head_row=next_row;

  return;
}

/*
  Task 4.4

  Helper function for update_game. Update the tail...

  ...on the board: blank out the current tail, and change the new
  tail from a body character (^<v>) into a tail character (wasd)

  ...in the snake struct: update the row and col of the tail
*/

static char body_to_tail(char c){
  if(c=='^') return 'w';
  if(c=='<') return 'a';
  if(c=='v') return 's';
  if(c=='>') return 'd';
  return '?';
}
static void update_tail(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  snake_t* snake=&(game->snakes[snum]);
  //我已经获取了尾巴的坐标和char
  unsigned int cur_col=snake->tail_col;
  unsigned int cur_row=snake->tail_row;
  char tail_char=get_board_at(game,cur_row,cur_col);

  //现在需要获取下一个位置的坐标(既是新的尾巴，也是最后一个body)
  unsigned int next_col=get_next_col(cur_col,tail_char);
  unsigned int next_row=get_next_row(cur_row,tail_char);
  char body_char=get_board_at(game,next_row,next_col);

  char new_tail=body_to_tail(body_char);
  //更新尾巴
  set_board_at(game,next_row,next_col,new_tail);
  //把过去的尾巴变成空格
  set_board_at(game,cur_row,cur_col,' ');

  snake->tail_col=next_col;
  snake->tail_row=next_row;
  return;
}

/* Task 4.5 */
void update_game(game_t *game, int (*add_food)(game_t *game)) {
  // TODO: Implement this function.

  return;
}

/* Task 5.1 */
char *read_line(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/* Task 5.2 */
game_t *load_board(FILE *fp) {
  // TODO: Implement this function.
  return NULL;
}

/*
  Task 6.1

  Helper function for initialize_snakes.
  Given a snake struct with the tail row and col filled in,
  trace through the board to find the head row and col, and
  fill in the head row and col in the struct.
*/
static void find_head(game_t *game, unsigned int snum) {
  // TODO: Implement this function.
  return;
}

/* Task 6.2 */
game_t *initialize_snakes(game_t *game) {
  // TODO: Implement this function.
  return NULL;
}
