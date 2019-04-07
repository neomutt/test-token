#ifndef MUTT_WINDOW_H
#define MUTT_WINDOW_H

/**
 * struct MuttWindow - A division of the screen
 *
 * Windows for different parts of the screen
 */
struct MuttWindow
{
  int rows;
  int cols;
  int row_offset;
  int col_offset;
};

extern struct MuttWindow *MuttIndexWindow;

#endif /* MUTT_WINDOW_H */
