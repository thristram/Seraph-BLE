/*! \mainpage CSRmesh&trade; Documentation
 *  \section intro_sec Introduction
 *   CSRmesh is a Bluetooth Smart based system that allows multiple 
 *   Bluetooth Smart devices to associate to a Mesh network and 
 *   communicate with other devices that are also associated on the network.
 *
 * \section model_sec Models 
 *  CSRmesh&tm; implements several models which a device can support. Models 
 *  define the state and behaviour of a device and hence can be used to control
 *  and configure Mesh devices. 
 */
/*! \defgroup Stack CSRmesh&trade; */
/*! \ingroup Stack */
/*! \defgroup CSRmesh Core Stack*/
/*! \addtogroup CSRmesh 
 *  \brief Core Stack API
 *  @{
 */
/*! @} */

#define CSR101x

/*! \ingroup Stack */
/*! \defgroup Models Models */
/*! \ingroup Models */
/*! \defgroup Watchdog_Model Watchdog Model */
/*! \ingroup Watchdog_Model */
/*!     \defgroup Watchdog_Client Client */
/*! \ingroup Watchdog_Model */
/*!     \defgroup Watchdog_Server Server */
/*! \ingroup Models */
/*! \defgroup Config_Model Config Model */
/*! \ingroup Config_Model */
/*!     \defgroup Config_Client Client */
/*! \ingroup Config_Model */
/*!     \defgroup Config_Server Server */
/*! \ingroup Models */
/*! \defgroup Group_Model Group Model */
/*! \ingroup Group_Model */
/*!     \defgroup Group_Client Client */
/*! \ingroup Group_Model */
/*!     \defgroup Group_Server Server */
/*! \ingroup Models */
/*! \defgroup Sensor_Model Sensor Model */
/*! \ingroup Sensor_Model */
/*!     \defgroup Sensor_Client Client */
/*! \ingroup Sensor_Model */
/*!     \defgroup Sensor_Server Server */
/*! \ingroup Models */
/*! \defgroup Actuator_Model Actuator Model */
/*! \ingroup Actuator_Model */
/*!     \defgroup Actuator_Client Client */
/*! \ingroup Actuator_Model */
/*!     \defgroup Actuator_Server Server */
/*! \ingroup Models */
/*! \defgroup Data_Model Data Model */
/*! \ingroup Data_Model */
/*!     \defgroup Data_Client Client */
/*! \ingroup Data_Model */
/*!     \defgroup Data_Server Server */
/*! \ingroup Models */
/*! \defgroup Bearer_Model Bearer Model */
/*! \ingroup Bearer_Model */
/*!     \defgroup Bearer_Client Client */
/*! \ingroup Bearer_Model */
/*!     \defgroup Bearer_Server Server */
/*! \ingroup Models */
/*! \defgroup Ping_Model Ping Model */
/*! \ingroup Ping_Model */
/*!     \defgroup Ping_Client Client */
/*! \ingroup Ping_Model */
/*!     \defgroup Ping_Server Server */
/*! \ingroup Models */
/*! \defgroup Battery_Model Battery Model */
/*! \ingroup Battery_Model */
/*!     \defgroup Battery_Client Client */
/*! \ingroup Battery_Model */
/*!     \defgroup Battery_Server Server */
/*! \ingroup Models */
/*! \defgroup Attention_Model Attention Model */
/*! \ingroup Attention_Model */
/*!     \defgroup Attention_Client Client */
/*! \ingroup Attention_Model */
/*!     \defgroup Attention_Server Server */
/*! \ingroup Models */
/*! \defgroup Power_Model Power Model */
/*! \ingroup Power_Model */
/*!     \defgroup Power_Client Client */
/*! \ingroup Power_Model */
/*!     \defgroup Power_Server Server */
/*! \ingroup Models */
/*! \defgroup Light_Model Light Model */
/*! \ingroup Light_Model */
/*!     \defgroup Light_Client Client */
/*! \ingroup Light_Model */
/*!     \defgroup Light_Server Server */
/*! \ingroup Models */
/*! \defgroup Asset_Model Asset Model */
/*! \ingroup Asset_Model */
/*!     \defgroup Asset_Client Client */
/*! \ingroup Asset_Model */
/*!     \defgroup Asset_Server Server */
/*! \ingroup Models */
/*! \defgroup Tracker_Model Tracker Model */
/*! \ingroup Tracker_Model */
/*!     \defgroup Tracker_Client Client */
/*! \ingroup Tracker_Model */
/*!     \defgroup Tracker_Server Server */
/*! \ingroup Models */
/*! \defgroup Time_Model Time Model */
/*! \ingroup Time_Model */
/*!     \defgroup Time_Client Client */
/*! \ingroup Time_Model */
/*!     \defgroup Time_Server Server */
/*! \ingroup Models */
/*! \defgroup Switch_Model Switch Model */
/*! \ingroup Switch_Model */
/*!     \defgroup Switch_Client Client */
/*! \ingroup Switch_Model */
/*!     \defgroup Switch_Server Server */
/*! \ingroup Models */
/*! \defgroup Tuning_Model Tuning Model */
/*! \ingroup Tuning_Model */
/*!     \defgroup Tuning_Client Client */
/*! \ingroup Tuning_Model */
/*!     \defgroup Tuning_Server Server */
/*! \ingroup Models */
/*! \defgroup Extension_Model Extension Model */
/*! \ingroup Extension_Model */
/*!     \defgroup Extension_Client Client */
/*! \ingroup Extension_Model */
/*!     \defgroup Extension_Server Server */
/*! \ingroup Models */
/*! \defgroup LargeObjectTransfer_Model LargeObjectTransfer Model */
/*! \ingroup LargeObjectTransfer_Model */
/*!     \defgroup LargeObjectTransfer_Client Client */
/*! \ingroup LargeObjectTransfer_Model */
/*!     \defgroup LargeObjectTransfer_Server Server */
/*! \ingroup Models */
/*! \defgroup Firmware_Model Firmware Model */
/*! \ingroup Firmware_Model */
/*!     \defgroup Firmware_Client Client */
/*! \ingroup Firmware_Model */
/*!     \defgroup Firmware_Server Server */
/*! \ingroup Models */
/*! \defgroup Diagnostic_Model Diagnostic Model */
/*! \ingroup Diagnostic_Model */
/*!     \defgroup Diagnostic_Client Client */
/*! \ingroup Diagnostic_Model */
/*!     \defgroup Diagnostic_Server Server */
/*! \ingroup Models */
/*! \defgroup Action_Model Action Model */
/*! \ingroup Action_Model */
/*!     \defgroup Action_Client Client */
/*! \ingroup Action_Model */
/*!     \defgroup Action_Server Server */
/*! \ingroup Models */
/*! \defgroup Beacon_Model Beacon Model */
/*! \ingroup Beacon_Model */
/*!     \defgroup Beacon_Client Client */
/*! \ingroup Beacon_Model */
/*!     \defgroup Beacon_Server Server */
/*! \ingroup Models */
/*! \defgroup BeaconProxy_Model BeaconProxy Model */
/*! \ingroup BeaconProxy_Model */
/*!     \defgroup BeaconProxy_Client Client */
/*! \ingroup BeaconProxy_Model */
/*!     \defgroup BeaconProxy_Server Server */

/*! \page XAP_8bit_handler_page Handling 8 bit data types on XAP
 *
 * On the XAP, a "byte" is 16 bits, not 8 bits. It is not the same as an 
 * "octet". This extends to the C compiler: all of the types char, short, and
 * int are 16 bits, and sizeof (int) equals 1. The same holds good for the 8 bit
 * defined types of uint8 and int8.
 *
 * Any operations performed on the 8 bit variables are executed by the processor
 * as a 16-bit operation. So the result of 255+1 on a uint8 variable will be 
 * stored in the memory as 256 instead of 0.
 *
 * Hence the application developer must take proper care while passing the 8-bit 
 * data types as arguments to the CSRmesh API.
 *
 * In the case of a uint8 variable the upper byte must always be 0.
 * In the case of a int8 variable the upper byte must be either 0 or 0xFF.
 *
 * While using a signed 8-bit variable which has a negative value must be 
 * explicitly sign extended.
 * Example: if int8 a = (int8)0xFF the value will be 255 instead of -1. 
 * To correctly store -1 in an int8 type store it as either int8 a = (int8)-1, 
 * or int8 a = (int8)0xFFFF.
 */
