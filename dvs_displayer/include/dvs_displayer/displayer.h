#pragma once

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <dvs_msgs/EventArray.h>
#include <opencv2/core/core.hpp>
#include <vector>
#include <list>

using std::vector;

namespace dvs_displayer
{

class Cluster{
private:
  int cx, cy; 
  double min_Rc, max_Rk, Rk, Rc, weight;
  bool delete_;
  cv::Mat pts;
  cv::Mat prev_pts;
  cv::Mat path;
  friend class Displayer;

public:
  Cluster(int& x, int& y);
  double distance(int, int);
  int clust_assign(cv::Mat& newevents_, cv::Mat* counts_); // returning the next center
  void new_center(int& x, int& y);
  void new_Rc(double&);
  void new_Rk();
  void cluster_resize();
  vector<int> bounding_box();

  void info();
};



class Displayer {
public:
  Displayer(ros::NodeHandle& nh, ros::NodeHandle nh_private); 
  virtual ~Displayer();

  void cluster_check();

private:
  ros::NodeHandle nh_;

  double weight_thres;

  // Callback functions
  void eventsCallback(const dvs_msgs::EventArray::ConstPtr& msg);
  // void imageCallback(const sensor_msgs::Image::ConstPtr& msg);

  // Subscribers
  ros::Subscriber event_sub_;
  // image_transport::Subscriber image_sub_;

  // Publishers
  image_transport::Publisher image_pub_;
  image_transport::Publisher image_pub_1;
  // image_transport::Publisher image_pub_2;

};

} // namespace
