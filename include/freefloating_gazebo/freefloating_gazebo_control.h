#ifndef FREEFLOATINGGAZEBOCONTROL_H
#define FREEFLOATINGGAZEBOCONTROL_H

#include <gazebo/common/Plugin.hh>
#include <ros/ros.h>
#include <ros/callback_queue.h>
#include <geometry_msgs/Wrench.h>
#include <geometry_msgs/Vector3.h>
#include <std_msgs/Float32MultiArray.h>
#include <sensor_msgs/JointState.h>
#include <gazebo_msgs/ModelState.h>
#include <std_msgs/Float32MultiArray.h>
#include <std_srvs/Empty.h>
#include <eigen3/Eigen/Core>
#include <freefloating_gazebo/thruster_mapper.h>

namespace gazebo
{

class FreeFloatingControlPlugin : public ModelPlugin
{

public:
    FreeFloatingControlPlugin() {}
    ~FreeFloatingControlPlugin()
    {
        rosnode_.shutdown();
        //    delete rosnode_;
    }

    void Load(physics::ModelPtr _model, sdf::ElementPtr _sdf);
    void Update();

private:

    // parse received joint command (joint states)
    void JointCommandCallBack(const sensor_msgs::JointStateConstPtr &_msg)
    {
        if(!control_joints_)
            return;
        // store received joint state
        joint_command_ = *_msg;
        joint_command_received_ = true;
    }
    // parse received body command
    void BodyCommandCallBack(const geometry_msgs::WrenchConstPtr& _msg);
    // parse received thruster command
    void ThrusterCommandCallBack(const sensor_msgs::JointStateConstPtr& _msg);
    // parse switch service
    bool SwitchService(std_srvs::EmptyRequest &req, std_srvs::EmptyResponse &res);

private:
    // -- general data ----------------------------------------
    std::string robot_namespace_;
    ros::NodeHandle rosnode_;
    ros::CallbackQueue callback_queue_;
    physics::ModelPtr model_;
    event::ConnectionPtr update_event_;
    ros::ServiceServer switch_service_;
    bool controller_is_running_;
    double update_T_;
    double z_surface_;

    // -- body control ----------------------------------------
    // model body data
    physics::LinkPtr body_;
    bool control_body_, wrench_control_;

    // thruster control
    ffg::ThrusterMapper mapper_;
    std::vector<physics::LinkPtr> thruster_links_;
    Eigen::VectorXd thruster_command_;

    // subscriber
    ros::Subscriber body_command_subscriber_;
    std::string body_command_topic_;
    bool body_command_received_;

    // -- joint control ----------------------------------------
    // model joint data
    std::vector<physics::JointPtr> joints_;
    bool control_joints_;

    // subscriber
    ros::Subscriber joint_command_subscriber_;
    std::string joint_command_topic_;
    sensor_msgs::JointState joint_command_;
    bool joint_command_received_;

    // -- publishers ----------------------------------------

    // publisher to joint state
    ros::Publisher joint_state_publisher_;
    sensor_msgs::JointState joint_states_;
    double t_prev_;

    // publisher to thruster percent use
    ros::Publisher thruster_use_publisher_;
    sensor_msgs::JointState thruster_use_;

};
GZ_REGISTER_MODEL_PLUGIN(FreeFloatingControlPlugin)
}
#endif // FREEFLOATINGGAZEBOCONTROL_H
