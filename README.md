# Event-Camera

#### Clone this repository under 'src'.

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
5.Clone or download the catkin_simple package (https://github.com/catkin/catkin_simple), which will be used to build the DVS/DAVIS driver packages:
```
cd src
# Cloning
git clone https://github.com/catkin/catkin_simple.git
# or Downloading as ZIP file
```

6.Clone or download this(rpg_dvs_ros) repository:
```
# Cloning
git clone https://github.com/uzh-rpg/rpg_dvs_ros.git
# or Downloading as ZIP file
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
9.If you do not have a DAVIS, you can still use this driver to read recorded files(rosbag), such as those of The Event Camera Dataset and Simulator. Example:
```
# i. Download a squence of the dataset, such as slider_depth.bag
# ii. Open a terminal and launch the roscore:
roscore
# iii. In another terminal, play the bag:
rosbag play -l path-to-file/slider_depth.bag
# iv. In another terminal, launch the DVS/DAVIS renderer:
roslaunch dvs_renderer renderer_mono.launch 
```

## Vehicle Tracking
Prepare rosbag files.

Every time that you open a terminal, you may need to run:

	source ~/catkin_ws/devel/setup.bash

to have access to the ROS commands.

Run in a terminal (you may need to "source" first):

	roscore

In another terminal play the bag with the event data:

	rosbag play -l path_to_file/11.bag

Then, in another terminal run the visualizer node:

	roslaunch dvs_displayer display_monocular.launch

In another terminal open the run:

	rqt_image_view
to see 'raw_events' topic.
