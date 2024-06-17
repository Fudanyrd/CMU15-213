/**
 * nsh.c: A naive shell implementation
 *
 * Just to get familiar with chapter 8 of csapp.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/** static type cast */
#define static_cast(a, tp) ((tp)(a))

/**
 * @brief fork function with error handling
 */
pid_t myfork(void);

volatile sig_atomic_t flag;
volatile sig_atomic_t pid_glb;

#define N 5

/** maximum length of a line*/
const unsigned max_len = 256U;

const char *first = "nsh > ";
const char *second = "... > ";

/************************************************
 *             In file Token.h
 ************************************************/
struct Token {
  char *word_;         // single word
  struct Token *next_; // pointer to the next token.
};
/**
 * @return a pointer to a allocated token
 */
struct Token *new_token(void);
/**
 * @brief free a list of tokens.
 */
void free_token(struct Token *head);

/************************************************
 *             In file Command.h
 ************************************************/
struct Command {
  struct Token *head_; // head to the linked list of tokens
  struct Token *tail_; // tail of the linked list of tokens
  unsigned num_token_; // number of tokens
};
void init_cmd(struct Command *cmd) {
  cmd->head_ = NULL;
  cmd->tail_ = NULL;
  cmd->num_token_ = 0;
}
void free_cmd(struct Command *cmd) {
  if (cmd != NULL) {
    free_token(cmd->head_);
  }
}
/**
 * @brief take input somewhere and get the command.
 * @param cmd[out] the parse result
 */
void parse(struct Command *cmd, FILE *in);

/************************************************
 *             In file Command.h
 ************************************************/
/** execution meta */
struct ExecMeta {
  char *executable_; // executable
  char **argv_;      // arguments(should end with NULL)
};
/** use command to initialize a meta[out] */
void init_exec_meta(struct ExecMeta *meta, struct Command *cmd);
/** free the meta */
void free_meta(struct ExecMeta *meta) { free(meta->argv_); }

/************************************************
 *              In file String.h
 ************************************************/
struct String {
  char *dat_;        // the actuall line.
  unsigned int len_; // number of chars in the line(including ending '\0')
  unsigned int vol_; // current allocated space of the line
};
void init_str(struct String *s) {
  s->dat_ = NULL;
  s->len_ = s->vol_ = 0;
}
/**
 * @brief read line from a input stream.
 * @param line[out] the line from input.
 * @param in the input stream
 */
void get_line(struct String *line, FILE *in);

/**
 * @brief put a character to the back of a string
 * @return 0 if succeeded.
 */
int str_push_back(struct String *s, char ch);
void free_str(struct String *s) { free(s->dat_); }

/**
 * signal handlers
 */
void sigchld_handler(int s) {
  int oldflag = flag;
  pid_glb = waitpid(-1, NULL, 0);
  flag = oldflag;
}
void sigint_handler(int s) {
  // write is signal-safe.
  write(STDOUT_FILENO, "\n", 1);
}

int main(int argc, char **argv) {
  struct Command cmd;
  sigset_t mask;
  sigset_t prev;
  signal(SIGCHLD, sigchld_handler);
  signal(SIGINT, sigint_handler);
  sigemptyset(&mask);
  sigaddset(&mask, SIGCHLD);

  while (1) {
    init_cmd(&cmd);
    parse(&cmd, stdin);
    if (cmd.head_ == NULL) {
      continue;
    }
    if (strcmp(cmd.head_->word_, "exit") == 0) {
      free_cmd(&cmd);
      break;
    }
    sigprocmask(SIG_BLOCK, &mask, &prev);

    // start execution
    pid_t pid;
    if ((pid = myfork()) == 0) { // child process
      struct ExecMeta meta;
      init_exec_meta(&meta, &cmd);
      int stat = execve(meta.executable_, meta.argv_, NULL);
      if (stat < 0) {
        fprintf(stderr, "executable %s not found.\n", cmd.head_->word_);
      }

      // end
      free_meta(&meta);
      free_cmd(&cmd);
      exit(0);
    }
    pid_glb = 0;
    sigprocmask(SIG_SETMASK, &prev, NULL);
    while (pid_glb == 0) {
    }

    free_cmd(&cmd);
  }

  printf("Well, bye.\n");
  exit(0);
}

pid_t myfork() {
  pid_t res = fork();

  // error handling
  if (res < 0) {
    fprintf(stderr, "fork failure, abort");
    exit(0);
  }

  return res;
}

struct Token *new_token(void) {
  struct Token *res = static_cast(malloc(sizeof(struct Token)), struct Token *);
  if (res == NULL) {
    fprintf(stderr, "malloc failure, abort\n");
  }
  res->word_ = NULL;
  res->next_ = NULL;
  return res;
}

void free_token(struct Token *head) {
  if (head == NULL) {
    return;
  }
  if (head->word_ != NULL) {
    free(head->word_);
  }
  free_token(head->next_);
  free(head);
}

/**
 * @return the end(overflow value) of a token or blank;
 * -1 if reach the end of the line.
 */
int seek(const char *buffer, int start) {
  int end;
  switch (buffer[start]) {
  case '\0':
  case '\n':
  case '\\':
    return -1;
  default:
    break;
  }

  if (buffer[start] == ' ' || buffer[start] == '\t') {
    for (end = start + 1; buffer[end] == ' ' || buffer[end] == '\t'; ++end) {
      if (buffer[end] == '\n' || buffer[end] == '\\' || buffer[end] == '\0') {
        break;
      }
    }
  } else {
    for (end = start + 1; buffer[end] != ' ' && buffer[end] != '\t'; ++end) {
      if (buffer[end] == '\n' || buffer[end] == '\\' || buffer[end] == '\0') {
        break;
      }
    }
  }

  return end;
}

/**
 * @brief split a line, only reserve the tokens.
 * @return 0 if the line not end with '\'(i.e. the command ended)
 */
int split(const char *buffer, struct Command *cmd) {
  /**
   * WARNING:
   * You need to take care of head, tail and num_token of cmd.
   */

  int l = 0; // iter from the left
  int r;     // iter to the right
  int i;

  while ((r = seek(buffer, l)) != -1) {
#ifdef DEBUG
    // check overflow issues
    assert(r > l);
#endif
    // create tokens or not?
    if (buffer[l] != ' ' && buffer[l] != '\t') {
      // a token!
      char *word = static_cast(malloc(sizeof(char) * (r - l + 1)), char *);
      if (word == NULL) {
        fprintf(stderr, "malloc failure, abort");
      }

      // copy the token
      for (i = 0; i < (r - l); ++i) {
        word[i] = buffer[l + i];
      }
      word[i] = 0;

      // create a new node
      if (cmd->tail_ != NULL) { // initialized cmd
        cmd->tail_->next_ = new_token();
        cmd->tail_ = cmd->tail_->next_;
      } else { // not initialized cmd
        cmd->head_ = cmd->tail_ = new_token();
      }

      cmd->tail_->word_ = word;
      cmd->num_token_ += 1;
    }
    l = r;
  }

#ifdef DEBUG
  // check that we have actually reached end of line.
  assert(buffer[l] == '\n' || buffer[l] == '\\' || buffer[l] == '\0');
#endif
  return buffer[l] == '\\';
}

void parse(struct Command *cmd, FILE *in) {
  init_cmd(cmd);
  struct String line;
  init_str(&line);

  printf("%s", first);
  get_line(&line, in);
  while (split(line.dat_, cmd)) {
    printf("%s", second);
    get_line(&line, in);
  }

  free_str(&line);
  return;
}

void init_exec_meta(struct ExecMeta *meta, struct Command *cmd) {
  if (cmd == NULL) {
    fprintf(stderr, "command is null?? IMPOSSIBLE!");
    return;
  }
  meta->executable_ = NULL;
  meta->argv_ = NULL;
  if (cmd->num_token_ == 0) {
    return;
  }

  meta->executable_ = cmd->head_->word_;

  struct Token *iter = cmd->head_;
  int i = 0;
  meta->argv_ =
      static_cast(malloc(sizeof(char *) * (cmd->num_token_ + 1)), char **);
  while (iter != NULL) {
    meta->argv_[i++] = iter->word_;
    iter = iter->next_;
  }

  meta->argv_[i] = NULL;
}

int str_push_back(struct String *s, char ch) {
  if (s == NULL) {
    return -1;
  }

  if (s->len_ < s->vol_) {
    s->dat_[s->len_] = ch;
    s->len_++;
    return 0;
  }
  // else s->len_ == s->vol_
  s->vol_ = s->vol_ << 1;
  s->dat_ = static_cast(
      realloc(s->dat_, s->vol_ == 0 ? 4U : sizeof(char) * s->vol_), char *);
  if (s->dat_ == NULL) { // oops, fail
    return -1;
  }
  s->dat_[s->len_] = ch;
  s->len_++;
  return 0;
}

void get_line(struct String *line, FILE *in) {
  if (line == NULL) {
    fprintf(stderr, "Invalid argument to line: line is null\n");
    return;
  }

  if (line->dat_ != NULL) {
    free(line->dat_);
    init_str(line);
  }
  line->dat_ = static_cast(malloc(sizeof(char) * 4), char *);
  line->len_ = 0;
  line->vol_ = 4;

  char ch;
  while ((ch = fgetc(in)) != '\n') {
    if (str_push_back(line, ch) != 0) {
      fprintf(stderr, "str push back failure, abort");
      exit(0);
    }
  }
  if (str_push_back(line, '\n') != 0) {
    fprintf(stderr, "str push back failure, abort");
    exit(0);
  }
  if (str_push_back(line, '\0') != 0) {
    fprintf(stderr, "str push back failure, abort");
    exit(0);
  }
}
