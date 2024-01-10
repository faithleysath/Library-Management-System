# LMS - 图书馆管理系统

* 角色列表:
    * 用户
    * 图书馆工作人员
    * 供应商
	* 超级管理员
    
* 用户列表

```c++
        // 插入Accounts表的初始记录
        QString insertAccountsSql = "INSERT INTO Accounts (User_name, Password, Role_ID, Active) VALUES "
                                    "('user', 'password', 1, 1), "
                                    "('worker', 'password', 2, 1), "
                                    "('supplier', 'password', 3, 1), "
                                    "('admin', 'password', 4, 1),"
                                    "('tianyi', 'password', 1, 0);";
        query.exec(insertAccountsSql);
        // 插入Users表的初始记录
        QString insertUsersSql = "INSERT INTO Users (Full_name, User_name, Email, DOB) VALUES "
                                 "('用户', 'user', 'user@example.com', '1990-01-01'), "
                                 "('图书管理员', 'worker', 'worker@example.com', '1990-01-02'), "
                                 "('供应商', 'supplier', 'supplier@example.com', '1990-01-03'), "
                                 "('超级管理员', 'admin', 'admin@example.com', '1990-01-04'), "
                                 "('吴天一, 'tianyi', '202283250010@nuist.edu.cn', '2003-12-07');";
        query.exec(insertUsersSql);
```

即`user`、`worker`、`supplier`、`admin`、`tianyi`这五个用户，密码均为`password`