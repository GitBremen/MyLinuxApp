#ifndef __SQLITE_H
#define __SQLITE_H

#include <sqlite3.h>
#include "warehouse.h"

#define SQLITE_DB "/root/warehouse.db"
void sqlite_Init(void);
void *pthread_sqlite(void *args);

#endif
