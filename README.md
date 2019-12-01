一个C语言实现的基于HTTP的文件管理服务器

https:
1. 安装 openssl，配置 CmakeLists.txt中的依赖。
2. 根据 https://blog.csdn.net/fly2010love/article/details/46415307 中的教程使用openssl生成 ca.crt server.crt server.key client.crt client.key 五个文件。
3. 将 ca.crt server.crt server.key 复制在 cmake-build-debug 新建server文件夹下。
4. 根据 https://www.cnblogs.com/saryli/p/8691541.html 使用 ca.crt client.crt client.key 配置 postman。

参与人：李煜峰 周杰 李文啸
