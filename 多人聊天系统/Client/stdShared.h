#ifndef __STD_SHARED_H__
#define __STD_SHARED_H__


enum FUNCTION
{
    REGISTER = 1,
    LOGIN,
    ADDFRIEND,          /* 添加好友 */
    DELFRIEND,          /* 删除好友 */
    PRIVATE_CHAT,       /* 私人聊天 */
};


typedef enum RESPONSE_CODE
{
    /*-----------------注册-------------*/
    /* 注册失败 */
    REGISTER_ERROR = 1,
    /* 注册成功 */
    REGISTER_SUCCESS,

    /*-----------------登陆-------------*/
    /* 用户不存在 */    
    LOGIN_NOUSER_ERROR,
    /* 密码不匹配 */
    LOGIN_NOMATCH_ERROR,
    /* 登陆成功 */
    LOGIN_SUCCESS,

    /* ------------添加好友------------*/
    ADDFRIEND_NOUSER_ERROR,
    ADDFRIEND_RECEIVE_APPLY,
    ADDFRIEND_REQUEST_SUCCESS,
} RESPONSE_CODE;


/* 传输的数据 */
typedef struct Message
{
    int bussinessType;  /* 业务类型 */
    char name[128];
    char passwd[128];

    char toname[128];       /* 被添加的联系人 */
    char fromname[128];     /* 主动添加申请人 */
} Message;

/* 接收服务器的信息 */
typedef struct ResponseMsg
{
    int bussinessType;  /* 业务类型 */
    RESPONSE_CODE code; /* 回复码 */

    char fromName[128]; /* 发信息的人(主动方) */
} ResponseMsg;


#endif //__STD_SHARED_H__