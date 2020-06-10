#ifndef __FUSION_PUBLIC_CORE_DEPTHCOLORTOPCL_H__
#define __FUSION_PUBLIC_CORE_DEPTHCOLORTOPCL_H__

#include <FuAssert.h>
#include <Buffer.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <string>
#include <vector>
#include <tinyply.h>
#include <fstream>

namespace fu {
namespace fusion {

static void DepthColorToPCL(const std::string& colorFilepath, const std::string& depthFilepath, int thres, const std::string& plyFilepath)
{
	/// load color and depth imgs
	cv::Mat color = cv::imread(colorFilepath, cv::IMREAD_ANYCOLOR);
	cv::Mat depth = cv::imread(depthFilepath, cv::IMREAD_UNCHANGED);
	size_t dw = depth.cols;
	size_t dh = depth.rows;
	size_t cw = color.cols;
	size_t ch = color.rows;
	DebugAssertMsg(dw == cw, "depth and color images do not have the same size.");
	DebugAssertMsg(dh == ch, "depth and color images do not have the same size.");
	/// convert to point cloud
	std::vector<float3> v(dw * dh);
	std::vector<uchar4> c(dw * dh);
	for (int h = 0; h < dh; h++)
	{
		for (int w = 0; w < dw; w++)
		{
			const float radius = depth.at<float>(h, w);
			if (radius > thres)
			{
				continue;
			}
			else
			{
				const float az = (static_cast<float>(w) / static_cast<float>(dw)- 0.5f) * 2.0f * CV_PI /*+ CV_PI*/;
				const float el = (static_cast<float>(h) / static_cast<float>(dh) - 0.5f) * CV_PI;
				const float x = std::cos(el) * std::cos(az) * radius;
				const float z = std::cos(el) * std::sin(az) * radius;
				const float y = std::sin(el) * radius;
				const uchar R = color.at<cv::Vec3b>(h, w)[2];
				const uchar G = color.at<cv::Vec3b>(h, w)[1];
				const uchar B = color.at<cv::Vec3b>(h, w)[0];
				const uchar A = 255;
				v.emplace_back(make_float3(x, y, z));
				c.emplace_back(make_uchar4(R, G, B, A));
			}
		}
	}
	/// create and save ply
	std::filebuf fb;
	fb.open(plyFilepath, std::ios::out | std::ios::binary);
	std::ostream outStream(&fb);
	tinyply::PlyFile outputFile;
	outputFile.add_properties_to_element("vertex", { "x", "y", "z" }, tinyply::Type::FLOAT32, v.size(), reinterpret_cast<uint8_t*>(v.data()), tinyply::Type::INVALID, 0);
	outputFile.add_properties_to_element("vertex", { "red", "green", "blue", "alpha" }, tinyply::Type::UINT8, c.size(), reinterpret_cast<uint8_t*>(c.data()), tinyply::Type::INVALID, 0);
	outputFile.write(outStream, false);
	fb.close();
}
}	///	!namespace fusion
}	///	!namespace fu
#endif	///	!__FUSION_PUBLIC_CORE_DEPTHCOLORTOPCL_H__