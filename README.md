# Test code to split up mutt_extract_token()

## Build

- It expects NeoMutt to be built in `../neo`
  - `make`
  - `make test`

## Notes

- Eliminate dependence on MuttWindow
  1. Duplicate environment
  2. Set `COLUMNS`
  3. Pass to exec

Won't work.  We'd need to pass the dupe environment to the token function.
exec() needs to happen outside of token function.

- Move `filter.c` to libmutt?
  - Factor out environment
  - Factor out `EXECSHELL`
  - Do we need both `fd` and `FILE` versions?

- Eliminate myvar?
  - Fold myvar support into libconfig?
  - Handle is still held by NeoMutt

## Option Behaviour and Callers

- **`MUTT_TOKEN_BACKTICK_VARS`** - Expand variables within backticks
  - Contents are executed in a shell (`EXECSHELL`)
  - `parse_set()`

- **`MUTT_TOKEN_COMMENT`** - Don't reap comments
  - `pattern.eat_regex()`
  - `pattern.eat_query()`
  - `pattern.eat_date()`

- **`MUTT_TOKEN_CONDENSE`** - `^(char)` to control chars (macros)
  - `mutt_parse_push()`
  - `mutt_parse_macro()`

I think this should be happening after token, in the caller.

- **`MUTT_TOKEN_EQUAL`** - Treat '=' as a special
  - `parse_set()`
  - `parse_setenv()`

- **`MUTT_TOKEN_NOSHELL`** - Don't expand environment variables
  - Recursive call to `mutt_extract_token()`

- **`MUTT_TOKEN_PATTERN`** - `~%=!|` are terms (for patterns)
  - `pattern.eat_regex()`
  - `pattern.eat_query()`
  - `pattern.eat_date()`

These are all non-whitespace.  Do we need special cases for them?

- **`MUTT_TOKEN_QUESTION`** - Treat `?` as a special
  - `parse_set()`

For ':set ?var' or ':set var?'
Can't this be dealt with by the caller?

- **`MUTT_TOKEN_QUOTE`** - Don't interpret quotes
  - `parse_alias()`
  - `parse_my_hdr()`

- **`MUTT_TOKEN_SEMICOLON`** - Don't treat `;` as special
  - `parse_alias()`

- **`MUTT_TOKEN_SPACE`** - Don't treat whitespace as a term
  - `parse_alias()`
  - `parse_ifdef()`
  - `parse_my_hdr()`
  - `mutt_parse_hook()`

- **`MUTT_TOKEN_NO_FLAGS`** - Default behaviour
  - `do_uncolor()`
  - `icmd_bind()`
  - `mutt_expando_format()`
  - `mutt_lua_source_file()`
  - `mutt_parse_bind()`
  - `mutt_parse_exec()`
  - `mutt_parse_hook()`
  - `mutt_parse_icommand()`
  - `mutt_parse_rc_line()`
  - `mutt_parse_score()`
  - `mutt_parse_unbind()`
  - `mutt_parse_unhook()`
  - `mutt_parse_unscore()`
  - `parse_alias()`
  - `parse_alternates()`
  - `parse_attachments()`
  - `parse_attach_list()`
  - `parse_attr_spec()`
  - `parse_color()`
  - `parse_color_pair()`
  - `parse_echo()`
  - `parse_group()`
  - `parse_grouplist()`
  - `parse_ifdef()`
  - `parse_ignore()`
  - `parse_keymap()`
  - `parse_lists()`
  - `parse_mailboxes()`
  - `parse_object()`
  - `parse_path_list()`
  - `parse_path_unlist()`
  - `parse_replace_list()`
  - `parse_setenv()`
  - `parse_source()`
  - `parse_spam_list()`
  - `parse_stailq()`
  - `parse_subscribe()`
  - `parse_subscribe_to()`
  - `parse_tag_formats()`
  - `parse_tag_transforms()`
  - `parse_unalias()`
  - `parse_unalternates()`
  - `parse_unattachments()`
  - `parse_unattach_list()`
  - `parse_uncolor()`
  - `parse_unignore()`
  - `parse_unlists()`
  - `parse_unmailboxes()`
  - `parse_unmy_hdr()`
  - `parse_unreplace_list()`
  - `parse_unstailq()`
  - `parse_unsubscribe()`
  - `parse_unsubscribe_from()`

