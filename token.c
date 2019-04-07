#include "config.h"
#include <string.h>
#include "mutt/mutt.h"
#include "config/lib.h"
#include "token.h"
#include "myvar.h"
#include "vars.h"
#include "filter.h"

/**
 * mutt_extract_token - Extract one token from a string
 * @param dest  Buffer for the result
 * @param tok   Buffer containing tokens
 * @param flags Flags, see #TokenFlags
 * @retval  0 Success
 * @retval -1 Error
 */
int mutt_extract_token(struct Buffer *dest, struct Buffer *tok, TokenFlags flags)
{
  if (!dest || !tok)
    return -1;

  char ch;
  char qc = 0; /* quote char */
  char *pc = NULL;

  /* reset the destination pointer to the beginning of the buffer */
  dest->dptr = dest->data;

  SKIPWS(tok->dptr);
  while ((ch = *tok->dptr))
  {
    if (!qc)
    {
      if ((ISSPACE(ch) && !(flags & MUTT_TOKEN_SPACE)) ||
          ((ch == '#') && !(flags & MUTT_TOKEN_COMMENT)) ||
          ((ch == '=') && (flags & MUTT_TOKEN_EQUAL)) ||
          ((ch == '?') && (flags & MUTT_TOKEN_QUESTION)) ||
          ((ch == ';') && !(flags & MUTT_TOKEN_SEMICOLON)) ||
          ((flags & MUTT_TOKEN_PATTERN) && strchr("~%=!|", ch)))
      {
        break;
      }
    }

    tok->dptr++;

    if (ch == qc)
      qc = 0; /* end of quote */
    else if (!qc && ((ch == '\'') || (ch == '"')) && !(flags & MUTT_TOKEN_QUOTE))
      qc = ch;
    else if ((ch == '\\') && (qc != '\''))
    {
      if (!*tok->dptr)
        return -1; /* premature end of token */
      switch (ch = *tok->dptr++)
      {
        case 'c':
        case 'C':
          if (!*tok->dptr)
            return -1; /* premature end of token */
          mutt_buffer_addch(dest, (toupper((unsigned char) *tok->dptr) - '@') & 0x7f);
          tok->dptr++;
          break;
        case 'e':
          mutt_buffer_addch(dest, '\033');
          break;
        case 'f':
          mutt_buffer_addch(dest, '\f');
          break;
        case 'n':
          mutt_buffer_addch(dest, '\n');
          break;
        case 'r':
          mutt_buffer_addch(dest, '\r');
          break;
        case 't':
          mutt_buffer_addch(dest, '\t');
          break;
        default:
          if (isdigit((unsigned char) ch) && isdigit((unsigned char) *tok->dptr) &&
              isdigit((unsigned char) *(tok->dptr + 1)))
          {
            mutt_buffer_addch(dest, (ch << 6) + (*tok->dptr << 3) + *(tok->dptr + 1) - 3504);
            tok->dptr += 2;
          }
          else
            mutt_buffer_addch(dest, ch);
      }
    }
    else if ((ch == '^') && (flags & MUTT_TOKEN_CONDENSE))
    {
      if (!*tok->dptr)
        return -1; /* premature end of token */
      ch = *tok->dptr++;
      if (ch == '^')
        mutt_buffer_addch(dest, ch);
      else if (ch == '[')
        mutt_buffer_addch(dest, '\033');
      else if (isalpha((unsigned char) ch))
        mutt_buffer_addch(dest, toupper((unsigned char) ch) - '@');
      else
      {
        mutt_buffer_addch(dest, '^');
        mutt_buffer_addch(dest, ch);
      }
    }
    else if ((ch == '`') && (!qc || (qc == '"')))
    {
      FILE *fp = NULL;
      pid_t pid;
      char *ptr = NULL;
      size_t expnlen;
      struct Buffer expn;
      int line = 0;

      pc = tok->dptr;
      do
      {
        pc = strpbrk(pc, "\\`");
        if (pc)
        {
          /* skip any quoted chars */
          if (*pc == '\\')
            pc += 2;
        }
      } while (pc && *pc != '`');
      if (!pc)
      {
        mutt_debug(LL_DEBUG1, "mismatched backticks\n");
        return -1;
      }
      struct Buffer cmd;
      mutt_buffer_init(&cmd);
      *pc = '\0';
      if (flags & MUTT_TOKEN_BACKTICK_VARS)
      {
        /* recursively extract tokens to interpolate variables */
        mutt_extract_token(&cmd, tok,
                           MUTT_TOKEN_QUOTE | MUTT_TOKEN_SPACE | MUTT_TOKEN_COMMENT |
                               MUTT_TOKEN_SEMICOLON | MUTT_TOKEN_NOSHELL);
      }
      else
      {
        cmd.data = mutt_str_strdup(tok->dptr);
      }
      *pc = '`';
      pid = mutt_create_filter(cmd.data, NULL, &fp, NULL);
      if (pid < 0)
      {
        mutt_debug(LL_DEBUG1, "unable to fork command: %s\n", cmd);
        FREE(&cmd.data);
        return -1;
      }
      FREE(&cmd.data);

      tok->dptr = pc + 1;

      /* read line */
      mutt_buffer_init(&expn);
      expn.data = mutt_file_read_line(NULL, &expn.dsize, fp, &line, 0);
      mutt_file_fclose(&fp);
      mutt_wait_filter(pid);

      /* if we got output, make a new string consisting of the shell output
       * plus whatever else was left on the original line */
      /* BUT: If this is inside a quoted string, directly add output to
       * the token */
      if (expn.data && qc)
      {
        mutt_buffer_addstr(dest, expn.data);
        FREE(&expn.data);
      }
      else if (expn.data)
      {
        expnlen = mutt_str_strlen(expn.data);
        tok->dsize = expnlen + mutt_str_strlen(tok->dptr) + 1;
        ptr = mutt_mem_malloc(tok->dsize);
        memcpy(ptr, expn.data, expnlen);
        strcpy(ptr + expnlen, tok->dptr);
        if (tok->destroy)
          FREE(&tok->data);
        tok->data = ptr;
        tok->dptr = ptr;
        tok->destroy = 1; /* mark that the caller should destroy this data */
        ptr = NULL;
        FREE(&expn.data);
      }
    }
    else if ((ch == '$') && (!qc || (qc == '"')) &&
             ((*tok->dptr == '{') || isalpha((unsigned char) *tok->dptr)))
    {
      const char *env = NULL;
      char *var = NULL;

      if (*tok->dptr == '{')
      {
        pc = strchr(tok->dptr, '}');
        if (pc)
        {
          var = mutt_str_substr_dup(tok->dptr + 1, pc);
          tok->dptr = pc + 1;

          if ((flags & MUTT_TOKEN_NOSHELL))
          {
            mutt_buffer_addch(dest, ch);
            mutt_buffer_addch(dest, '{');
            mutt_buffer_addstr(dest, var);
            mutt_buffer_addch(dest, '}');
            FREE(&var);
          }
        }
      }
      else
      {
        for (pc = tok->dptr; isalnum((unsigned char) *pc) || *pc == '_'; pc++)
          ;
        var = mutt_str_substr_dup(tok->dptr, pc);
        tok->dptr = pc;
      }
      if (var)
      {
        struct Buffer result;
        mutt_buffer_init(&result);
        int rc = cs_str_string_get(Config, var, &result);

        if (CSR_RESULT(rc) == CSR_SUCCESS)
        {
          mutt_buffer_addstr(dest, result.data);
          FREE(&result.data);
        }
        else if ((env = myvar_get(var)))
        {
          mutt_buffer_addstr(dest, env);
        }
        else if (!(flags & MUTT_TOKEN_NOSHELL) && (env = mutt_str_getenv(var)))
        {
          mutt_buffer_addstr(dest, env);
        }
        else
        {
          mutt_buffer_addch(dest, ch);
          mutt_buffer_addstr(dest, var);
        }
        FREE(&var);
      }
    }
    else
      mutt_buffer_addch(dest, ch);
  }
  mutt_buffer_addch(dest, 0); /* terminate the string */
  SKIPWS(tok->dptr);
  return 0;
}

