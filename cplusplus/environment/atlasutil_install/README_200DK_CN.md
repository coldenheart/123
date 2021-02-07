中文|[English](README_300_EN.md)
# atlasutil库使用说明<a name="ZH-CN_TOPIC_0228768065"></a>
1. atlasutil库对当前开源社区样例中

    - Atlas200DK板载摄像头

    - acl dvpp图像和视频处理

    - acl模型推理等进行封装

    等重复代码进行封装，提供一组公共接口。


2. 本库仅供当前社区开源样例使用，不覆盖ascend平台应用开发的所有场景，不作为用户应用开发的标准库；仅支持Atlas200DK和Atlas300样例。

# 部署方法

$\color{red}{以下命令在开发环境上用安装开发套件包的用户执行}$
1.  下载源码  
 **cd $HOME**   
 **git clone https://gitee.com/ascend/samples.git**   
 

2.  编译并安装atlasutil  
    **cd $HOME/samples/cplusplus/common/atlasutil/src**     
    **make**   
    **make install**
    >![输入图片说明](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "屏幕截图.png") **说明：**  
    >  **生成的libatalsutil.so在\\$HOME/ascend_ddk/arm/lib/下；头文件在\\$HOME/ascend_ddk/arm/include/atlasutil下。**   

3.  将编译好的so传到运行环境 (如开发环境和运行环境安装在同一服务器，请忽略此步)   
    **scp \$HOME/ascend_ddk/arm/lib/libatlasutil.so HwHiAiUser@192.168.1.2:/home/HwHiAiUser/ascend_ddk/arm/lib/**   

    
 

 
