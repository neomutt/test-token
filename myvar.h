#ifndef MUTT_MYVAR_H
#define MUTT_MYVAR_H

#include "mutt/mutt.h"

struct MyVar
{
  char *name;
  char *value;
  TAILQ_ENTRY(MyVar) entries;
};
TAILQ_HEAD(MyVarHead, MyVar);

extern struct MyVarHead MyVars; ///< List of all the user's custom config variables

void        myvar_del(const char *var);
const char *myvar_get(const char *var);
void        myvar_set(const char *var, const char *val);

#endif /* MUTT_MYVAR_H */
