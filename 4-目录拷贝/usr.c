#include"myhead.h"

void usr_add(node *head)
{
    data new_data;
    printf("请输入用户id\n");
    scanf("%d",&new_data.id);while(getchar()!='\n');
    node *p = find_node(head,new_data);
    if(p != NULL)
    {
        printf("该id已存在！\n");
        return;
    }

    printf("请输入姓名\n");
    fgets(new_data.name,20,stdin);strtok(new_data.name,"\n");
    printf("请设置密码\n");
    scanf("%d",&new_data.pwd);while(getchar()!='\n');
    printf("请输入tel\n");
    scanf("%d",&new_data.tel);while(getchar()!='\n');
    kl_add_tail(new_data,head);
    add_info_to_file(new_data);
    return;
}

void usr_show(node *head)
{
    printf("id\t name\t pwd\t tel\n");
    kl_show(head);
    return;
}

void usr_del(node *head)
{
    data get_info;
    printf("请输入用户id\n");
    scanf("%d",&get_info.id);
    kl_del(get_info,head);
    return;
}

node *login(node *head)
{
    printf("请输入用户id\n");
    data get_info;
    scanf("%d",&get_info.id);while(getchar()!='\n');
    node *p = find_node(head,get_info);
    if(p == NULL)
    {
        printf("该id不存在！\n");
        return NULL;
    }
    
    int n = 3;
    while(n--)
    {
        printf("请输入密码\n");
        scanf("%d",&get_info.pwd);while(getchar()!='\n');
        if(get_info.pwd == p->data.pwd)
        {
            printf("登录成功！\n");
            return p;
        }
        else
        {
            printf("密码错误，请重新输入！\n");
        }
    }
    printf("密码输错三次，建议重置密码！\n");
    return NULL;
}

void update_info(node *head)
{
    int cmd;
    data p;
    printf("请输入id\n");
    scanf("%d",&p.id);
    node *usr = find_node(head,p);
    if(usr == NULL)
    {
        printf("该id不存在!\n");
        return;
    }
    while(1)
    {
        printf("0.退出\n 1.修改id\n 2.修改密码\n 3.修改用户名\n 4.修改电话\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 0)    break;
        switch(cmd)
        {
            case 1: printf("请输入新的id\n");
                    scanf("%d",&p.id);while(getchar()!='\n');
                    if(find_node(head,p) != NULL)
                        printf("该id已被占用!\n");
                    else usr->data.id = p.id;
                    break;
            case 2: printf("请输入新的密码\n");
                    scanf("%d",&usr->data.pwd);while(getchar()!='\n');
                    break;
            case 3: printf("请输入新的用户名\n");
                    fgets(usr->data.name,20,stdin);strtok(usr->data.name,"\n");
                    break;
            case 4: printf("请输入新的电话\n");
                    scanf("%d",&usr->data.tel);while(getchar()!='\n');
                    break;
            default: break;
        }
    }
    empty_and_rewrite_info_to_file(head);
    return;
}

void panel()
{
    node *head = kl_init();
    if(head == NULL)
        return;
    read_file_to_link(head);
    node *usr;
    int cmd;
    while(1)
    {
        printf("\n==================================================\n");
        printf("==                  0.退出                      ==\n");
        printf("==                  1.登录                      ==\n");
        printf("==                  2.注册                      ==\n");
        printf("==                  3.普通用户                  ==\n");
        printf("==================================================\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 0)    break;
        switch(cmd)
        {
            case 1: usr = login(head);
                    if(usr == NULL)
                        break;
                    login_usr();
                    break;
            case 2: usr_add(head);
                    break;
            case 3: regular_usr();
                    break;
            default: break;
        }
    }
    return;
}

void regular_usr()
{
    int cmd;
    char dir[300];
    while(1)
    {
        printf("\n======================普通用户====================\n");
        printf("==                     0.退出                   ==\n");
        printf("==                  1.全部目录查询               ==\n");
        printf("==================================================\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 0)    break;
        else
        {
            printf("请输入要路径\n");
            fgets(dir,sizeof(dir),stdin);strtok(dir,"\n");
            show_all_dir(dir,0);
        }
    }
    return;
}

void login_usr()
{
    /*
    全部目录拷贝、指定目录查询、指定目录拷贝*/
    int cmd,n;
    char src_dir[300],dest_dir[300];
    while(1)
    {
        printf("\n======================普通用户====================\n");
        printf("==                     0.退出                   ==\n");
        printf("==                  1.全部目录查询               ==\n");
        printf("==                  2.全部目录拷贝               ==\n");
        printf("==                  3.指定目录查询               ==\n");
        printf("==                  4.指定目录拷贝               ==\n");
        printf("==================================================\n");
        scanf("%d",&cmd);while(getchar()!='\n');
        if(cmd == 0)    break;
        switch(cmd)
        {
            case 1: printf("请输入路径\n");
                    fgets(src_dir,sizeof(src_dir),stdin);strtok(src_dir,"\n");
                    puts(src_dir);
                    show_all_dir(src_dir,0);
                    break;
            case 2: printf("请输入要拷贝的目录\n");
                    fgets(src_dir,sizeof(src_dir),stdin);strtok(src_dir,"\n");
                    printf("请输入新建目录\n");
                    fgets(dest_dir,sizeof(dest_dir),stdin);strtok(dest_dir,"\n");
                    n = copy_all_dir(src_dir,dest_dir);
                    if(n == 0)
                        printf("拷贝完成!\n");
                    break;
            case 3: printf("请输入路径\n");
                    fgets(src_dir,sizeof(src_dir),stdin);strtok(src_dir,"\n");
                    show_special_dir(src_dir,0);
                    break;
            case 4: printf("请输入要拷贝的目录\n");
                    fgets(src_dir,sizeof(src_dir),stdin);strtok(src_dir,"\n");
                    printf("请输入新建目录名\n");
                    fgets(dest_dir,sizeof(dest_dir),stdin);strtok(dest_dir,"\n");
                    n = copy_special_dir(src_dir,dest_dir);
                    if(n == 0)
                        printf("拷贝完成!\n");
                    break;
            default: break;
        }
    }
    return;
}