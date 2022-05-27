#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

int main(int argc, char **argv)
{
  if (argc != 3)
  {
    printf("Use: %s <content.db3> <forum_id>", argv[0]);
    return 0;
  }

  int forum_id = strtol(argv[2], NULL, 10);

  parser_init(argv[1]);
  parser_get_by_forum_id(forum_id);
  parser_finalize();

  return 0;
}