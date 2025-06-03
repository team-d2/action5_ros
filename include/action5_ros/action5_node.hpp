// Copyright 2025 team-d2
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ACTION5_ROS__ACTION5_NODE_HPP_
#define ACTION5_ROS__ACTION5_NODE_HPP_

#include <memory>
#include <string>

#include "action5_ros/visibility.hpp"
#include "cv_bridge/cv_bridge.h"
#include "opencv4/opencv2/opencv.hpp"
#include "rclcpp/node.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "sensor_msgs/msg/image.hpp"

namespace action5_ros
{

class Action5Node : public rclcpp::Node
{
public:
  RCLCPP_SMART_PTR_DEFINITIONS(Action5Node)

  static constexpr auto kDefaultNodeName = "action5";

  ACTION5_ROS_PUBLIC
  inline Action5Node(
    const std::string & node_name, const std::string & node_namespace,
    const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions())
  : rclcpp::Node(node_name, node_namespace, node_options)
  {
    image_raw_pub_ = this->create_publisher<ImageMsg>("image_raw", rclcpp::QoS(1).best_effort());
    image_compressed_pub_ = this->create_publisher<sensor_msgs::msg::CompressedImage>(
      "image_raw/compressed", rclcpp::QoS(1).best_effort());

    capture_.open(
      this->declare_parameter<std::string>(
        "dev", "/dev/v4l/by-id/usb-DJI_OsmoAction5pro_SN:D68A675E_123456789ABCDEF-video-index0"));

    frame_id_ = this->declare_parameter<std::string>("frame_id", "camera_frame");

    image_publish_timer_ = this->create_wall_timer(
      std::chrono::duration<double>(1.0 / this->declare_parameter<int>("rate", 30.0)),
      [this]() {this->publishImage();});
  }

  ACTION5_ROS_PUBLIC
  explicit inline Action5Node(
    const std::string & node_name, const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions())
  : Action5Node(node_name, "", node_options)
  {
  }

  ACTION5_ROS_PUBLIC
  explicit inline Action5Node(const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions())
  : Action5Node(kDefaultNodeName, "", node_options)
  {
  }

private:
  using ImageMsg = sensor_msgs::msg::Image;

  void publishImage()
  {
    if (!capture_.isOpened()) {
      RCLCPP_ERROR(this->get_logger(), "Failed to open camera device.");
      return;
    }

    cv::Mat frame;
    capture_ >> frame;

    if (frame.empty()) {
      RCLCPP_WARN(this->get_logger(), "Captured empty frame.");
      return;
    }

    auto header_msg = std_msgs::msg::Header(rosidl_runtime_cpp::MessageInitialization::SKIP);
    header_msg.frame_id = frame_id_;
    header_msg.stamp = this->now();

    auto cv_bridge_image = cv_bridge::CvImage(header_msg, "bgr8", frame);

    cv::imshow("camera", frame);

    auto image_compressed_msg = cv_bridge_image.toCompressedImageMsg();
    image_compressed_pub_->publish(*image_compressed_msg);

    auto image_msg = cv_bridge_image.toImageMsg();
    image_raw_pub_->publish(*image_msg);
    cv::waitKey(1);
  }

  // capture
  cv::VideoCapture capture_;

  // parameter
  std::string frame_id_;

  // image publisher
  rclcpp::Publisher<ImageMsg>::SharedPtr image_raw_pub_;
  rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr image_compressed_pub_;

  // publish timer
  rclcpp::TimerBase::SharedPtr image_publish_timer_;
};

}  // namespace action5_ros

#endif  // ACTION5_ROS__ACTION5_NODE_HPP_
