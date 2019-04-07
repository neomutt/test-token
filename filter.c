#include "config.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mutt/mutt.h"
#include "mutt.h"
#include "filter.h"
#include "window.h"

/**
 * mutt_create_filter - Set up filter program
 * @param[in]  s      Command string
 * @param[out] fp_in  FILE pointer of stdin
 * @param[out] fp_out FILE pointer of stdout
 * @param[out] fp_err FILE pointer of stderr
 * @retval num PID of filter
 */
pid_t mutt_create_filter(const char *s, FILE **fp_in, FILE **fp_out, FILE **fp_err)
{
  return mutt_create_filter_fd(s, fp_in, fp_out, fp_err, -1, -1, -1);
}

/**
 * mutt_wait_filter - Wait for the exit of a process and return its status
 * @param pid Process id of the process to wait for
 * @retval num Exit status of the process identified by pid
 * @retval -1  Error
 */
int mutt_wait_filter(pid_t pid)
{
  int rc;

  waitpid(pid, &rc, 0);
  mutt_sig_unblock_system(true);
  rc = WIFEXITED(rc) ? WEXITSTATUS(rc) : -1;

  return rc;
}

/**
 * mutt_create_filter_fd - Run a command on a pipe (optionally connect stdin/stdout)
 * @param[in]  cmd    Command line to invoke using `sh -c`
 * @param[out] fp_in  File stream pointing to stdin for the command process, can be NULL
 * @param[out] fp_out File stream pointing to stdout for the command process, can be NULL
 * @param[out] fp_err File stream pointing to stderr for the command process, can be NULL
 * @param[in]  fdin   If `in` is NULL and fdin is not -1 then fdin will be used as stdin for the command process
 * @param[in]  fdout  If `out` is NULL and fdout is not -1 then fdout will be used as stdout for the command process
 * @param[in]  fderr  If `error` is NULL and fderr is not -1 then fderr will be used as stderr for the command process
 * @retval num PID of the created process
 * @retval -1  Error creating pipes or forking
 *
 * This function provides multiple mechanisms to handle IO sharing for the
 * command process. File streams are prioritized over file descriptors if
 * present.
 *
 * @code{.c}
 *    mutt_create_filter_fd(commandline, NULL, NULL, NULL, -1, -1, -1);
 * @endcode
 *
 * Additionally, fp_in, fp_out, and fp_err will point to FILE* streams
 * representing the processes stdin, stdout, and stderr.
 */
pid_t mutt_create_filter_fd(const char *cmd, FILE **fp_in, FILE **fp_out,
                            FILE **fp_err, int fdin, int fdout, int fderr)
{
  int pin[2], pout[2], perr[2], pid;

  if (fp_in)
  {
    *fp_in = NULL;
    if (pipe(pin) == -1)
      return -1;
  }

  if (fp_out)
  {
    *fp_out = NULL;
    if (pipe(pout) == -1)
    {
      if (fp_in)
      {
        close(pin[0]);
        close(pin[1]);
      }
      return -1;
    }
  }

  if (fp_err)
  {
    *fp_err = NULL;
    if (pipe(perr) == -1)
    {
      if (fp_in)
      {
        close(pin[0]);
        close(pin[1]);
      }
      if (fp_out)
      {
        close(pout[0]);
        close(pout[1]);
      }
      return -1;
    }
  }

  mutt_sig_block_system();

  pid = fork();
  if (pid == 0)
  {
    mutt_sig_unblock_system(false);

    if (fp_in)
    {
      close(pin[1]);
      dup2(pin[0], 0);
      close(pin[0]);
    }
    else if (fdin != -1)
    {
      dup2(fdin, 0);
      close(fdin);
    }

    if (fp_out)
    {
      close(pout[0]);
      dup2(pout[1], 1);
      close(pout[1]);
    }
    else if (fdout != -1)
    {
      dup2(fdout, 1);
      close(fdout);
    }

    if (fp_err)
    {
      close(perr[0]);
      dup2(perr[1], 2);
      close(perr[1]);
    }
    else if (fderr != -1)
    {
      dup2(fderr, 2);
      close(fderr);
    }

    if (MuttIndexWindow && (MuttIndexWindow->cols > 0))
    {
      char columns[11];
      snprintf(columns, sizeof(columns), "%d", MuttIndexWindow->cols);
      mutt_envlist_set("COLUMNS", columns, 1);
    }

    execle(EXECSHELL, "sh", "-c", cmd, NULL, mutt_envlist_getlist());
    _exit(127);
  }
  else if (pid == -1)
  {
    mutt_sig_unblock_system(true);

    if (fp_in)
    {
      close(pin[0]);
      close(pin[1]);
    }

    if (fp_out)
    {
      close(pout[0]);
      close(pout[1]);
    }

    if (fp_err)
    {
      close(perr[0]);
      close(perr[1]);
    }

    return -1;
  }

  if (fp_out)
  {
    close(pout[1]);
    *fp_out = fdopen(pout[0], "r");
  }

  if (fp_in)
  {
    close(pin[0]);
    *fp_in = fdopen(pin[1], "w");
  }

  if (fp_err)
  {
    close(perr[1]);
    *fp_err = fdopen(perr[0], "r");
  }

  return pid;
}

