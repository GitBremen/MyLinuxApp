#ifndef __DATABASE_H__
#define __DATABASE_H__

/* 判断用户是否存在 */
int userIsExist(const char * name);

/* 将用户信息写到数据库中 */
int saveUserInfo(const char * name, const char * passwd);

/* 用户名和密码是否匹配 */
int userMatchPasswd(const char * name, const char * passwd);

/* 维护用户名和通信句柄的映射 */
int saveUserMapConnfd(const char * name, int connfd);

/* 用户下线映射处理 */
int delUserMapConnfd(int connfd);

/* 根据用户名获取通信句柄 */
int accordCUserNameGetConnfd(const char * name);

#endif // __DATABASE_H__