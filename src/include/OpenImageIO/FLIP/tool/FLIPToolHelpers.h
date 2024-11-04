/*
 * Copyright (c) 2020-2024, NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * SPDX-FileCopyrightText: Copyright (c) 2020-2024 NVIDIA CORPORATION & AFFILIATES
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Visualizing and Communicating Errors in Rendered Images
// Ray Tracing Gems II, 2021,
// by Pontus Andersson, Jim Nilsson, and Tomas Akenine-Moller.
// Pointer to the chapter: https://research.nvidia.com/publication/2021-08_Visualizing-and-Communicating.

// Visualizing Errors in Rendered High Dynamic Range Images
// Eurographics 2021,
// by Pontus Andersson, Jim Nilsson, Peter Shirley, and Tomas Akenine-Moller.
// Pointer to the paper: https://research.nvidia.com/publication/2021-05_HDR-FLIP.

// FLIP: A Difference Evaluator for Alternating Images
// High Performance Graphics 2020,
// by Pontus Andersson, Jim Nilsson, Tomas Akenine-Moller,
// Magnus Oskarsson, Kalle Astrom, and Mark D. Fairchild.
// Pointer to the paper: https://research.nvidia.com/publication/2020-07_FLIP.

// Code by Pontus Ebelin (formerly Andersson), Jim Nilsson, and Tomas Akenine-Moller.

#include <cmath>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <filesystem>

#if defined(_WIN32) && !defined(NOMINMAX)
#define NOMINMAX
#endif

#define FIXED_DECIMAL_DIGITS(x, d) std::fixed << std::setprecision(d) << (x)

#include <OpenImageIO/FLIP/FLIP.h>

#include "imagehelpers.h"
// #include "commandline.h"
// #include "filename.h"
#include "pooling.h"

namespace FLIPTool
{
    inline std::string f2s(float value, size_t decimals = 4)
    {
        std::stringstream ss;
        ss << std::string(value < 0.0f ? "m" : "p") << FIXED_DECIMAL_DIGITS(std::abs(value), decimals);
        return ss.str();
    }

    // Optionally store the intermediate LDR images and LDR-FLIP error maps produced during the evaluation of HDR-FLIP.
    // static void saveHDROutputLDRImages(commandline& commandLine, const FLIP::Parameters& parameters, const std::string& basename, const FLIP::filename& flipFileName,
    //     const FLIP::filename& referenceFileName, const FLIP::filename& testFileName, const std::string& destinationDirectory,
    //     std::vector<FLIP::image<float>*> hdrOutputFlipLDRImages, std::vector<FLIP::image<FLIP::color3>*> hdrOutputLDRImages)
    // {
    //     if (hdrOutputLDRImages.size() > 0)
    //     {
    //         FLIP::filename rFileName("tmp.png");
    //         FLIP::filename tFileName("tmp.png");
    //         if (hdrOutputLDRImages.size() != parameters.numExposures * 2)
    //         {
    //             std::cout << "FLIP tool error: the number of LDR images from HDR-FLIP is not the expected number.\nExiting.\n";
    //             exit(EXIT_FAILURE);
    //         }

    //         const float exposureStepSize = (parameters.stopExposure - parameters.startExposure) / (parameters.numExposures - 1);
    //         for (int i = 0; i < parameters.numExposures; i++)
    //         {
    //             std::string expCount, expString;
    //             setExposureStrings(i, parameters.startExposure + i * exposureStepSize, expCount, expString);

    //             if (basename == "")
    //             {
    //                 rFileName.setName(referenceFileName.getName() + "." + parameters.tonemapper + "." + expCount + "." + expString);
    //                 tFileName.setName(testFileName.getName() + "." + parameters.tonemapper + "." + expCount + "." + expString);
    //             }
    //             else
    //             {
    //                 rFileName.setName(basename + ".reference." + "." + expCount);
    //                 tFileName.setName(basename + ".test." + "." + expCount);
    //             }
    //             FLIP::image<FLIP::color3>* rImage = hdrOutputLDRImages[0];
    //             FLIP::image<FLIP::color3>* tImage = hdrOutputLDRImages[1];
    //             hdrOutputLDRImages.erase(hdrOutputLDRImages.begin());
    //             hdrOutputLDRImages.erase(hdrOutputLDRImages.begin());
    //             rImage->LinearRGBTosRGB();
    //             tImage->LinearRGBTosRGB();
    //             ImageHelpers::pngSave(destinationDirectory + "/" + rFileName.toString(), *rImage);
    //             ImageHelpers::pngSave(destinationDirectory + "/" + tFileName.toString(), *tImage);
    //             delete rImage;
    //             delete tImage;
    //         }
    //     }
    //     if (hdrOutputFlipLDRImages.size() > 0)
    //     {
    //         if (hdrOutputFlipLDRImages.size() != parameters.numExposures)
    //         {
    //             std::cout << "FLIP tool error: the number of FLIP LDR images from HDR-FLIP is not the expected number.\nExiting.\n";
    //             exit(EXIT_FAILURE);
    //         }

    //         const float exposureStepSize = (parameters.stopExposure - parameters.startExposure) / (parameters.numExposures - 1);
    //         for (int i = 0; i < parameters.numExposures; i++)
    //         {
    //             std::string expCount, expString;
    //             setExposureStrings(i, parameters.startExposure + i * exposureStepSize, expCount, expString);

    //             FLIP::image<float>* flipImage = hdrOutputFlipLDRImages[0];
    //             hdrOutputFlipLDRImages.erase(hdrOutputFlipLDRImages.begin());

    //             FLIP::image<FLIP::color3> pngResult(flipImage->getWidth(), flipImage->getHeight());

    //             if (!commandLine.optionSet("no-magma"))
    //             {
    //                 pngResult.colorMap(*flipImage, FLIP::magmaMap);
    //             }
    //             else
    //             {
    //                 pngResult.copyFloatToColor3(*flipImage);
    //             }
    //             if (basename == "")
    //             {
    //                 ImageHelpers::pngSave(destinationDirectory + "/" + "flip." + referenceFileName.getName() + "." + testFileName.getName() + "." + std::to_string(int(std::round(parameters.PPD))) + "ppd.ldr." + parameters.tonemapper + "." + expCount + "." + expString + ".png", pngResult);
    //             }
    //             else
    //             {
    //                 ImageHelpers::pngSave(destinationDirectory + "/" + basename + "." + expCount + ".png", pngResult);
    //             }
    //             ImageHelpers::pngSave(destinationDirectory + "/" + flipFileName.toString(), pngResult);
    //             delete flipImage;
    //         }
    //     }
    // }

    static void printStats(FLIP::image<float>& errorMapFLIP, FLIPPooling::pooling<float>& pooledValues,const std::string& ref, const std::string& test)
    {
        for (int y = 0; y < errorMapFLIP.getHeight(); y++)
        {
            for (int x = 0; x < errorMapFLIP.getWidth(); x++)
            {
                pooledValues.update(x, y, errorMapFLIP.get(x, y));
            }
        }

        // Collect pooled values and elapsed time.
        float mean = pooledValues.getMean();
        float weightedMedian = pooledValues.getPercentile(0.5f, true);
        float firstWeightedQuartile = pooledValues.getPercentile(0.25f, true);
        float thirdWeightedQuartile = pooledValues.getPercentile(0.75f, true);
        float minValue = pooledValues.getMinValue();
        float maxValue = pooledValues.getMaxValue();

       
        std::cout << "FLIP between reference image <" << ref << "> and test image <" << test << ">\n";
        std::cout << "     Mean: " << FIXED_DECIMAL_DIGITS(mean, 6) << "\n";
        
        std::cout << "     Weighted median: " << FIXED_DECIMAL_DIGITS(weightedMedian, 6) << "\n";
        std::cout << "     1st weighted quartile: " << FIXED_DECIMAL_DIGITS(firstWeightedQuartile, 6) << "\n";
        std::cout << "     3rd weighted quartile: " << FIXED_DECIMAL_DIGITS(thirdWeightedQuartile, 6) << "\n";
        std::cout << "     Min: " << FIXED_DECIMAL_DIGITS(minValue, 6) << "\n";
        std::cout << "     Max: " << FIXED_DECIMAL_DIGITS(maxValue, 6) << "\n";
        
    }



    int runFlip(const std::string& ref, const std::string& test){

        FLIP::Parameters parameters;
        FLIPPooling::pooling<float> pooledValues;

        

        bool bUseHDR = (ref.find("exr") != std::string::npos);

        FLIP::image<FLIP::color3> referenceImage;
        ImageHelpers::load(referenceImage, ref);   // Load reference image.
        if (!bUseHDR)
        {
            referenceImage.sRGBToLinearRGB();
        }

        FLIP::image<FLIP::color3> testImage;
        ImageHelpers::load(testImage, test);     // Load test image.
        if (!bUseHDR)
        {
            testImage.sRGBToLinearRGB();
        }

        pooledValues = FLIPPooling::pooling<float>(100);
        FLIP::image<float> errorMapFLIP(referenceImage.getWidth(), referenceImage.getHeight(), 0.0f);

        FLIP::evaluate(referenceImage, testImage, bUseHDR, parameters, errorMapFLIP);

        std::cout << "     Assumed tone mapper: " << ((parameters.tonemapper == "aces") ? "ACES" : (parameters.tonemapper == "hable" ? "Hable" : "Reinhard")) << "\n";
        std::cout << "     Start exposure: " << FIXED_DECIMAL_DIGITS(parameters.startExposure, 4) << "\n";
        std::cout << "     Stop exposure: " << FIXED_DECIMAL_DIGITS(parameters.stopExposure, 4) << "\n";
        std::cout << "     Number of exposures: " << parameters.numExposures << "\n\n";


        printStats(errorMapFLIP, pooledValues, ref, test);
        return 1;

    }

}