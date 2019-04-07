#include "config.h"
#include "mutt/mutt.h"
#include "token.h"

int main(int argc, char *argv[])
{
  struct Buffer *dest = mutt_buffer_alloc(256);
  struct Buffer *tok = mutt_buffer_alloc(256);

  TokenFlags flags = MUTT_TOKEN_NO_FLAGS;

  for (argc--; argc > 0; argc--, argv++)
  {
    printf("Source: %s\n", argv[1]);
    printf("Flags:  %d\n", flags);

    mutt_buffer_reset(dest);
    mutt_buffer_strcpy(tok, argv[1]);
    tok->dptr = tok->data;

    while (MoreArgs(tok))
    {
      mutt_extract_token(dest, tok, flags);
      printf("    >>>%s<<<\n", dest->data);
    }

    printf("\n");
  }

  mutt_buffer_free(&dest);
  mutt_buffer_free(&tok);
  return 0;
}
