#include <pluginlib/class_list_macros.h>
#include "autorace/nodelet_autorace.h"
#include "autorace/nodelet_pcam.h"

PLUGINLIB_EXPORT_CLASS(autorace::NodeletAutorace, nodelet::Nodelet)
PLUGINLIB_EXPORT_CLASS(autorace::NodeletPcam, nodelet::Nodelet)