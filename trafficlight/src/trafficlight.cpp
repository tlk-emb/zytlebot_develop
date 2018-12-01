#include <iostream>
#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "unistd.h"
#include <math.h>
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <geometry_msgs/Twist.h>


class ImageConverter
{
  ros::NodeHandle nh_;
  // if zybo
  ros::Subscriber image_sub_;

public:
    // コンストラクタ
    ImageConverter()
    // if pc
            : it_(nh_) {

    // カラー画像をサブスクライブ
      image_sub_ = it_.subscribe("/image_raw", 1,
                                 &ImageConverter::imageCb, this);
      image_pub_ = it_.advertise("/image_trafficlight", 1);

 }

  // デストラクタ
  ~ImageConverter()
  {
    // 全てのウインドウは破壊
    cv::destroyAllWindows();
  }

  


  // コールバック関数
  void imageCb(const sensor_msgs::ImageConstPtr &msg) {
  {
    cv_bridge::CvImagePtr cv_ptr, pub_img;
    try {
      // ROSからOpenCVの形式にtoCvCopy()で変換。cv_ptr->imageがcv::Matフォーマット。
      cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception &e) {
      ROS_ERROR("cv_bridge exception: %s", e.what());
      return;
    }
    cv::Mat baseImage = cv_ptr->image;

    // baseImageをなんか処理する
    cv::line(baseImage, cv::Point(0, 0), cv::Point(10, 10), cv::Scalar(0, 0, 255), 3, 8);

    // publish
    cv_ptr->image = baseImage;
    image_pub_.publish(cv_ptr3->toImageMsg());
  }
};

int main(int argc, char** argv)
{
  ros::init(argc, argv, "trafficlight");
  ImageConverter ic;
  ros::spin();
  return 0;
}
