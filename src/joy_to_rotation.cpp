/**
	
 */

#include <ros/ros.h>
#include <sensor_msgs/Joy.h>
#include <std_msgs/Float64.h>

// mode
#define LOGICOOL

std_msgs::Float64 carrier_velocity, child_R_velocity, child_L_velocity;	//unit:[rad](tabun)

double base_vel;

void JoyCallBack(const sensor_msgs::Joy &msg) {
#ifdef LOGICOOL
  carrier_velocity.data = base_vel * msg.axes[1];
  if (msg.axes[7]==1){
    child_R_velocity.data = (-1)*base_vel*(1-msg.axes[5]);
		child_L_velocity.data = (-1)*base_vel*(1-msg.axes[2]);
	}
  else if (msg.axes[7]==-1){
    child_R_velocity.data = base_vel*(1-msg.axes[5]);
		child_L_velocity.data = base_vel*(1-msg.axes[2]);
	}
  else{
		child_R_velocity.data = 0;
		child_L_velocity.data = 0;
	}
#endif

  ROS_INFO_STREAM_ONCE("HAVE JOY !!");
  // ROS_INFO("x=%f y=%f theta=%f",velocity.x,velocity.y,velocity.z);
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "joy_to_rotation");
  ros::NodeHandle n;
  ros::NodeHandle local_nh("~");

  ros::Subscriber joy_sub = n.subscribe("/joy", 1, JoyCallBack);
  ros::Publisher carrier_vel_pub =
      n.advertise<std_msgs::Float64>("/carrier/carrier_wheel_velocity_controller/command", 1);
	ros::Publisher child_L_vel_pub =
      n.advertise<std_msgs::Float64>("/carrier/child_wheel_L_velocity_controller/command", 1);
	ros::Publisher child_R_vel_pub =
      n.advertise<std_msgs::Float64>("/carrier/child_wheel_R_velocity_controller/command", 1);
  ros::Rate loop_rate(30);

  if (!local_nh.hasParam("/carrier/base_vel")) {
    ROS_INFO("Parameter /carrier/base_vel is not defined. Now, it is set default "
             "value");
    local_nh.setParam("/carrier/base_vel", 1.0);
  }
  if (!local_nh.getParam("/carrier/base_vel", base_vel)) {
    ROS_ERROR("No value set on base_vel");
    return -1;
  }

  while (ros::ok()) {
    carrier_vel_pub.publish(carrier_velocity);
		child_L_vel_pub.publish(child_L_velocity);
		child_R_vel_pub.publish(child_R_velocity);
    ros::spinOnce();
    loop_rate.sleep();
  }
}
