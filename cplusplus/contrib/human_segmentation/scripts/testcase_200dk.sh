tf_model="https://modelzoo-train-atc.obs.cn-north-4.myhuaweicloud.com/003_Atc_Models/AE/ATC%20Model/human_segmentation/human_segmentation.pb"
aipp_cfg="https://modelzoo-train-atc.obs.cn-north-4.myhuaweicloud.com/003_Atc_Models/AE/ATC%20Model/human_segmentation/insert_op.cfg"

model_name="human_segmentation"
presenter_server_name="human_segmentation"

data_source="https://c7xcode.obs.cn-north-4.myhuaweicloud.com/models/human_segmentation/person.mp4"
project_name="cplusplus_human_segmentation_video"

version=$1

script_path="$( cd "$(dirname $BASH_SOURCE)" ; pwd -P)"
project_path=${script_path}/..

declare -i success=0
declare -i inferenceError=1
declare -i verifyResError=2



function downloadData() {

    mkdir -p ${project_path}/data/

    wget -O ${project_path}/data/"person.mp4"  ${data_source}  --no-check-certificate
    if [ $? -ne 0 ];then
        echo "download person.mp4 failed, please check Network."
        return 1
    fi

    return 0
}


function setAtcEnv() {
    # 设置模型转换时需要的环境变量
    if [[ ${version} = "c73" ]] || [[ ${version} = "C73" ]];then
        export install_path=/home/HwHiAiUser/Ascend/ascend-toolkit/latest
        export PATH=/usr/local/python3.7.5/bin:${install_path}/atc/ccec_compiler/bin:${install_path}/atc/bin:$PATH
        export PYTHONPATH=${install_path}/atc/python/site-packages/te:${install_path}/atc/python/site-packages/topi:$PYTHONPATH
        export ASCEND_OPP_PATH=${install_path}/opp
        export LD_LIBRARY_PATH=${install_path}/atc/lib64:${LD_LIBRARY_PATH}
    elif [[ ${version} = "c75" ]] || [[ ${version} = "C75" ]];then
        export install_path=$HOME/Ascend/ascend-toolkit/latest
        export PATH=/usr/local/python3.7.5/bin:${install_path}/atc/ccec_compiler/bin:${install_path}/atc/bin:$PATH
        export ASCEND_OPP_PATH=${install_path}/opp
        export PYTHONPATH=${install_path}/atc/python/site-packages:${install_path}/atc/python/site-packages/auto_tune.egg/auto_tune:${install_path}/atc/python/site-packages/schedule_search.egg:$PYTHONPATH
        export LD_LIBRARY_PATH=${install_path}/atc/lib64:${LD_LIBRARY_PATH}
    fi

    return 0
}

function setBuildEnv() {
    # 设置代码编译时需要的环境变量
    if [[ ${version} = "c73" ]] || [[ ${version} = "C73" ]];then
        export DDK_PATH=/home/HwHiAiUser/Ascend/ascend-toolkit/latest/arm64-linux_gcc7.3.0
        export NPU_HOST_LIB=${DDK_PATH}/acllib/lib64/stub
    elif [[ ${version} = "c75" ]] || [[ ${version} = "C75" ]];then
        export DDK_PATH=/home/HwHiAiUser/Ascend/ascend-toolkit/latest/arm64-linux
        export NPU_HOST_LIB=${DDK_PATH}/acllib/lib64/stub
    fi

    return 0
}

function downloadOriginalModel() {

    mkdir -p ${project_path}/model/
	
    wget -O ${project_path}/model/${tf_model##*/} ${tf_model} --no-check-certificate
    if [ $? -ne 0 ];then
        echo "install tf_model failed, please check Network."
        return 1
    fi
	
    wget -O ${project_path}/model/${aipp_cfg##*/} ${aipp_cfg} --no-check-certificate
    if [ $? -ne 0 ];then
        echo "install aipp_cfg failed, please check Network."
        return 1
    fi

    return 0
}

function main() {

    if [[ ${version}"x" = "x" ]];then
        echo "ERROR: version is invalid"
        return ${inferenceError}
    fi

    # 下载测试集
    downloadData
    if [ $? -ne 0 ];then
        echo "ERROR: download test images failed"
        return ${inferenceError}
    fi

    mkdir -p ${HOME}/models/${project_name}     
    if [[ $(find ${HOME}/models/${project_name} -name ${model_name}".om")"x" = "x" ]];then 
        downloadOriginalModel
        if [ $? -ne 0 ];then
            echo "ERROR: download original model failed"
            return ${inferenceError}
        fi

        # 设置模型转换的环境变量
        setAtcEnv
        if [ $? -ne 0 ];then
            echo "ERROR: set atc environment failed"
            return ${inferenceError}
        fi

        # 转模型
        cd ${project_path}/model/
        
        atc --input_shape="input_rgb:1,512,512,3" --output=${HOME}/models/${project_name}/${model_name} --insert_op_conf=${project_path}/model/insert_op.cfg --framework=3 --model=${project_path}/model/${tf_model##*/} --soc_version=Ascend310  --input_format=NHWC
	
        if [ $? -ne 0 ];then
            echo "ERROR: convert model failed"
            return ${inferenceError}
        fi

        ln -s ${HOME}/models/${project_name}/${model_name}".om" ${project_path}/model/${model_name}".om"
        if [ $? -ne 0 ];then
            echo "ERROR: failed to set model soft connection"
            return ${inferenceError}
        fi
    else 

        ln -s ${HOME}/models/${project_name}/${model_name}".om" ${project_path}/model/${model_name}".om"
        if [ $? -ne 0 ];then
            echo "ERROR: failed to set model soft connection"
            return ${inferenceError}
        fi
    fi

    # 创建目录用于存放编译文件
    mkdir -p ${project_path}/build/intermediates/host
    if [ $? -ne 0 ];then
        echo "ERROR: mkdir build folder failed. please check your project"
        return ${inferenceError}
    fi
    cd ${project_path}/build/intermediates/host

    setBuildEnv
    if [ $? -ne 0 ];then
        echo "ERROR: set build environment failed"
        return ${inferenceError}
    fi

    # 产生Makefile
    cmake ${project_path}/src -DCMAKE_CXX_COMPILER=aarch64-linux-gnu-g++ -DCMAKE_SKIP_RPATH=TRUE
    if [ $? -ne 0 ];then
        echo "ERROR: cmake failed. please check your project"
        return ${inferenceError}
    fi

    make 
    if [ $? -ne 0 ];then
        echo "ERROR: make failed. please check your project"
        return ${inferenceError}
    fi

    cd ${project_path}/out

    # 重新配置程序运行所需的环境变量
    export LD_LIBRARY_PATH=
    export LD_LIBRARY_PATH=/home/HwHiAiUser/Ascend/acllib/lib64:/home/HwHiAiUser/ascend_ddk/arm/lib:${LD_LIBRARY_PATH}

    # 开启presenter server
    cd ${script_path}/
    cd ../../../../common/
    bash run_presenter_server.sh ${script_path}/human_segmentation.conf
    if [ $? -ne 0 ];then
        echo "ERROR: run presenter server failed. please check your project"
        return ${inferenceError}
    fi

    sleep 2
    # 运行程序
    mv ${project_path}/out/main ${project_path}/out/${project_name}

    cd ${project_path}/out/
    ./${project_name} ${project_path}/data/person.mp4 &

    sleep 8

    project_pid=`ps -ef | grep "${project_name}" | grep "data" | awk -F ' ' '{print $2}'`
    if [[ ${project_pid}"X" != "X" ]];then
        echo -e "\033[33m kill existing project process: kill -9 ${project_pid}.\033[0m"
        kill -9 ${project_pid}
        if [ $? -ne 0 ];then
            echo "ERROR: kill project process failed."
            return ${inferenceError}
        fi

        presenter_server_pid=`ps -ef | grep "presenter_server\.py" | grep "${presenter_server_name}" | awk -F ' ' '{print $2}'`
        if [[ ${presenter_server_pid}"X" != "X" ]];then
            echo -e "\033[33mNow do presenter server configuration, kill existing presenter process: kill -9 ${presenter_server_pid}.\033[0m"
            kill -9 ${presenter_server_pid}
            if [ $? -ne 0 ];then
                echo "ERROR: kill presenter server process failed."
                return ${inferenceError}
            fi
        fi
    else 
        echo "ERROR: run failed. please check your project"
        return ${inferenceError}
    fi

    echo "run success"

    return ${success}
}
main
