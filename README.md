# **车载健康评测系统**
## 文件目录
- `server` 服务端目录
    - `include` 第三方头文件
    - `lib` 静态库
    - `template` Web页面渲染文件
    - `tmp_data` 测量数据保存页面
    - `server.cc` 服务端代码
    - `util.hpp` 工具模块
- `soure` 下位机目录
    - `test/test.ino` 下位机代码
## **Server运行环境**
### Linux
### GCC 4.9+
- Centos下gcc升级方法
```
//这是偷懒的一种做法
yum install centos-release-scl -y
yum install devtoolset-7 -y
scl enable devtoolset-7 bash   // 只有本次有效 
//在 ~/.bash_profile 里面 添加一行 每次打开终端都会自动执行 即可永久生效
gcc --version
```
- Debian 一般默认版本即可
### make

## **第三方库**
- [**httplib**](https://github.com/yhirose/cpp-httplib) 只需要使用头文件
- [**jsoncpp**](https://github.com/open-source-parsers/jsoncpp)
已链接静态库

        自行编译方法，解压缩后输入以下命令
        先安装scons
            sudo yum install scons
            或者
            sudo apt-get install scons
        scons platform=linux-gcc

- [**ctemplate**](https://github.com/olafvdspek/ctemplate) 已链接静态库

        自行编译方法，解压缩后输入以下命令
        ./autogen.sh && ./configure
        make install

- **boost** 需进行安装

        yum install boost
        yum install boost-devel
        yum install boost-doc

- 源码已下载至lib中，可自行编译安装，httplib只需要头文件即可，项目中已包含可不必下载。其他库已链接静态库

## **服务端运行**
   `./server`
## **查看端口状态**
   `netstat -anp | grep 9527`
## **客户端运行**
   `ip地址:9527/all_info`
