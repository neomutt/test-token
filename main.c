#include "config.h"
#include "mutt/mutt.h"
#include "token.h"

int main(int argc, char *argv[])
{
  if (argc < 2)
    return 1;

  struct Buffer *dest = mutt_buffer_alloc(128);
  struct Buffer *tok = mutt_buffer_from(argv[1]);
  tok->dptr = tok->data;

  TokenFlags flags = MUTT_TOKEN_NO_FLAGS;

  do
  {
    mutt_extract_token(dest, tok, flags);
    printf(">>>%s<<<\n", dest->data);
  } while (MoreArgs(tok));

  mutt_buffer_free(&dest);
  mutt_buffer_free(&tok);
  return 0;
}
