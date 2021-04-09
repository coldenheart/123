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

* File sample_process.h
* Description: handle acl resource
*/
#pragma once
#include "acl/acl.h"
#include "atlasutil/atlas_model.h"
#include "atlasutil/dvpp_process.h"
#include <map>
#include <memory>

#define FRAME_LENGTH 30
#define KEY_POINTS 14
#define SKELETON_CENTER 2

typedef struct keyPoints{
    float point_x;
    float point_y;
    int num;
} keyPointsT;

typedef struct connection{
    int point_1;
    int point_2;
    float score;
} connectionT;

typedef struct EngineTransNew
{
    float data[1][SKELETON_CENTER][FRAME_LENGTH][KEY_POINTS];
    size_t bufferSize;
}EngineTransNewT;

/**
* ClassifyProcess
*/
class GestureDetect {
public:
    GestureDetect(const char* OpenPoseModelPath, const char* GestureModelPath,
                 uint32_t modelWidth, uint32_t modelHeight);
    ~GestureDetect();

    AtlasError Init();
    AtlasError Preprocess(ImageData& resizedImage, ImageData& srcImage);
    AtlasError OpenPoseInference(std::vector<InferenceOutput>& inferOutputs, ImageData& resizedImage);
    AtlasError GestureInference(std::vector<InferenceOutput>& inferOutputs,
                                std::shared_ptr<EngineTransNewT> motion_data_new);
    AtlasError Postprocess(ImageData& image, std::vector<InferenceOutput>& modelOutput,
                                std::shared_ptr<EngineTransNewT> motion_data_new, int &success_num);
    AtlasError PostGestureProcess(std::vector<InferenceOutput>& modelOutput);
private:
    AtlasError InitResource();
    AtlasError InitModel(const char* omModelPath_openpose, const char* omModelPath_gesture);
    AtlasError CreateImageInfoBuffer();
    void DrowBoundBoxToImage(std::vector<BBox>& detectionResults,
                             const std::string& origImagePath);
    void DestroyResource();

private:
    int32_t deviceId_;
    aclrtContext context_;
    aclrtStream stream_;
    uint32_t imageInfoSize_;
    void* imageInfoBuf_;
    AtlasModel  modelOpenPose_;
    AtlasModel  modelGesture_;

    const char* modelPathOpenPose_;
    const char* modelPathGesture_;
    uint32_t modelWidth_;
    uint32_t modelHeight_;
    uint32_t inputDataSize_;
    DvppProcess dvpp_;
    aclrtRunMode runMode_;
    bool isInited_;
};

