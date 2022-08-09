#include "dvs_displayer/displayer.h"
// #include "dvs_displayer/custom_cmaps.h"
#include <cv_bridge/cv_bridge.h>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <opencv2/imgcodecs.hpp> 
#include <random>

using namespace std;
using namespace cv;

namespace dvs_displayer {


list<Cluster> list_;
int q = 0;
double alpha_x = 0.8;
double alpha_R = 0.7;
double R_multiple = 2;

Displayer::Displayer(ros::NodeHandle& nh, ros::NodeHandle nh_private) : nh_(nh){
  //! Set up subscribers and publishers
  event_sub_ = nh_.subscribe("events", 1, &Displayer::eventsCallback, this);

  image_transport::ImageTransport it_(nh_);
  // image_sub_ = it_.subscribe("image", 1, &Displayer::imageCallback, this);
  image_pub_ = it_.advertise("event_image", 1);
  image_pub_1 = it_.advertise("raw_event_image", 1);
  // image_pub_2 = it_.advertise("raw_image", 1);

  weight_thres = 100;
}


Displayer::~Displayer(){
  image_pub_.shutdown();
}


// void Displayer::imageCallback(const sensor_msgs::Image::ConstPtr& msg)
// {
//   cout<<1<<endl;
//   cv_bridge::CvImagePtr cv_ptr;
//   cv_bridge::CvImage raw_img;

//   try
//   {
//     cv_ptr = cv_bridge::toCvCopy(msg);
//   }
//   catch (cv_bridge::Exception& e)
//   {
//     ROS_ERROR("cv_bridge exception: %s", e.what());
//     return;
//   }

//   // Convert to BGR image
//   if (msg->encoding == "mono8")
//   {
//     cv::cvtColor(cv_ptr->image, raw_img.image, CV_GRAY2BGR);
//   }

//   cv_bridge::CvImage cv_image;
//   raw_img.image.copyTo(cv_image.image);
//   cv_image.encoding = "bgr8";
//   image_pub_2.publish(cv_image.toImageMsg());
// }


void Displayer::eventsCallback(const dvs_msgs::EventArray::ConstPtr& msg){
  // Exit if there are no subscribers
  // if (image_pub_.getNumSubscribers() <= 0)
  // {
  //   return;
  // }
  
  //! Create image if at least one subscriber
  cv_bridge::CvImage cv_image;
  cv_bridge::CvImage cv_image_1;
  if (msg->events.size() == 0){
    cv_image.header.stamp = msg->events[msg->events.size()/2].ts;
    cv_image_1.header.stamp = msg->events[msg->events.size()/2].ts;
  }

// --------------------------------------------------------------------
  //! Put subscribed events in the matrix 'newevents'
  cv::Mat newevents;
  // cv::Mat newevents_1;
  vector<int> temp;
  for (const dvs_msgs::Event& ev : msg->events){
    // if (ev.polarity > 0){
      temp = {ev.y, ev.x};
      cv::Mat m(cv::Mat(temp).t());
      newevents.push_back(m);
    // }
    // else{
    //   newevents_1.push_back(m);
    // }
    

    // cout<<ev.ts<<endl;
    
    // vector<double> ts_temp = {ev.ts};
    // cv::Mat m(cv::Mat(ts_temp).t());
    // ts.push_back(m);
    // if (aa < 3){
    //   temp1.push_back(m);
    //   aa++;
    // }
  }
  // cout<<endl<<endl;
  // cout<<temp1<<endl;
  // cout<<temp1.at<int>(0,0)<<endl;
  // cout<<temp1.at<int>(0,1)<<endl;

  // cv::Mat t = cv::Mat(3,2,CV_8U);
  // cout<<t<<endl;
  // cout<<t.at<int>(0,1)<<endl;
  // cout<<t.at<int>(0,2)<<endl;

  // cout<<"New events received"<<endl;
  // cout<<"Initial # of Cluster: "<<list_.size()<<endl;
  //! If there's no cluster, create one when the new callback has been called.
  if (list_.size() == 0){
    srand(time(NULL));
    int random = rand() % newevents.rows;
    Cluster c1(newevents.at<int>(random,0), newevents.at<int>(random,1));
    list_.push_back(c1);
  }
  
  //! matrix for checking whether all events have been processed (assigned to any of cluster).
  cv::Mat counts = cv::Mat::zeros(newevents.rows, 1, CV_32S); // cluster assign check
  int nonzero = cv::countNonZero(counts);

  list<Cluster>::iterator it = list_.begin(); 
  int next_center;

  //! Processing all events
  while(nonzero != counts.rows){   

    next_center = it->clust_assign(newevents, &counts); //! Assigning events into the clusters depends on the seek size (Rk).
    it->cluster_resize(); //! Remove events which are out of the cluster size (Rc) from clusters.
    nonzero = cv::countNonZero(counts);
    if (it == --(list_.end()) && nonzero != counts.rows){
      list_.push_back(Cluster(newevents.at<int>(next_center,0), newevents.at<int>(next_center,1)));
    }
    ++it;
    
  }
  cluster_check();

  cv::Mat box_corners; //! Matrix containing four coordinates of bounding box
  it = list_.begin();
  while(it != list_.end()){
    temp = it->bounding_box();
    cv::Mat m(cv::Mat(temp).t());
    box_corners.push_back(m);
    ++it;
  }
  // cout<<"Bounding BOX"<<endl;
  // cout<<"# of box: "<<box_corners.rows<<endl;
  // cout<<box_corners<<endl;
  
  cout<<"Done Clustering"<<endl;
  cout<<"# of Cluster: "<<list_.size()<<endl;
// --------------------------------------------------------------------
  cv_image.encoding = "bgr8";
  cv_image_1.encoding = "bgr8";
  cv_image.image = cv::Mat(msg->height, msg->width, CV_8UC3, cv::Scalar(255,255,255)); //! Create image
  cv_image_1.image = cv::Mat(msg->height, msg->width, CV_8UC3, cv::Scalar(255,255,255));
  
  if (list_.size() != 0){
    int i = 0;
    for (it = list_.begin(); it != list_.end(); ++it, ++i){
      int loop = -1;

      std::random_device rd;
      //! random_device 를 통해 난수 생성 엔진을 초기화
      std::mt19937 gen(rd());
      //! 0 부터 99 까지 균등하게 나타나는 난수열을 생성하기 위해 균등 분포 정의
      std::uniform_int_distribution<int> dis(0, 255);
      int r = dis(gen);
      int g = r%100;
      int b = r%50;
      
      while(loop != it->pts.rows-1){
        ++loop;
        cv_image.image.at<cv::Vec3b>(it->pts.at<int>(loop,0), it->pts.at<int>(loop,1)) = cv::Vec3b(r,g,b);

      }
      //! Drawing Box
      Point pt1(box_corners.at<int>(i,0), box_corners.at<int>(i,1));
      Point pt2(box_corners.at<int>(i,2), box_corners.at<int>(i,3));
      rectangle(cv_image.image, pt1, pt2, Scalar(0,0,255));
      //! Drawing path
      loop = 0;
      while(loop != it->path.rows){
        cout<<it->path<<endl;
        if (loop != it->path.rows-1)
          line(cv_image.image, Point(it->path.at<int>(loop,1), it->path.at<int>(loop,0)), Point(it->path.at<int>(loop+1,1), it->path.at<int>(loop+1,0)), Scalar(0,255,0));\
        else
          line(cv_image.image, Point(it->path.at<int>(loop,1), it->path.at<int>(loop,0)), Point(it->cy, it->cx), Scalar(0,255,0));
        ++loop;
      }
      
      //! Center point
      circle(cv_image.image, Point(it->cy, it->cx), 5, Scalar(100,100,100));

    }
    ++q;
  }
  
  int loop = 0;
  while(loop != newevents.rows){
    cv_image_1.image.at<cv::Vec3b>(newevents.at<int>(loop,0), newevents.at<int>(loop,1)) = cv::Vec3b(0,0,255);
    ++loop;
  }
  // loop = 0;
  // while(loop != newevents_1.rows){
  //   cv_image_1.image.at<cv::Vec3b>(newevents_1.at<int>(loop,0), newevents_1.at<int>(loop,1)) = cv::Vec3b(0,0,255);
  //   ++loop;
  // }

  image_pub_.publish(cv_image.toImageMsg());
  image_pub_1.publish(cv_image_1.toImageMsg());
}


Cluster::Cluster(int& x, int& y){
  cx = x;
  cy = y;
  max_Rk = 50.0; 
  min_Rc = 15.0;
  Rc = min_Rc;
  Rk = 40;
  weight = 0;
  delete_ = 0;
}


double Cluster::distance(int x_, int y_){
  return sqrt(pow((cx - x_),2) + pow((cy - y_),2));
}


int Cluster::clust_assign(cv::Mat& newevents_, cv::Mat* counts_){
  int next_center;
  double farthest = 0;

  //! Put center coor. into the path matrix
  // if (q % 10 == 0){
    vector<int> temp = {cx, cy};
    cv::Mat m(cv::Mat(temp).t());
    path.push_back(m);
  // }

  // vector<int> idx;
  pts.copyTo(prev_pts);
  double R;
  for (int i = 0; i < newevents_.rows; i++){
    if (counts_->at<int>(i) == 0){
      double dist = distance(newevents_.at<int>(i,0), newevents_.at<int>(i,1));
      if (dist <= Rk){
        vector<int> temp = {newevents_.at<int>(i,0), newevents_.at<int>(i,1)};
        cv::Mat m(cv::Mat(temp).t());
        pts.push_back(m);
        counts_->at<int>(i) = 1;
        //! new center assign
        new_center(newevents_.at<int>(i,0), newevents_.at<int>(i,1));
        //! Compute R
        R = distance(newevents_.at<int>(i,0), newevents_.at<int>(i,1));
        //! Will be used when recomputing Rc and Rk
        // idx.push_back(i);
        new_Rc(R);
      }
      else{
        R = distance(newevents_.at<int>(i,0), newevents_.at<int>(i,1));
        if (R > farthest){
          farthest = R;
          next_center = i;
        }        
      }
    }
  }
  if (pts.rows == prev_pts.rows){
    cout<<"SAME"<<endl;
    delete_ = 1;
  }
  new_Rk();
  // //! Recompute Rc and Rk
  // if (idx.size() != 0){
  //   vector<int>::iterator it = idx.begin();
  //   double max_R = distance(newevents_.at<int>(*it,0), newevents_.at<int>(*it,1));
  //   for (it += 1; it != idx.end(); it++){
  //     R = distance(newevents_.at<int>(*it,0), newevents_.at<int>(*it,1));
  //     if (R > max_R)
  //       max_R = R;
  //   }
  //   cout<<max_R<<endl;
  //   new_Rc(max_R);
  //   new_Rk();
  // }
  // cout<<"NEW Rc: "<<Rc<<endl;
  // cout<<"NEW Rk: "<<Rk<<endl;
  return next_center;
}


void Cluster::cluster_resize(){
  cv::Mat new_pts;
  int loop = 0;
  if (pts.rows != 0){
    while(loop != pts.rows-1){
      double dist = distance(pts.at<int>(loop,0), pts.at<int>(loop,1));
      if (dist <= Rc){
        vector<int> temp = {pts.at<int>(loop,0), pts.at<int>(loop,1)};
        cv::Mat m(cv::Mat(temp).t());
        new_pts.push_back(m);
      }
      loop++;
    }
  pts = new_pts;
  }
  weight = pts.rows;
}


void Displayer::cluster_check(){
  list<Cluster>::iterator it = list_.begin(); 
  // int i = 0;
  // cout<<"Cluster Checking"<<endl;
  // cout<<"Before removing: # of Cluster: "<<list_.size()<<endl;
  if (list_.size() != 0){
    while(it != list_.end()){
      if (it->weight < weight_thres || it->delete_){
        it = list_.erase(it);
        // ++i;
      }
      else
        ++it;
      // if (it->delete_){ 
      //   // it = list_.erase(it);
      //   cout<<1111<<endl;
      // }
    }
  }
  // cout<<i<<" cluster deleted"<<endl;
}


vector<int> Cluster::bounding_box(){
  if (pts.rows != 0){
    // int loop = 0;
    // int max_x = pts.at<int>(0,1);
    // int max_y = pts.at<int>(0,0);
    // int min_x = pts.at<int>(0,1);
    // int min_y = pts.at<int>(0,0);
    // while(loop != pts.rows-1){
    //   max_x = max(max_x, pts.at<int>(loop,1));
    //   max_y = max(max_y, pts.at<int>(loop,0));
    //   min_x = min(min_x, pts.at<int>(loop,1));
    //   min_y = min(min_y, pts.at<int>(loop,0));
    //   ++loop;
    // }
    int min_x = cy - (int)Rc;
    int min_y = cx - (int)Rc;
    int max_x = cy + (int)Rc;
    int max_y = cx + (int)Rc;
    return vector<int>{ min_x, min_y, max_x, max_y };
  }
}

void Cluster::new_center(int& x, int& y){
  cx = alpha_x*cx + (1-alpha_x)*x;
  cy = alpha_x*cy + (1-alpha_x)*y;
}

void Cluster::new_Rc(double& R_){
  Rc = max(min_Rc, (Rc*alpha_R + R_*(1-alpha_R)));
  // cout<<"NEW Rc: "<<Rc<<endl;
}

void Cluster::new_Rk(){
  Rk = min(max_Rk, Rc*R_multiple);  
  // cout<<"NEW Rk: "<<R<<endl;

}

void Cluster::info(){
  cout<<cx<<","<<cy<<endl;
}

} // namespace