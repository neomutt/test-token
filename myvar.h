/**
 * @file
 * Handling of personal config ('my' variables)
 *
 * @authors
 * Copyright (C) 2018 Richard Russon <rich@flatcap.org>
 *
 * @copyright
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 2 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MUTT_MYVAR_H
#define MUTT_MYVAR_H

#include "mutt/mutt.h"

/**
 * struct MyVar - A user-set variable
 */
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
