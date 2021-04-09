/**
* Copyright 2020 Huawei Technologies Co., Ltd
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at

* http://www.apache.org/licenses/LICENSE-2.0

* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.

* File sample_process.cpp
* Description: handle acl resource
*/
#include <iostream>
#include "acl/acl.h"
#include "atlasutil/atlas_model.h"
#include "colorize_process.h"

using namespace std;

namespace {
    uint32_t kTopNConfidenceLevels = 5;
}

ColorizeProcess::ColorizeProcess(const char* modelPath, 
                                 uint32_t modelWidth, uint32_t modelHeight)
:deviceId_(0), context_(nullptr), stream_(nullptr), inputBuf_(nullptr), 
modelWidth_(modelWidth), modelHeight_(modelHeight), isInited_(false){
    modelPath_ = modelPath;
}

ColorizeProcess::~ColorizeProcess() {
    destroy_resource();
}

AtlasError ColorizeProcess::init_resource() {
    // ACL init
    const char *aclConfigPath = "../src/acl.json";
    AtlasError ret = aclInit(aclConfigPath);
    if (ret != ACL_ERROR_NONE) {
        ATLAS_LOG_ERROR("Acl init failed");
        return ATLAS_ERROR;
    }
    ATLAS_LOG_INFO("Acl init success");

    // open device
    ret = aclrtSetDevice(deviceId_);
    if (ret != ACL_ERROR_NONE) {
        ATLAS_LOG_ERROR("Acl open device %d failed", deviceId_);
        return ATLAS_ERROR;
    }
    ATLAS_LOG_INFO("Open device %d success", deviceId_);

    ret = aclrtGetRunMode(&runMode_);
    if (ret != ACL_ERROR_NONE) {
        ATLAS_LOG_ERROR("acl get run mode failed");
        return ATLAS_ERROR;
    }

    return ATLAS_OK;
}

AtlasError ColorizeProcess::create_input(size_t inputDataSize) {
    
    aclrtMalloc(&inputBuf_, (size_t)(inputDataSize), ACL_MEM_MALLOC_HUGE_FIRST);
    if (inputBuf_ == nullptr) {
        ATLAS_LOG_ERROR("Acl malloc image buffer failed.");
        return ATLAS_ERROR;
    }

    AtlasError ret = model_.CreateInput(inputBuf_, inputDataSize);
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Create mode input dataset failed");
        return ATLAS_ERROR;
    }

    return ATLAS_OK;
}

AtlasError ColorizeProcess::init() {
    if (isInited_) {
        ATLAS_LOG_INFO("Classify instance is initied already!");
        return ATLAS_OK;
    }

    AtlasError ret = init_resource();
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Init acl resource failed");
        return ATLAS_ERROR;
    }

    ret = model_.Init(modelPath_);
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Init model failed");
        return ATLAS_ERROR;
    }

    inputDataSize_ = model_.get_model_size();

    ret = create_input(inputDataSize_);
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Create model input failed");
        return ATLAS_ERROR;
    }

    isInited_ = true;
    return ATLAS_OK;
}

AtlasError ColorizeProcess::preprocess(const string& imageFile) {
    // read image using OPENCV
    cv::Mat mat = cv::imread(imageFile, CV_LOAD_IMAGE_COLOR);
    //resize
    cv::Mat reiszeMat;
    cv::resize(mat, reiszeMat, cv::Size(modelWidth_, modelHeight_));

    // deal image
    reiszeMat.convertTo(reiszeMat, CV_32FC3);
    reiszeMat = 1.0 * reiszeMat / 255;
    cv::cvtColor(reiszeMat, reiszeMat, CV_BGR2Lab);

    // pull out L channel and subtract 50 for mean-centering
    std::vector<cv::Mat> channels;
    cv::split(reiszeMat, channels);
    cv::Mat reiszeMatL = channels[0] - 50;

    if (mat.empty()) {
        return ATLAS_ERROR;
    }

    if (runMode_ == ACL_HOST) {
        aclError ret = aclrtMemcpy(inputBuf_, inputDataSize_,
                                   reiszeMatL.ptr<uint8_t>(), inputDataSize_,
                                   ACL_MEMCPY_HOST_TO_DEVICE);
        if (ret != ACL_ERROR_NONE) {
            ATLAS_LOG_ERROR("Copy resized image data to device failed.");
            return ATLAS_ERROR;
        }
    } else {
        memcpy(inputBuf_, reiszeMatL.ptr<uint8_t>(), inputDataSize_);
    }

    return ATLAS_OK;
}

AtlasError ColorizeProcess::inference(std::vector<InferenceOutput>& inferOutputs) {
    AtlasError ret = model_.Execute(inferOutputs);
    if (ret != ATLAS_OK) {
        ATLAS_LOG_ERROR("Execute model inference failed");
        return ATLAS_ERROR;
    }

    return ATLAS_OK;
}

AtlasError ColorizeProcess::postprocess(const string& imageFile, vector<InferenceOutput>& modelOutput)
{
    uint32_t dataSize = 0;
    void* data = modelOutput[0].data.get();
    if (data == nullptr) 
    {
        return ATLAS_ERROR;
    }

    dataSize = modelOutput[0].size;

    uint32_t size = static_cast<uint32_t>(dataSize) / sizeof(float);
    // get a channel and b channel result data
    cv::Mat mat_a(56, 56, CV_32FC1, const_cast<float*>((float*)data));
    cv::Mat mat_b(56, 56, CV_32FC1, const_cast<float*>((float*)data + size / 2));

    // pull out L channel in original image
    cv::Mat mat = cv::imread(imageFile, CV_LOAD_IMAGE_COLOR);
    mat.convertTo(mat, CV_32FC3);
    mat = 1.0 * mat / 255;
    cv::cvtColor(mat, mat, CV_BGR2Lab);
    std::vector<cv::Mat> channels;
    cv::split(mat, channels);

    // resize to match size of original image L
    int r = mat.rows;
    int c = mat.cols;
    cv::Mat mat_a_up(r, c, CV_32FC1);
    cv::Mat mat_b_up(r, c, CV_32FC1);
    cv::resize(mat_a, mat_a_up, cv::Size(c, r));
    cv::resize(mat_b, mat_b_up, cv::Size(c, r));

    // result Lab image
    cv::Mat newChannels[3] = { channels[0], mat_a_up, mat_b_up };
    cv::Mat resultImage;
    cv::merge(newChannels, 3, resultImage);

    //convert back to rgb
    cv::cvtColor(resultImage, resultImage, CV_Lab2BGR);
    resultImage = resultImage * 255;
    save_image(imageFile, resultImage);

    return ATLAS_OK;
}

void ColorizeProcess::save_image(const string& origImageFile, cv::Mat& image) {
    int pos = origImageFile.find_last_of("/");

    string filename(origImageFile.substr(pos + 1));
    stringstream sstream;
    sstream.str("");
    sstream << "./output/out_" << filename;

    string outputPath = sstream.str();    
    cv::imwrite(outputPath, image);
}

void ColorizeProcess::destroy_resource()
{
    model_.DestroyInput();
    model_.DestroyResource();

    aclrtFree(inputBuf_);
    inputBuf_ = nullptr;
    AtlasError ret;
    if (stream_ != nullptr) {
        ret = aclrtDestroyStream(stream_);
        if (ret != ACL_ERROR_NONE) {
            ATLAS_LOG_ERROR("destroy stream failed");
        }
        stream_ = nullptr;
    }
    ATLAS_LOG_INFO("end to destroy stream");

    if (context_ != nullptr) {
        ret = aclrtDestroyContext(context_);
        if (ret != ACL_ERROR_NONE) {
            ATLAS_LOG_ERROR("destroy context failed");
        }
        context_ = nullptr;
    }
    ATLAS_LOG_INFO("end to destroy context");

    ret = aclrtResetDevice(deviceId_);
    if (ret != ACL_ERROR_NONE) {
        ATLAS_LOG_ERROR("reset device failed");
    }
    ATLAS_LOG_INFO("end to reset device is %d", deviceId_);

    ret = aclFinalize();
    if (ret != ACL_ERROR_NONE) {
        ATLAS_LOG_ERROR("finalize acl failed");
    }
    ATLAS_LOG_INFO("end to finalize acl");
}
