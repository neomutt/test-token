#include "config.h"
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "mutt/mutt.h"
#include "mutt.h"
#include "filter.h"
#include "window.h"

pid_t mutt_create_filter(const char *s, FILE **fp_in, FILE **fp_out, FILE **fp_err)
{
  return mutt_create_filter_fd(s, fp_in, fp_out, fp_err, -1, -1, -1);
}

int mutt_wait_filter(pid_t pid)
{
  int rc;

  waitpid(pid, &rc, 0);
  mutt_sig_unblock_system(true);
  rc = WIFEXITED(rc) ? WEXITSTATUS(rc) : -1;

  return rc;
}

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
