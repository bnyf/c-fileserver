一个C语言实现的基于HTTP的文件管理服务器

https:
1. 安装 openssl，配置 CmakeLists.txt中的依赖
2. 根据 https://blog.csdn.net/fly2010love/article/details/46415307中的教程使用openssl生成ca.crt server.crt server.key client.crt client.key五个文件
3. 将ca.crt server.crt server.key复制在cmake-build-debug新建server文件夹下
4. 根据https://www.cnblogs.com/saryli/p/8691541.html使用ca.crt client.crt client.key配置postman


参与人：李煜峰 周杰 李文啸