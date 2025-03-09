#ifndef __MENUINTERFACE_H__
#define __MENUINTERFACE_H__


/* 注册/登陆 菜单 */
void menuInterface();

/* 业务菜单 */
void bussinessMenu();

/* 登陆成功之后 */
void afterLoginSuccess(const char * loginName);

/* 添加好友 */
void addFriend(const char * fromName);

#endif //__MENUINTERFACE_H__