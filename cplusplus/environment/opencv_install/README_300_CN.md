中文|[English](README_300_EN.md)

# 安装ffmpeg+opencv<a name="ZH-CN_TOPIC_0228768065"></a>

安装ffmpeg和opencv的原因是适配多样性的数据预处理和后处理，昇腾社区的部分样例也是基于ffmpeg和opencv做的处理。

$\color{red}{以下命令在开发环境上执行，以普通用户为HwHiAiUser为例，请根据实际情况进行修改。}$  


1.  安装相关依赖  
    **sudo apt-get install build-essential libgtk2.0-dev libavcodec-dev libavformat-dev libjpeg-dev libtiff5-dev git cmake libswscale-dev pkg-config -y** 
 
    >![输入图片说明](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "屏幕截图.png") **说明：**  
    >  **若apt-get安装依赖出现类似报错（dpkg: error processing package *** (--configure)） ，请参考[FAQ](https://bbs.huaweicloud.com/forum/thread-74123-1-1.html)来解决。**  
    
2.  安装ffmpeg  
    1. 创建文件夹，用于存放编译后的文件  
        **mkdir -p $HOME/ascend_ddk/x86**

    2. 下载ffmpeg  
        **cd $HOME**  
        **wget http://www.ffmpeg.org/releases/ffmpeg-4.1.3.tar.gz --no-check-certificate**  
        **tar -zxvf ffmpeg-4.1.3.tar.gz**  
        **cd ffmpeg-4.1.3**

    3. 安装ffmpeg   
        **./configure --enable-shared --enable-pic --enable-static --disable-x86asm  --prefix=\$HOME/ascend_ddk/x86**  
        **make -j8**    
        **make install** 

    4. 将ffmpeg添加到系统环境变量中，使得其他程序能够找到ffmpeg环境   
        切换为root用户  
        **su root**  
        打开conf配置文件  
        **vim /etc/ld.so.conf.d/ffmpeg.conf**  
        在末尾添加一行。  
        **/home/HwHiAiUser/ascend_ddk/x86/lib**  
        使配置生效  
        **ldconfig**   

    5. 配置profile系统文件   
        **vim /etc/profile**  
        在末尾添加一行   
        **export PATH=$PATH:/home/HwHiAiUser/ascend_ddk/x86/bin**  
        使配置文件生效    
        **source /etc/profile**  
    
    6. 使opencv能找到ffmpeg。  
        **cp /home/HwHiAiUser/ascend_ddk/x86/lib/pkgconfig/\* /usr/share/pkgconfig**  
       切换回普通用户  
        **exit**
    >![输入图片说明](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "屏幕截图.png") **说明：
    4、5、6三步中的HwHiAiUser请根据实际情况替换。** 

3.  安装opencv 
    1.  下载opencv  
        **cd \$HOME**    
        **git clone -b 4.3.0 https://gitee.com/mirrors/opencv.git**   
        **cd opencv**  
        **mkdir build**  
        **cd build**  

    2.  安装opencv   
        ```
        cmake -D BUILD_SHARED_LIBS=ON -D BUILD_TESTS=OFF -D CMAKE_BUILD_TYPE=RELEASE  -D WITH_LIBV4L=ON -D CMAKE_INSTALL_PREFIX=$HOME/ascend_ddk/x86 ..  
        ```
        **make -j8**  
        **make install**  
 

4.  将开发环境安装的ffmpeg、opencv库导入运行环境中，以提供运行使用  **（如开发环境和运行环境在同一服务器上，请忽略此步）**    
   
    $\color{red}{注意：以下操作在运行环境执行}$ 
  
    普通用户登录运行环境 
     
     **mkdir \$HOME/ascend_ddk**   
     **scp -r HwHiAiUser@X.X.X.X:/home/HwHiAiUser/ascend_ddk/x86 \$HOME/ascend_ddk**   
     **scp -r HwHiAiUser@X.X.X.X:/usr/lib/x86_64-linux-gnu/lib\* \$HOME/ascend_ddk/x86/lib**  
    <br/> </br> 
 
    >![输入图片说明](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "屏幕截图.png") **说明：
    X.X.X.X请替换为开发环境的ip，HwHiAiUser请根据实际情况替换。** 