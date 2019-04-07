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

- **`MUTT_TOKEN_EQUAL`** - Treat '=' as a special
  - `parse_set()`
  - `parse_setenv()`

- **`MUTT_TOKEN_NOSHELL`** - Don't expand environment variables
  - Recursive call to `mutt_extract_token()`

- **`MUTT_TOKEN_PATTERN`** - `!)|~` are terms (for patterns)
  - `pattern.eat_regex()`
  - `pattern.eat_query()`
  - `pattern.eat_date()`

- **`MUTT_TOKEN_QUESTION`** - Treat `?` as a special
  - `parse_set()`

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

