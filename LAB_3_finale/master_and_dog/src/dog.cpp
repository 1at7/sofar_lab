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

// Include here the ".h" files corresponding to the topic type you use.
// ...
#include <std_msgs/Int16.h>
#include <geometry_msgs/Point.h>
#include <std_msgs/Float64.h>
#include <visualization_msgs/Marker.h>


#define DEFAULT_INPUT 0.0
// You may have a number of globals here.
//...
geometry_msgs::Point virtualLeader, previousLeaderPos;

// Callback functions...

/* Define here the variable which hold the message */

geometry_msgs::Point goal_pos_msg ;
bool virtualLeaderInfoReceived     = false ;
void virtualLeaderCallback(geometry_msgs::Point goal_pos_msg){
    // Copy goal information into global variables
    virtualLeader.x = goal_pos_msg.x ;
    virtualLeader.y = goal_pos_msg.y ;
    virtualLeader.z = goal_pos_msg.z ;
    virtualLeaderInfoReceived = true ;
}

ros::Publisher pubMarker ;
visualization_msgs::Marker marker;

void initializeMarker( ){
    // Fetch node name. Markers will be blue if the word "blue" is in the name, red otherwise.
    std::string nodeName ;
    nodeName = ros::this_node::getName();
    // Create a marker for this node. Only timestamp and position will be later updated.
    marker.header.frame_id = "/map";
    marker.ns = nodeName;
    marker.id = 0;
    marker.type = visualization_msgs::Marker::SPHERE;
    marker.action = visualization_msgs::Marker::ADD;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;
    marker.scale.x = 0.2;
    marker.scale.y = 0.2;
    marker.scale.z = 0.2;
    if( nodeName.find("blue") != std::string::npos ){
        marker.color.r = 0.0f;
        marker.color.g = 0.0f;
        marker.color.b = 1.0f;
        marker.color.a = 0.5;
    }else{
        marker.color.r = 1.0f;
        marker.color.g = 0.0f;
        marker.color.b = 0.0f;
        marker.color.a = 0.5;
    }
}

void publishMarkerAt( geometry_msgs::Point markerPos) {    
    marker.header.stamp = ros::Time::now();
    marker.pose.position.x = markerPos.x ;
    marker.pose.position.y = markerPos.y ;
    marker.lifetime = ros::Duration();
    pubMarker.publish(marker);
}



double speed ;
void kbdCallback( std_msgs::Float64 speed_msg ) {
    speed = speed_msg.data;
}

    // ... Callback function code



int main (int argc, char** argv)
{

    //ROS Initialization
    ros::init(argc, argv, "dog");

    // Define your node handles
    // Define your node handles
    ros::NodeHandle nh_loc("~") ;
    ros::NodeHandle nh_glob ;

    // Read the node parameters if any
    // ...

    // Declare your node's subscriptions and service clients
    // ...
    ros::Subscriber subToGoal = 
        nh_glob.subscribe<geometry_msgs::Point>("/master_pos",1,virtualLeaderCallback);
    ros::Subscriber subToVel = 
        nh_glob.subscribe<std_msgs::Float64>("/master_vel",1,kbdCallback);

    // Declare you publishers and service servers
    // ...
    ros::Publisher pubPos = nh_glob.advertise<geometry_msgs::Point>("/pos",1);
    ros::Publisher pubVel = nh_glob.advertise<std_msgs::Float64>("/vel",1);
    pubMarker = nh_glob.advertise<visualization_msgs::Marker>("/visualization_marker",1) ;



    geometry_msgs::Point pos ;
    double input_x,input_y;
    nh_loc.param("input_x"  , input_x , DEFAULT_INPUT);
    //ROS_INFO("input_x: %d",input_x) ;

    nh_loc.param("input_y"  , input_y , DEFAULT_INPUT);
    //ROS_INFO("input_y: %d",input_y) ;

    pos.x = input_x ;  pos.y = input_y ; pos.z = 0.0 ; 
    publishMarkerAt( pos ) ; 

    std_msgs::Float64 dogSpeed;
    

    initializeMarker() ;
    //double speed = 1.0;


    ros::Rate rate(50);   // Or other rate.
    ros::Time currentTime, prevTime = ros::Time::now() ; 
    while (ros::ok()){

        ros::spinOnce();
	currentTime = ros::Time::now() ;
        ros::Duration timeElapsed = currentTime - prevTime ;
        prevTime = currentTime ;

        if( virtualLeaderInfoReceived ){
            //if( (virtualLeader.x!=previousLeaderPos.x) || (virtualLeader.y!=previousLeaderPos.y) ){
                // Set a goal close to virtual leader position 
                //goal.x = virtualLeader.x + ((1.0*rand())/RAND_MAX-0.5)*0.3*worldSize ;
                //goal.y = virtualLeader.y + ((1.0*rand())/RAND_MAX-0.5)*0.3*worldSize ;
                //goal.z = 0.0 ;
                //virtualLeaderInfoReceived = false ;
                //previousLeaderPos = virtualLeader ;
            //}
		double norm = sqrt(pow((virtualLeader.x - pos.x),2) + pow((virtualLeader.y - pos.y),2));
		pos.x += ((virtualLeader.x - pos.x)/norm) *speed *timeElapsed.toSec() ;
        	pos.y += ((virtualLeader.y - pos.y)/norm) *speed *timeElapsed.toSec()  ;
		dogSpeed.data = speed ;
		
        }

        // Control swarm member.
        

        pubPos.publish(pos) ;
	pubVel.publish(dogSpeed) ;

        // Publish a marker to visualize under Rviz
        publishMarkerAt( pos ) ;
            
        rate.sleep();
    }
}
