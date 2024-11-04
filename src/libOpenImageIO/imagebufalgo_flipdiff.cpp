// Copyright Contributors to the OpenImageIO project.
// SPDX-License-Identifier: Apache-2.0
// https://github.com/AcademySoftwareFoundation/OpenImageIO


/// \file
/// Implementation of ImageBufAlgo algorithms.

#include <cmath>
#include <iostream> 

#include <OpenImageIO/Imath.h>
#include <OpenImageIO/dassert.h>
#include <OpenImageIO/fmath.h>
#include <OpenImageIO/imagebuf.h>
#include <OpenImageIO/imagebufalgo.h>
#include <OpenImageIO/imagebufalgo_util.h>
#include <OpenImageIO/FLIP/tool/FLIPToolHelpers.h>

using Imath::Color3f;

#include "imageio_pvt.h"
// #include "OpenImageIO/tinyexr.h"
// #include "OpenImageIO/stb_image.h"
// #include "OpenImageIO/stb_image_write.h"


 

template<class T>
inline Imath::Vec3<T>
powf(const Imath::Vec3<T>& x, float y)
{
    return Imath::Vec3<T>(powf(x[0], y), powf(x[1], y), powf(x[2], y));
}



OIIO_NAMESPACE_BEGIN


ImageBufAlgo::CompareResults
ImageBufAlgo::compare_flip(const std::string& ref, const std::string& test)
{
    
    ImageBufAlgo::CompareResults result;


    FLIPTool::runFlip(ref, test);

    print("\nFLIP!!\n");
    return result;
}



OIIO_NAMESPACE_END