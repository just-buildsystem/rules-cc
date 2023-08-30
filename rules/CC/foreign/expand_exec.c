/*
 * Copyright 2023 Huawei Cloud Computing Technology Co., Ltd.
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

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* usage: ./expand_exec [VARS...] -- ARGS... */
int main(int argc, const char *argv[]) {
  char **outv;
  const char **varv;
  int i, j, varc, sep = 0, retval = 0;
  for (i = 1; i < argc; ++i)
    if (strcmp(argv[i], "--") == 0) {
      sep = i;
      break;
    }
  varc = sep - 1;
  argc -= sep + 1;
  if (sep == 0 || argc < 1)
    return 1; /* error: missing sep or args */
  varv = &argv[1];
  argv = &argv[sep + 1];
  outv = (char **)calloc((size_t)(argc + 1), sizeof(char *));
  for (i = 0; i < argc; ++i) { /* iterate ARGS */
    const char *arg = argv[i];
    size_t arg_pos = 0, arg_len = strlen(arg);
    size_t out_pos = 0, out_len = arg_len;
    size_t str_pos = 0, str_len = 0;
    char *out = (char *)calloc((size_t)(out_len + 1), sizeof(char));
    for (; arg_pos < arg_len; ++arg_pos) {
      if (strncmp(&arg[arg_pos], "$(", 2) == 0) {
        const char *start = &arg[arg_pos + 2];
        const char *end = strchr(start, ')');
        if (end == NULL) {
          retval = 2; /* error: unterminated $(VAR) expression */
          free(out);
          goto cleanup;
        }
        for (j = 0; j < varc; ++j) { /* lookup VAR */
          const char *var = varv[j];
          size_t len_var = strlen(var);
          if ((size_t)(end - start) != len_var)
            continue;
          if (strncmp(&arg[arg_pos + 2], var, len_var) == 0) {
            size_t val_len, out_len_new;
            const char *val = getenv(var);
            if (val == NULL)
              val = "";
            val_len = strlen(val);
            out_len_new = out_pos + str_len + val_len;
            if (out_len_new > out_len) {
              out = (char *)realloc(out, out_len_new + 1);
              out_len = out_len_new;
            }
            strncat(out, &arg[str_pos], str_len); /* concat preceding substr */
            strncat(out, val, val_len);           /* concat variable value */
            arg_pos += len_var + 2;
            out_pos += str_len + val_len;
            str_pos = arg_pos + 1;
            str_len = 0;
            break;
          }
        }
        if (j != varc)
          continue; /* success */
      }
      ++str_len;
    }
    if (str_len > 0) {
      if (out_pos + str_len > out_len) {
        out = (char *)realloc(out, out_pos + str_len + 1);
      }
      strncat(out, &arg[str_pos], str_len);
    }
    outv[i] = out;
  }
  execvp(outv[0], outv);
  retval = 3; /* error: exec failed */
cleanup:
  for (i = 0; i < argc; ++i)
    if (outv[i] != NULL)
      free(outv[i]);
  free(outv);
  return retval;
}
