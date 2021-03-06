English|[中文](README_200DK_CN.md)

# Basic Environment Configuration  
This readme file describes how to configure the basic environment, including the sudo permission, apt source, Atlas 200 DK networking, and environment variables. If they have been configured, skip this workflow.    

 **Perform the  following  operations  in  the  development  environment.** 


1. Add the following environment variables to the development environment for ATC model conversion. 

    1. Open the **.bashrc** file.  
        **vim ~/.bashrc** 

        Add the following environment variables to the file.  
        - For CANN 3.0.0 

            **export install_path=\\$HOME/Ascend/ascend-toolkit/latest**
    
            **export PATH=/usr/local/python3.7.5/bin:\\${install_path}/atc/ccec_compiler/bin:\\${install_path}/atc/bin:\\$PATH**  
    
            **export ASCEND_OPP_PATH=\\${install_path}/opp**  
   
            **export LD_LIBRARY_PATH=\\${install_path}/atc/lib64**  

            **export PYTHONPATH=\\${install_path}/atc/python/site-packages/te:\\${install_path}/atc/python/site-packages/topi:\\$PYTHONPATH**   
            
    
        - For CANN 3.1.0 

            **export install_path=\\$HOME/Ascend/ascend-toolkit/latest**
    
            **export PATH=/usr/local/python3.7.5/bin:\\${install_path}/atc/ccec_compiler/bin:\\${install_path}/atc/bin:\\$PATH**  
    
            **export ASCEND_OPP_PATH=\\${install_path}/opp**  
   
            **export LD_LIBRARY_PATH=\\${install_path}/atc/lib64**  
          
            **export PYTHONPATH=\\${install_path}/atc/python/site-packages:\\${install_path}/atc/python/site-packages/auto_tune.egg/auto_tune:\\${install_path}/atc/python/site-packages/schedule_search.egg:$PYTHONPATH**  
        >![](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "screenshot.png") **NOTE**  
        >**- If the development environment and operating environment are set up on the same server, do not configure LD_LIBRARY_PATH, avoiding the conflict with LD_LIBRARY_PATH in the operating environment when running the sample.** 

        - For CANN 3.2.0  

            **export install_path=\\$HOME/Ascend/ascend-toolkit/latest** 

            **export PATH=/usr/local/python3.7.5/bin:\\${install_path}/atc/ccec_compiler/bin:\\${install_path}/atc/bin:\\$PATH**  

            **export ASCEND_OPP_PATH=\${install_path}/opp**  

            **export ASCEND_AICPU_PATH=\${install_path}** 

        >![](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "screenshot.png") **NOTE**  
        >**- Replace install_path with the actual installation path.**  

    2. Make the configuration take effect.   
        **source ~/.bashrc**  

 **Perform the following operations in the operating environment (Atlas 200 DK).**   
1. Log in to the operating environment.  
    **ssh HwHiAiUser@X.X.X.X**  

2. Grant the sudo permission to the **HwHiAiUser** user.


    Switch to the **root** user. The default password of the **root** user is Mind@123.   
    **su root**

    Grant the write permission on the **sudoers** file and open the file.  
    **chmod u+w /etc/sudoers**   
    **vi /etc/sudoers** 

    Add the following content below **`# User privilege specification`** in the **sudoers** file.  
     **HwHiAiUser ALL=(ALL:ALL) ALL** 

    ![](https://images.gitee.com/uploads/images/2020/1128/121157_37d3b82d_7401379.png "Screenshot.png")  
    Run the following commands to remove the write permission on the **`/etc/sudoers`** file and switch to the common user:  
     **chmod u-w /etc/sudoers**  
     **exit**  
    >![](https://images.gitee.com/uploads/images/2020/1130/162342_1d7d35d7_7401379.png "screenshot.png") **NOTE**  
    >**After the dependency installation, you can cancel the sudo permission by yourself.**

3. Connect the Atlas 200 DK to the Internet.

    **sudo vi /etc/netplan/01-netcfg.yaml**   
    Set the following parameters.  
    **Note: The configuration of both Netplan and Python is indentation-sensitive.** 

    ```
    network:
      version: 2
    #  renderer: NetworkManager
      renderer: networkd
      ethernets:
        eth0:
          dhcp4: yes 
       
        usb0:
          dhcp4: no 
          addresses: [192.168.1.2/24] 
          gateway4: 192.168.0.1
    ```
    Connect the Atlas 200 DK to the Internet with a network cable, and run the following command for the configuration to take effect:   
    **sudo netplan apply**      

4. Update the apt source for the Atlas 200 DK.

     **Select either of the following two sources. If the source fails to be updated, replace it with an available source.** 

- Huawei Ubuntu 18.04 (ARM) source  

  Run the following command to change the source.  
  **sudo wget -O /etc/apt/sources.list https://repo.huaweicloud.com/repository/conf/Ubuntu-Ports-bionic.list**   

  Update the sources.  
  **sudo apt-get update** 

- Ubuntu 18.04 (ARM) source 

  Modify the source file.  
  **sudo vi /etc/apt/sources.list**   

  Replace the source file content with the following Ubuntu (ARM) sources:

```
deb http://ports.ubuntu.com/ bionic main restricted universe multiverse
deb-src http://ports.ubuntu.com/ bionic main restricted universe multiverse
deb http://ports.ubuntu.com/ bionic-updates main restricted universe multiverse
deb-src http://ports.ubuntu.com/ bionic-updates main restricted universe multiverse
deb http://ports.ubuntu.com/ bionic-security main restricted universe multiverse
deb-src http://ports.ubuntu.com/ bionic-security main restricted universe multiverse
deb http://ports.ubuntu.com/ bionic-backports main restricted universe multiverse
deb-src http://ports.ubuntu.com/ bionic-backports main restricted universe multiverse
deb http://ports.ubuntu.com/ubuntu-ports/ bionic main universe restricted
deb-src http://ports.ubuntu.com/ubuntu-ports/ bionic main universe restricted  
```
Update the sources.  
**sudo apt-get update** 

5. Install pyACL.  

- For CANN 3.0.0 

  Run the **find / -name pyACL** command in the development environment to copy the **pyACL** directory in the **arm64-linux_gcc7.3.0** directory to the **/home/HwHiAiUser/Ascend/** directory in the operating environment.

- For CANN 3.1.0


  Install pyACL by referring to https://support.huaweicloud.com/intl/en-us/dedg-A200dk_3000_c75/atlased_04_0017.html.

6. Add environment variables to the operating environment to run the project.
    1. Open the **.bashrc** file.  
        **vim ~/.bashrc** 

        Add the following environment variables to the file.  
        **export LD_LIBRARY_PATH=/home/HwHiAiUser/ascend_ddk/arm/lib:/home/HwHiAiUser/Ascend/acllib/lib64:\$LD_LIBRARY_PATH**     
        **export PYTHONPATH=/home/HwHiAiUser/Ascend/pyACL/python/site-packages/acl:\$PYTHONPATH**

        Save the configuration and exit.  
        **wq!**   

     2. Make the configuration take effect.  
        **source ~/.bashrc**

