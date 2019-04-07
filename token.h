#ifndef MUTT_TOKEN_H
#define MUTT_TOKEN_H

struct Buffer;

typedef uint16_t TokenFlags;               ///< Flags for mutt_extract_token(), e.g. #MUTT_TOKEN_EQUAL
#define MUTT_TOKEN_NO_FLAGS            0   ///< No flags are set
#define MUTT_TOKEN_EQUAL         (1 << 0)  ///< Treat '=' as a special
#define MUTT_TOKEN_CONDENSE      (1 << 1)  ///< ^(char) to control chars (macros)
#define MUTT_TOKEN_SPACE         (1 << 2)  ///< Don't treat whitespace as a term
#define MUTT_TOKEN_QUOTE         (1 << 3)  ///< Don't interpret quotes
#define MUTT_TOKEN_PATTERN       (1 << 4)  ///< !)|~ are terms (for patterns)
#define MUTT_TOKEN_COMMENT       (1 << 5)  ///< Don't reap comments
#define MUTT_TOKEN_SEMICOLON     (1 << 6)  ///< Don't treat ; as special
#define MUTT_TOKEN_BACKTICK_VARS (1 << 7)  ///< Expand variables within backticks
#define MUTT_TOKEN_NOSHELL       (1 << 8)  ///< Don't expand environment variables
#define MUTT_TOKEN_QUESTION      (1 << 9)  ///< Treat '?' as a special

int mutt_extract_token(struct Buffer *dest, struct Buffer *tok, TokenFlags flags);

#endif /* MUTT_TOKEN_H */
