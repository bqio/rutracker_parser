#include <stdio.h>
#include <sqlite3.h>
#include <zlib.h>
#include <malloc.h>
#include <jansson.h>

#include "defines.h"
#include "parser.h"

static sqlite3 *g_db_handle = (sqlite3 *)NULL;
static sqlite3_stmt *g_db_stmt = (sqlite3_stmt *)NULL;

int parser_init(char *filename)
{
  if (sqlite3_open(filename, &g_db_handle))
  {
    printf("sqlite3_open failed: %s\n", sqlite3_errmsg(g_db_handle));
    return PARSER_INIT_ERROR;
  }

  return PARSER_INIT_OK;
}

int parser_get_all_content()
{
  int rc;
  char *db_query = sqlite3_mprintf(PARSER_SQL_GET_ALL_CONTENT);

  sqlite3_reset(g_db_stmt);

  rc = sqlite3_prepare_v2(g_db_handle, db_query, -1, &g_db_stmt, NULL);

  sqlite3_free(db_query);

  if (rc != SQLITE_OK)
  {
    printf("sqlite3_prepare_v2 failed: %s\n", sqlite3_errmsg(g_db_handle));
    return -1;
  }

  while (sqlite3_step(g_db_stmt) == SQLITE_ROW)
  {
    const unsigned char *topic_id = sqlite3_column_text(g_db_stmt, 0);
    const void *topic_blob = sqlite3_column_blob(g_db_stmt, 1);
    int blob_size = sqlite3_column_bytes(g_db_stmt, 1);

    uLongf blob_dest_size = 10000 * sizeof(int);
    Bytef *blob_dest = calloc(blob_dest_size, 1);

    uncompress(blob_dest, &blob_dest_size, (const Bytef *)topic_blob, (uLongf)blob_size);

    FILE *fptr;

    fptr = fopen(sqlite3_mprintf("data/out/%s.txt", (const char *)topic_id), "wb");
    fwrite(blob_dest, blob_dest_size, 1, fptr);
    fclose(fptr);

    printf("Topic ID: %s; Blob Size: %i\n", topic_id, blob_size);
  }

  sqlite3_reset(g_db_stmt);

  return 0;
}

int parser_get_by_forum_id(int forum_id)
{
  int rc;
  char *db_query = sqlite3_mprintf(PARSER_SQL_GET_BY_FORUM_ID, forum_id);

  sqlite3_reset(g_db_stmt);

  rc = sqlite3_prepare_v2(g_db_handle, db_query, -1, &g_db_stmt, NULL);

  sqlite3_free(db_query);

  if (rc != SQLITE_OK)
  {
    printf("sqlite3_prepare_v2 failed: %s\n", sqlite3_errmsg(g_db_handle));
    return -1;
  }

  int count = 0;

  json_t *json_root = json_object();
  json_t *json_arr = json_array();

  while (sqlite3_step(g_db_stmt) == SQLITE_ROW)
  {
    count++;
    const unsigned char *title = sqlite3_column_text(g_db_stmt, 2);
    const unsigned char *hash = sqlite3_column_text(g_db_stmt, 1);

    const void *content_blob = sqlite3_column_blob(g_db_stmt, 7);
    int content_blob_size = sqlite3_column_bytes(g_db_stmt, 7);

    const void *files_blob = sqlite3_column_blob(g_db_stmt, 9);
    int files_blob_size = sqlite3_column_bytes(g_db_stmt, 9);

    uLongf blob_dest_size = 10000 * sizeof(int);

    Bytef *content_blob_dest = calloc(blob_dest_size, 1);
    Bytef *files_blob_dest = calloc(blob_dest_size, 1);

    uncompress(content_blob_dest, &blob_dest_size, (const Bytef *)content_blob, (uLongf)content_blob_size);
    uncompress(files_blob_dest, &blob_dest_size, (const Bytef *)files_blob, (uLongf)files_blob_size);

    json_t *json_obj = json_object();

    json_object_set(json_obj, "title", json_string((const char *)title));
    json_object_set(json_obj, "hash", json_string((const char *)hash));
    json_object_set(json_obj, "content", json_string((const char *)content_blob_dest));
    json_object_set(json_obj, "files", json_string((const char *)files_blob_dest));

    json_array_append(json_arr, json_obj);

    printf("%s\n", title);
  }

  printf("Total: %i", count);

  json_object_set(json_root, "titles", json_arr);

  json_dump_file(json_arr, sqlite3_mprintf("forum_%i.json", forum_id), JSON_INDENT(2));

  sqlite3_reset(g_db_stmt);

  return 0;
}

int parser_finalize()
{
  sqlite3_close(g_db_handle);

  return 0;
}