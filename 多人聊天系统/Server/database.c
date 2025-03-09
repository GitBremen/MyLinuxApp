#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

/* 判断用户是否存在 */
int userIsExist(const char * name)
{
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql = "select count(*) from userInfo where username = '%s';";

    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, name);

    printf("requestSql = %s\n", requestSql); 
    
    char ** result = NULL;  /* 结果集 */
    int row = 0;            /* 行 */
    int column = 0;         /* 列 */
    sqlite3_get_table(m_base, requestSql, &result, &row, &column, NULL);
    
    /* 真正的结果在result[1] */
    int isExist = atoi(result[1]);

    /* 关闭数据库 */
    sqlite3_close(m_base);
    
    return isExist;
}

/* 将用户信息写到数据库中 */
int saveUserInfo(const char * name, const char * passwd)
{
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql = "insert into userInfo (username, passwd) values ('%s', '%s');";

    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, name, passwd);

    printf("requestSql = %s\n", requestSql); 

    /* 执行sql语句  */
    sqlite3_exec(m_base, requestSql, NULL, NULL, NULL);

    /* 关闭数据库 */
    sqlite3_close(m_base);
}

/* 用户名和密码是否匹配 */
int userMatchPasswd(const char * name, const char * passwd)
{
    /* todo... */
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql = "select count(*) from userInfo where username = '%s' and passwd = '%s';";

    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, name, passwd);

    printf("requestSql = %s\n", requestSql); 
    
    char ** result = NULL;  /* 结果集 */
    int row = 0;            /* 行 */
    int column = 0;         /* 列 */
    sqlite3_get_table(m_base, requestSql, &result, &row, &column, NULL);
    
    /* 真正的结果在result[1] */
    int isMatch = atoi(result[1]);

    /* 关闭数据库 */
    sqlite3_close(m_base);

    return isMatch;
}


/* 维护用户名和通信句柄的映射 */
int saveUserMapConnfd(const char * name, int connfd)
{
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql = "insert into mapInfo (username, connfd) values ('%s', '%d');";

    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, name, connfd);

    printf("requestSql = %s\n", requestSql); 

    /* 执行sql语句  */
    sqlite3_exec(m_base, requestSql, NULL, NULL, NULL);

    /* 关闭数据库 */
    sqlite3_close(m_base);
}

/* 用户下线映射处理 */
int delUserMapConnfd(int connfd)
{
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql  = "delete from mapInfo where connfd = '%d';";
    
    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, connfd);

    printf("requestSql = %s\n", requestSql);

    /* 执行sql语句  */
    sqlite3_exec(m_base, requestSql, NULL, NULL, NULL);

    /* 关闭数据库 */
    sqlite3_close(m_base);
}

/* 根据用户名获取通信句柄 */
int accordCUserNameGetConnfd(const char * name)
{
    /* 绑定数据库 */
    sqlite3 * m_base = NULL;
    sqlite3_open("./base.db", &m_base);

    const char * sql  = "select connfd from mapInfo where username = '%s';";

    /* 格式化字符串 */
    char requestSql[1024] = { 0 };
    sprintf(requestSql, sql, name);

    printf("requestSql = %s\n", requestSql); 
    
    char ** result = NULL;  /* 结果集 */
    int row = 0;            /* 行 */
    int column = 0;         /* 列 */
    sqlite3_get_table(m_base, requestSql, &result, &row, &column, NULL);
    
    /* 真正的结果在result[1] */
    int connfd = atoi(result[1]);
    
    /* 关闭数据库 */
    sqlite3_close(m_base);

    return connfd;
}