/*M///////////////////////////////////////////////////////////////////////////////////////
//
//  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
//
//  By downloading, copying, installing or using the software you agree to this license.
//  If you do not agree to this license, do not download, install,
//  copy or use the software.
//
//
//                          License Agreement
//                For Open Source Computer Vision Library
//
// Copyright (C) 2000-2008, Intel Corporation, all rights reserved.
// Copyright (C) 2009, Willow Garage Inc., all rights reserved.
// Third party copyrights are property of their respective owners.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//   * Redistribution's of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//   * Redistribution's in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//   * The name of the copyright holders may not be used to endorse or promote products
//     derived from this software without specific prior written permission.
//
// This software is provided by the copyright holders and contributors "as is" and
// any express or implied warranties, including, but not limited to, the implied
// warranties of merchantability and fitness for a particular purpose are disclaimed.
// In no event shall the Intel Corporation or contributors be liable for any direct,
// indirect, incidental, special, exemplary, or consequential damages
// (including, but not limited to, procurement of substitute goods or services;
// loss of use, data, or profits; or business interruption) however caused
// and on any theory of liability, whether in contract, strict liability,
// or tort (including negligence or otherwise) arising in any way out of
// the use of this software, even if advised of the possibility of such damage.
//
//M*/
#ifndef __OPENCV_PRECOMP_H__
#define __OPENCV_PRECOMP_H__

// #include "opencv2/core/utility.hpp"

// #include "opencv2/core/private.hpp"

// #include "opencv2/calib3d.hpp"
// #include "opencv2/imgproc.hpp"
// #include "opencv2/features2d.hpp"

// #include "opencv2/core/ocl.hpp"

#include <opencv2/opencv.hpp>

#define GET_OPTIMIZED(func) (func)

#ifndef CV_OVERRIDE
#  define CV_OVERRIDE override
#endif
#ifndef CV_FINAL
#  define CV_FINAL final
#endif

namespace cv
{

/**
 * Compute the number of iterations given the confidence, outlier ratio, number
 * of model points and the maximum iteration number.
 *
 * @param p confidence value
 * @param ep outlier ratio
 * @param modelPoints number of model points required for estimation
 * @param maxIters maximum number of iterations
 * @return
 * \f[
 * \frac{\ln(1-p)}{\ln\left(1-(1-ep)^\mathrm{modelPoints}\right)}
 * \f]
 *
 * If the computed number of iterations is larger than maxIters, then maxIters is returned.
 */
int RANSACUpdateNumIters( double p, double ep, int modelPoints, int maxIters );

class CV_EXPORTS PointSetRegistrator : public Algorithm
{
public:
    class CV_EXPORTS Callback
    {
    public:
        virtual ~Callback() {}
        virtual int runKernel(InputArray m1, InputArray m2, OutputArray model) const = 0;
        virtual void computeError(InputArray m1, InputArray m2, InputArray model, OutputArray err) const = 0;
        virtual bool checkSubset(InputArray, InputArray, int) const { return true; }
    };

    virtual void setCallback(const Ptr<PointSetRegistrator::Callback>& cb) = 0;
    virtual bool run(InputArray m1, InputArray m2, OutputArray model, OutputArray mask) const = 0;
};

CV_EXPORTS Ptr<PointSetRegistrator> createRANSACPointSetRegistrator(const Ptr<PointSetRegistrator::Callback>& cb,
                                                                    int modelPoints, double threshold,
                                                                    double confidence=0.99, int maxIters=1000 );

CV_EXPORTS Ptr<PointSetRegistrator> createLMeDSPointSetRegistrator(const Ptr<PointSetRegistrator::Callback>& cb,
                                                                   int modelPoints, double confidence=0.99, int maxIters=1000 );

template<typename T> inline int compressElems( T* ptr, const uchar* mask, int mstep, int count )
{
    int i, j;
    for( i = j = 0; i < count; i++ )
        if( mask[i*mstep] )
        {
            if( i > j )
                ptr[j] = ptr[i];
            j++;
        }
    return j;
}

static inline bool haveCollinearPoints( const Mat& m, int count )
{
    int j, k, i = count-1;
    const Point2f* ptr = m.ptr<Point2f>();

    // check that the i-th selected point does not belong
    // to a line connecting some previously selected points
    // also checks that points are not too close to each other
    for( j = 0; j < i; j++ )
    {
        double dx1 = ptr[j].x - ptr[i].x;
        double dy1 = ptr[j].y - ptr[i].y;
        for( k = 0; k < j; k++ )
        {
            double dx2 = ptr[k].x - ptr[i].x;
            double dy2 = ptr[k].y - ptr[i].y;
            if( fabs(dx2*dy1 - dy2*dx1) <= FLT_EPSILON*(fabs(dx1) + fabs(dy1) + fabs(dx2) + fabs(dy2)))
                return true;
        }
    }
    return false;
}
template<typename T>
static inline Mat
skewMat( const Mat_<T> &x )
{
  Mat_<T> skew(3,3);
  skew <<   0 , -x(2),  x(1),
          x(2),    0 , -x(0),
         -x(1),  x(0),    0;

  return std::move(skew);
}

static inline Mat
skew( InputArray _x )
{
  const Mat x = _x.getMat();
  const int depth = x.depth();
  CV_Assert( x.size() == Size(3,1) || x.size() == Size(1,3) );
  CV_Assert( depth == CV_32F || depth == CV_64F );

  Mat skewMatrix;
  if( depth == CV_32F )
  {
    skewMatrix = skewMat<float>(x);
  }
  else if( depth == CV_64F )
  {
    skewMatrix = skewMat<double>(x);
  }
  else
  {
    //CV_Error(CV_StsBadArg, "The DataType must be CV_32F or CV_64F");
  }

  return skewMatrix;
}


} // namespace cv

int checkChessboardBinary(const cv::Mat & img, const cv::Size & size);

#endif
