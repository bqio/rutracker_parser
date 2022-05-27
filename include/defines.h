#ifndef PARSER_INIT_OK
#define PARSER_INIT_OK (0)
#endif

#ifndef PARSER_INIT_ERROR
#define PARSER_INIT_ERROR (-1)
#endif

#ifndef PARSER_SQL_GET_ALL_CONTENT
#define PARSER_SQL_GET_ALL_CONTENT "SELECT * FROM contents LIMIT 5;"
#endif

#ifndef PARSER_SQL_GET_BY_FORUM_ID
#define PARSER_SQL_GET_BY_FORUM_ID "SELECT * FROM torrent t INNER JOIN contents c ON t.file_id = c.tid INNER JOIN files f ON c.tid = f.tid WHERE forum_id = (%i)"
#endif