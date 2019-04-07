#include "config.h"
#include <stddef.h>
#include "mutt/mutt.h"
#include "myvar.h"

struct MyVarHead MyVars = TAILQ_HEAD_INITIALIZER(MyVars);

const char *myvar_get(const char *var)
{
  struct MyVar *myv = NULL;

  TAILQ_FOREACH(myv, &MyVars, entries)
  {
    if (mutt_str_strcmp(myv->name, var) == 0)
      return NONULL(myv->value);
  }

  return NULL;
}

void myvar_set(const char *var, const char *val)
{
  struct MyVar *myv = NULL;

  TAILQ_FOREACH(myv, &MyVars, entries)
  {
    if (mutt_str_strcmp(myv->name, var) == 0)
    {
      mutt_str_replace(&myv->value, val);
      return;
    }
  }

  myv = mutt_mem_calloc(1, sizeof(struct MyVar));
  myv->name = mutt_str_strdup(var);
  myv->value = mutt_str_strdup(val);
  TAILQ_INSERT_TAIL(&MyVars, myv, entries);
}

void myvar_del(const char *var)
{
  struct MyVar *myv = NULL;

  TAILQ_FOREACH(myv, &MyVars, entries)
  {
    if (mutt_str_strcmp(myv->name, var) == 0)
    {
      TAILQ_REMOVE(&MyVars, myv, entries);
      FREE(&myv->name);
      FREE(&myv->value);
      FREE(&myv);
      return;
    }
  }
}
