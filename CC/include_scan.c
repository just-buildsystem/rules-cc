/* Copyright 2024 Huawei Cloud Computing Technology Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * Include scanner
 *   Parse file paths from command's space-separated stdout, select those that
 *   start with "include/", and copy them to the user-defined output directory.
 *
 * Usage:
 *   include_scan OUT_DIR ARGV...
 *
 * Example:
 *   ./include_scan out gcc -isystem include foo.c -E -M
 *   -> Generates output directory "out/include" from gcc's output
 */

#include <assert.h>
#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <libgen.h>       /* dirname */
#include <linux/limits.h> /* PATH_MAX */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h> /* mkdir */
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_SIZE PATH_MAX /* support up to PATH_MAX path lengths */
#define LIST_SIZE 1024     /* initial capacity of include list */

/* Buffer that can load in chunks, while maintaining previously unread data */
struct ShiftBuffer {
  char *pos;   /* position in full buffer */
  char *start; /* start of buffer's second half */
  size_t size; /* size of data in buffer's second half */
  char buffer[READ_SIZE * 2 + 1];
};

/* List of includes that dynamically resizes */
struct IncludeList {
  char **pos;      /* position for reading */
  size_t size;     /* number of list entries */
  size_t capacity; /* maximum capacity */
  char **list;
};

int helper_mkdir_p(char *path, mode_t mode);
int helper_pathcmp(const void *a, const void *b);
void helper_normpath(char *out_path, const char *in_path, size_t len);
int helper_copyfile(const char *from, const char *to, int wopen_mayfail);

/* ShiftBuffer functions */
void sbuf_init(struct ShiftBuffer *sbuf);
void sbuf_addpos(struct ShiftBuffer *sbuf, size_t offset);
char *sbuf_pos(struct ShiftBuffer *sbuf);
char *sbuf_end(struct ShiftBuffer *sbuf);
int sbuf_eof(struct ShiftBuffer *sbuf);
int sbuf_load(struct ShiftBuffer *sbuf, int fd);
int sbuf_shift(struct ShiftBuffer *sbuf);

/* IncludeList functions */
void incl_init(struct IncludeList *incl);
void incl_free(struct IncludeList *incl);
void incl_resize(struct IncludeList *incl, size_t new_cap);
void incl_append(struct IncludeList *incl, char *path);
void incl_setpos(struct IncludeList *incl, size_t pos);
char *incl_next(struct IncludeList *incl);

/* Parse paths from fd's space-separated string, select those that start with
 * pattern, and return normalized and deduplicated list.
 */
void parse_paths(struct IncludeList *incl, int fd, char const *pattern,
                 char const *prefix);

int main(int argc, const char *argv[]) {
  int fd[2];
  pid_t pid;
  int status = 0;
  int retval = 0;
  char *path = NULL;
  char *prefix;
  size_t prefix_len = 0;
  struct IncludeList incl;

  if (argc < 3) {
    fprintf(stderr, "usage: %s PREFIX FILE ARGV...\n", argv[0]);
    fprintf(stderr, "Missing arguments\n");
    exit(EXIT_FAILURE);
  }
  if (pipe(fd) < 0) {
    fprintf(stderr, "Failed to create pipe\n");
    return 1;
  }
  pid = fork();
  if (-1 == pid) {
    fprintf(stderr, "Failed to fork process\n");
    return 1;
  }

  if (pid == 0) { /* exec in child */
    dup2(fd[1], STDOUT_FILENO);
    close(fd[1]);
    close(fd[0]);
    execvp(argv[2], (char *const *)(argv + 2));
    exit(EXIT_FAILURE);
  }

  close(fd[1]);

  /* obtain include list from paths starting with "include/" */
  incl_init(&incl);
  prefix_len = strlen(argv[1]);
  prefix = (char *)calloc(prefix_len + 2, sizeof(char));
  strcat(prefix, argv[1]);
  strcat(prefix + prefix_len, "/");
  prefix_len += 1;
  parse_paths(&incl, fd[0], "include/", prefix);
  free(prefix);
  close(fd[0]);

  /* wait for child to finish */
  while (1) {
    if (waitpid(pid, &status, 0) == -1) {
      fprintf(stderr, "Waiting for child failed with: %s\n", strerror(errno));
      exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
      retval = WEXITSTATUS(status);
      break;
    } else if (WIFSIGNALED(status)) {
      const int kSignalBit = 128;
      int sig = WTERMSIG(status);
      retval = kSignalBit + sig;
      fprintf(stderr, "Child got killed by signal %d\n", sig);
      break;
    }
  }

  if (retval != 0) {
    fprintf(stderr, "Include scanner received non-zero exit code from child\n");
    exit(EXIT_FAILURE);
  }

  /* copy files from unprefixed path */
  incl_setpos(&incl, 0);
  while ((path = incl_next(&incl)) != NULL) {
    char *const to = path;
    char *const from = to + prefix_len;
    if (!helper_copyfile(from, to, /*wopen_mayfail=*/1)) {
      if (helper_mkdir_p(to, 0755) != 0) {
        fprintf(stderr, "mkdir failed: %s\n", to);
        exit(EXIT_FAILURE);
      }
      if (!helper_copyfile(from, to, /*wopen_mayfail=*/0)) {
        fprintf(stderr, "copy failed: %s\n", to);
        exit(EXIT_FAILURE);
      }
    }
  }

  incl_free(&incl);

  return 0;
}

void parse_paths(struct IncludeList *incl, int fd, char const *pattern,
                 char const *prefix) {
  size_t path_len;
  size_t prefix_len = strlen(prefix);
  size_t pattern_len = strlen(pattern);

  struct ShiftBuffer sbuf;
  sbuf_init(&sbuf);

  while (sbuf_load(&sbuf, fd) || !sbuf_eof(&sbuf)) {
    do {
      char *pos = sbuf_pos(&sbuf);
      char *end = strchr(pos, ' ');
      if (end == NULL) { /* not found */
        if (!sbuf_shift(&sbuf)) {
          fprintf(stderr, "File path too long for buffer\n");
          exit(EXIT_FAILURE);
        }
        break; /* read next chunk in sbuf */
      }

      if (end >= sbuf_end(&sbuf)) { /* eof */
        end = sbuf_end(&sbuf) - 1;
      }

      /* match prefix */
      path_len = (size_t)end - (size_t)pos;
      if (path_len >= pattern_len && strncmp(pattern, pos, pattern_len) == 0) {
        size_t new_len = prefix_len + path_len + 1;
        char *new_path = (char *)calloc(new_len, sizeof(char));
        strcat(new_path, prefix);
        helper_normpath(new_path + prefix_len, pos, path_len);
        new_path[new_len - 1] = '\0';
        incl_append(incl, new_path);
      }
      sbuf_addpos(&sbuf, path_len + 1);
    } while (!sbuf_eof(&sbuf));
  }

  /* deduplicate */
  if (incl->size > 1) {
    size_t cmp_pos = 0;
    size_t next_pos = 1;
    if (incl->size > 2) {
      qsort(incl->list, incl->size, sizeof(char *), helper_pathcmp);
    }
    /* uniq */
    for (; next_pos < incl->size; ++next_pos) {
      if (strcmp(incl->list[cmp_pos], incl->list[next_pos]) != 0) {
        cmp_pos = next_pos;
      } else { /* remove duplicate */
        free(incl->list[next_pos]);
        incl->list[next_pos] = NULL;
      }
    }
    incl->size = cmp_pos + 1;
  }
}

int helper_mkdir_p(char *path, mode_t mode) {
  char *p = path;
  while ((p = strchr(p + 1, '/'))) {
    int ret;
    *p = '\0';
    ret = mkdir(path, mode);
    *p = '/';
    if (ret == -1 && errno != EEXIST)
      return -1;
  }
  return 0;
}

int helper_pathcmp(const void *a, const void *b) {
  return strcmp(*(const char **)a, *(const char **)b);
}

void helper_normpath(char *out_path, const char *in_path, size_t len) {
  char *out_pos = out_path;
  const char *in_pos = in_path;

  while (in_pos < in_path + len) {
    size_t seg_len;
    const char *seg_end = strchr(in_pos, '/');
    if (seg_end == NULL || seg_end > in_path + len - 1) {
      seg_end = in_path + len - 1; /* last valid char */
    }
    seg_len = (size_t)seg_end - (size_t)in_pos + 1;
    if (seg_len == 2 && strncmp(in_pos, "./", 2) == 0) {
      in_pos += 2;
    } else if (seg_len == 3 && strncmp(in_pos, "../", 3) == 0) {
      in_pos += 3;
      if (out_pos > out_path) {
        *(out_pos - 1) = '\0';
        out_pos = strrchr(out_path, '/') + 1;
      } else {
        out_pos = NULL;
      }
      if (out_pos == NULL) {
        fprintf(stderr, "Cannot normalize upwards path %s\n", in_path);
        exit(EXIT_FAILURE);
      }
    } else {
      assert(out_pos + seg_len <= out_path + len);
      strncpy(out_pos, in_pos, seg_len);
      out_pos += seg_len;
      in_pos += seg_len;
    }
  }
}

int helper_copyfile(const char *from, const char *to, int wopen_mayfail) {
  FILE *in, *out;
  char buf[READ_SIZE] = {'\0'};
  size_t size;

  out = fopen(to, "w");
  if (out == NULL) {
    if (errno == EEXIST) {
      return 1; /* ok */
    }
    if (!wopen_mayfail) {
      fprintf(stderr, "Cannot open output file for writing %s\n", to);
    }
    return 0;
  }

  in = fopen(from, "r");
  if (in == NULL) {
    fclose(out);
    fprintf(stderr, "Cannot open input file for reading %s\n", from);
    return 0;
  }

  while ((size = fread(buf, sizeof(*buf), sizeof(buf), in)) > 0) {
    if (size != fwrite(buf, sizeof(*buf), size, out)) {
      fclose(in);
      fclose(out);
      fprintf(stderr, "Failed writing output file %s\n", to);
      return 0;
    }
  }

  if (!feof(in)) {
    fclose(in);
    fclose(out);
    fprintf(stderr, "Failed reading input file %s\n", from);
    return 0;
  }

  fclose(in);
  fclose(out);

  return 1;
}

void sbuf_init(struct ShiftBuffer *sbuf) {
  memset(sbuf->buffer, '\0', sizeof(sbuf->buffer));
  sbuf->start = sbuf->buffer + READ_SIZE;
  sbuf->pos = sbuf->start;
  sbuf->size = 0;
}

void sbuf_addpos(struct ShiftBuffer *sbuf, size_t offset) {
  sbuf->pos += offset;
}

char *sbuf_pos(struct ShiftBuffer *sbuf) { return sbuf->pos; }

char *sbuf_end(struct ShiftBuffer *sbuf) { return sbuf->start + sbuf->size; }

int sbuf_eof(struct ShiftBuffer *sbuf) { return sbuf->pos >= sbuf_end(sbuf); }

int sbuf_load(struct ShiftBuffer *sbuf, int fd) {
  int buf_size = 0;
  int read_size = 0;
  char *buf_pos = sbuf->start;
  if (sbuf->pos >= sbuf_end(sbuf)) {
    sbuf->pos = sbuf->start;
  }
  assert(sbuf->pos <= sbuf->start);
  do {
    read_size = read(fd, buf_pos, READ_SIZE - buf_size);
    if (read_size < 0) {
      fprintf(stderr, "read fd failed\n");
      exit(EXIT_FAILURE);
    }
    buf_size += read_size;
    buf_pos += read_size;
  } while (buf_size < READ_SIZE && read_size != 0);
  sbuf->size = buf_size;
  if (read_size == 0) { /* finished, insert space to mark end of data */
    *(sbuf->start + sbuf->size) = ' ';
  }
  return buf_size;
}

int sbuf_shift(struct ShiftBuffer *sbuf) {
  if (!sbuf_eof(sbuf) && sbuf->pos >= sbuf->start) {
    size_t size = (size_t)sbuf_end(sbuf) - (size_t)sbuf->pos;
    char *new_pos = sbuf->start - size;
    strncpy(new_pos, sbuf->pos, size);
    sbuf->pos = new_pos;
    return 1;
  }
  return 0; /* cannot shift, still valid data in buffer's first half */
}

void incl_init(struct IncludeList *incl) {
  incl->list = (char **)calloc(LIST_SIZE + 1, sizeof(char *));
  incl->pos = incl->list;
  incl->size = 0;
  incl->capacity = LIST_SIZE;
}

void incl_free(struct IncludeList *incl) {
  char *path = NULL;
  incl_setpos(incl, 0);
  while ((path = incl_next(incl)) != NULL) {
    free(path);
  }
  free(incl->list);
}

void incl_resize(struct IncludeList *incl, size_t new_cap) {
  char **new_list = (char **)calloc(new_cap + 1, sizeof(char *));
  assert(new_cap > incl->capacity);
  memcpy(new_list, incl->list, sizeof(*incl->list) * incl->capacity);
  incl_free(incl);
  incl->list = new_list;
  incl->capacity = new_cap;
}

void incl_append(struct IncludeList *incl, char *path) {
  if (incl->size == incl->capacity) {
    incl_resize(incl, incl->capacity * 2);
  }
  incl->list[incl->size++] = path;
}

void incl_setpos(struct IncludeList *incl, size_t pos) {
  assert(pos <= incl->size);
  incl->pos = incl->list + pos;
}

char *incl_next(struct IncludeList *incl) {
  char **pos = incl->pos;
  char **end = incl->list + incl->size;
  assert(pos <= end);
  while (pos < end) {
    pos = incl->pos++;
    if (*pos != NULL) {
      return *pos;
    }
  }
  return NULL;
}
