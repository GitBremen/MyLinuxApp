#include "sqlite.h"

char *currentTime = NULL;
char sqltime[32] = {0};

static int time_callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    if (argc > 0 && argv[0] != NULL)
    {
        strcpy(sqltime, argv[0]);
    }
    return 0;
}

static void sqlite_getTime(sqlite3 *db)
{
    int rc;
    char *zErrMsg = 0;
    char *sql_getTime = "SELECT datetime('now');";
    rc = sqlite3_exec(db, sql_getTime, time_callback, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return;
    }
}

void sqlite_Init(void)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char *sql = NULL;
    rc = sqlite3_open(SQLITE_DB, &db);
    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return;
    }
    sql = "CREATE TABLE IF NOT EXISTS warehouse("
          "TEMPERATURE           REAL    NOT NULL,"
          "ILLUMINATION          INT     NOT NULL,"
          "SAMPLETIME            TEXT    NOT NULL);";
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return;
    }
    sqlite3_close(db);
}

void sqlite_insert(struct storage_info *warehouse, sqlite3 *db)
{
    char *zErrMsg = 0;
    int rc;
    char *sql = NULL;
    /*给sql分配内存*/
    sql = (char *)malloc(256);
    memset(sql, 0, sizeof(sql));
    if (sql == NULL)
    {
        fprintf(stderr, "malloc failed\n");
        return;
    }

    sqlite_getTime(db);
    sprintf(sql, "INSERT INTO warehouse (TEMPERATURE,ILLUMINATION,SAMPLETIME) "
                 "VALUES (%f, %d,'%s');",
            warehouse->temperature, warehouse->illumination, sqltime);
    rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
        return;
    }
    free(sql);
}

void *pthread_sqlite(void *args)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;

    rc = sqlite3_open(SQLITE_DB, &db);

    if (rc)
    {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_free(zErrMsg);
        pthread_exit(NULL);
    }
    sqlite_insert(warehouse, db);

    sqlite3_close(db);
    pthread_exit(NULL);
}
