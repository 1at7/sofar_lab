/**
 * \file
 * \brief 
 * \author 
 * \version 0.1
 * \date 
 * 
 * \param[in] 
 * 
 * Subscribes to: <BR>
 *    ° 
 * 
 * Publishes to: <BR>
 *    ° 
 *
 * Description
 *
 */


//Cpp
#include <sstream>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <math.h>

//ROS
#include "ros/ros.h"
#include <tf/transform_listener.h>
#include <geometry_msgs/PointStamped.h>
#include <sensor_msgs/JointState.h>
#include "dual_planar_arm_msgs/DualArmIK.h"
#include "ros/service_client.h"


// Include here the ".h" files corresponding to the topic type you use.



int main (int argc, char** argv)
{         

	//ROS Initialization
    ros::init(argc, argv, "track_left_arm");

    // Define your node handles: YOU NEED AT LEAST ONE !
    ros::NodeHandle nh_glob, nh_loc("~") ;
    

    tf::TransformListener listener;
    dual_planar_arm_msgs::DualArmIK provider;
    geometry_msgs::PointStamped ip_Point;
    ip_Point.header.frame_id = "r_arm_base";


    // Declare your node's subscriptions and service clients
    ros::ServiceClient srvCl = nh_glob.serviceClient<dual_planar_arm_msgs::DualArmIK>("right_arm_IK_service");
    // Declare your publishers here.
    ros::Publisher right_pos = nh_glob.advertise<sensor_msgs::JointState>("/joint_command",1) ;

   
    ros::Rate rate(50);   // Or other rate.
    while (ros::ok()){
        ros::spinOnce();


        tf::StampedTransform transform;
        //dual_planar_arm_msgs::DualArmIK::Request req;
    	//dual_planar_arm_msgs::DualArmIK::Response resp;

    	try{
      		listener.lookupTransform("r_arm_base", "goal",  
                               ros::Time(0), transform);
      		ROS_INFO_STREAM("x cord : " << transform.getOrigin().x());
      		ROS_INFO_STREAM("y cord : " << transform.getOrigin().y());
    		}
    	catch (tf::TransformException ex){
     		 ROS_ERROR("%s",ex.what());
      		 ros::Duration(1.0).sleep();
      		 continue ;
    		}

    	//req.goal = ip_Point.point;
    	ip_Point.point.x = transform.getOrigin().x();
    	ip_Point.point.y = transform.getOrigin().y();
    	provider.request.goal = ip_Point;

        if (srvCl.exists()){
        	if (!srvCl.call(provider)){
        		ROS_ERROR("No service avilable. ");
        	}
        	else{

        		int sol_size = provider.response.solutions.size();
        		ROS_INFO_STREAM("Vector Size : " << sol_size);

        		if(sol_size >= 1){
				
					ROS_INFO_STREAM("Solution : " << provider.response.solutions[0]);
					right_pos.publish(provider.response.solutions[0]);
				}

        	}
        	
        }
        // Your code here.

        rate.sleep();
    }
}

