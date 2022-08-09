# Event-Camera

## Driver Installation

1.Install ROS dependencies:
```
sudo apt-get install ros-melodic-camera-info-manager
sudo apt-get install ros-melodic-image-view
```
2.Install libcaer:
```
sudo apt-get install libcaer-dev
```
3.Install catkin tools:
```
sudo apt-get install python-catkin-tools
```
4.Create a catkin workspace (if you have not done it yet):
```
mkdir -p davis/src
cd davis
catkin config --init --mkdirs --extend /opt/ros/melodic --merge-devel --cmake-args -DCMAKE_BUILD_TYPE=Release
```
5.Clone the catkin_simple package (https://github.com/catkin/catkin_simple), which will be used to build the DVS/DAVIS driver packages:
```
cd src
git clone https://github.com/catkin/catkin_simple.git
```

6.Clone this(rpg_dvs_ros) repository:
```
git clone https://github.com/uzh-rpg/rpg_dvs_ros.git
```
7.Build the packages:
```
catkin build davis_ros_driver
```

8.You can test the installation by running a provided launch file. It starts the driver (DVS or DAVIS) and the renderer (an image viewer).
```
  # i. First, build the renderer:
catkin build dvs_renderer
# ii. Set up the environment:
source ~/davis/devel/setup.bash
# iii. Then, launch the example:
roslaunch dvs_renderer davis_mono.launch
```
9.If you do not have a DAVIS, you can still use this driver to read recorded files, such as those of The Event Camera Dataset and Simulator. Example:
```
  # i. Download a squence of the dataset, such as slider_depth.bag
  # ii. Open a terminal and launch the roscore:
	roscore
	# iii. In another terminal, play the bag:
	rosbag play -l path-to-file/slider_depth.bag
	# iv. In another terminal, launch the DVS/DAVIS renderer:
	roslaunch dvs_renderer renderer_mono.launch 
```
