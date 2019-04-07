#ifndef MUTT_FILTER_H
#define MUTT_FILTER_H

#include <stdio.h>
#include <unistd.h>

pid_t mutt_create_filter_fd(const char *cmd, FILE **fp_in, FILE **fp_out, FILE **fp_err, int fdin, int fdout, int fderr);
pid_t mutt_create_filter(const char *s, FILE **fp_in, FILE **fp_out, FILE **fp_err);
int mutt_wait_filter(pid_t pid);

#endif /* MUTT_FILTER_H */
