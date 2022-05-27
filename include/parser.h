#ifndef PARSER_H_
#define PARSER_H_

int parser_init(char *filename);
int parser_get_all_content();
int parser_get_by_forum_id(int forum_id);
int parser_finalize();

#endif