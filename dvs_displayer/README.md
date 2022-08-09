# dvs_displayer

### Run
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

In another terminal open the dynamic reconfigure and play around with the parameters in the window named `dvs_displayer_one`

	rosrun rqt_reconfigure rqt_reconfigure
