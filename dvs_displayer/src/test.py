#-*- coding: utf-8 -*- 

import os

import cv2
from torch import float32
from cv_bridge import CvBridge

import numpy as np

import rospy
from sensor_msgs.msg import Image
from std_msgs.msg import Float32
from dv import NetworkEventInput

class EVENT:
    def __init__(self):
        self.pub = rospy.Publisher('events', Float32, queue_size=10)
    def callback(self, msg):
        # rospy.loginfo('Got image')

        print(msg)
        # rospy.loginfo('Publishing image')
        # self.pub.publish(msg)
        # print(type(msg))

    def subscriber(self):
        rospy.Subscriber("/events_off_mean_1", Float32, self.callback)


if __name__ == '__main__':
    rospy.init_node('IMGGET')
    try:
        event_obj = EVENT()
        event_obj.subscriber()

        rospy.spin()
    except rospy.ROSInterruptException:
        pass