
#ifndef IC4_C_PROPERTY_CONSTANTS_H_INC_
#define IC4_C_PROPERTY_CONSTANTS_H_INC_

/** @defgroup properties Properties
 * 
 * @{
 * 
 * @defgroup propid Property Identifiers
 * 
 * @brief A lot of common property identifiers are described here.
 * 
 * @{
 */

/**
 * @brief Number of frames to acquire for each trigger.
 */
#define IC4_PROPID_ACQUISITION_BURST_FRAME_COUNT "AcquisitionBurstFrameCount"

/**
 * @brief Minimum interval between frames in an acquisition burst.
 */
#define IC4_PROPID_ACQUISITION_BURST_INTERVAL "AcquisitionBurstInterval"

/**
 * @brief Controls the acquisition rate at which the frames are captured.
 */
#define IC4_PROPID_ACQUISITION_FRAME_RATE "AcquisitionFrameRate"

/**
 * @brief Sets the acquisition mode of the device. It defines mainly the number of frames to capture during an acquisition and the way the acquisition stops.
 */
#define IC4_PROPID_ACQUISITION_MODE "AcquisitionMode"

/**
 * @brief Starts the Acquisition of the device. The number of frames captured is specified by AcquisitionMode.
 */
#define IC4_PROPID_ACQUISITION_START "AcquisitionStart"

/**
 * @brief Stops the Acquisition of the device at the end of the current Frame. It is mainly used when AcquisitionMode is Continuous but can be used in any acquisition mode.
 */
#define IC4_PROPID_ACQUISITION_STOP "AcquisitionStop"

/**
 * @brief Provides the device key that allows the device to check the validity of action commands. The device internal assertion of an action signal is only authorized if the ActionDeviceKey and the action device key value in the protocol message are equal.
 */
#define IC4_PROPID_ACTION_DEVICE_KEY "ActionDeviceKey"

/**
 * @brief Provides the key that the device will use to validate the action on reception of the action protocol message.
 */
#define IC4_PROPID_ACTION_GROUP_KEY "ActionGroupKey"

/**
 * @brief Provides the mask that the device will use to validate the action on reception of the action protocol message.
 */
#define IC4_PROPID_ACTION_GROUP_MASK "ActionGroupMask"

/**
 * @brief Indicates the size of the scheduled action commands queue. This number represents the maximum number of scheduled action commands that can be pending at a given point in time.
 */
#define IC4_PROPID_ACTION_QUEUE_SIZE "ActionQueueSize"

/**
 * @brief Cancels all scheduled actions.
 */
#define IC4_PROPID_ACTION_SCHEDULER_CANCEL "ActionSchedulerCancel"

/**
 * @brief Schedules the action to be executed time specified by ActionSchedulerTime, with optional repetition in intervals of ActionSchedulerInterval.
 */
#define IC4_PROPID_ACTION_SCHEDULER_COMMIT "ActionSchedulerCommit"

/**
 * @brief Action repetition interval. A value of 0 schedules the action to be executed only once.
 */
#define IC4_PROPID_ACTION_SCHEDULER_INTERVAL "ActionSchedulerInterval"

/**
 * @brief Indicates whether there are actions scheduled.
 */
#define IC4_PROPID_ACTION_SCHEDULER_STATUS "ActionSchedulerStatus"

/**
 * @brief Camera time for when the action is to be scheduled.
 */
#define IC4_PROPID_ACTION_SCHEDULER_TIME "ActionSchedulerTime"

/**
 * @brief Selects to which Action Signal further Action settings apply.
 */
#define IC4_PROPID_ACTION_SELECTOR "ActionSelector"

/**
 * @brief Enable the region of interest for auto focus.
 */
#define IC4_PROPID_AUTO_FOCUS_ROI_ENABLE "AutoFocusROIEnable"

/**
 * @brief Vertical size of the auto focus region of interest.
 */
#define IC4_PROPID_AUTO_FOCUS_ROI_HEIGHT "AutoFocusROIHeight"

/**
 * @brief Horizontal offset of the auto focus region of interest.
 */
#define IC4_PROPID_AUTO_FOCUS_ROI_LEFT "AutoFocusROILeft"

/**
 * @brief Vertical offset of the auto focus region of interest.
 */
#define IC4_PROPID_AUTO_FOCUS_ROI_TOP "AutoFocusROITop"

/**
 * @brief Horizontal size of the auto focus region of interest.
 */
#define IC4_PROPID_AUTO_FOCUS_ROI_WIDTH "AutoFocusROIWidth"

/**
 * @brief Enable the region of interest for auto functions.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_ENABLE "AutoFunctionsROIEnable"

/**
 * @brief Vertical size of the auto functions region of interest.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_HEIGHT "AutoFunctionsROIHeight"

/**
 * @brief Horizontal offset of the auto functions region of interest.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_LEFT "AutoFunctionsROILeft"

/**
 * @brief Select a predefined region of interest for auto functions.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_PRESET "AutoFunctionsROIPreset"

/**
 * @brief Vertical offset of the auto functions region of interest.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_TOP "AutoFunctionsROITop"

/**
 * @brief Horizontal size of the auto functions region of interest.
 */
#define IC4_PROPID_AUTO_FUNCTIONS_ROI_WIDTH "AutoFunctionsROIWidth"

/**
 * @brief Controls ratio of the selected color component to a reference color component. It is used for white balancing.
 */
#define IC4_PROPID_BALANCE_RATIO "BalanceRatio"

/**
 * @brief This enumeration selects a balance ratio control to configuration. Once a balance ratio control has been selected, all changes to the balance ratio settings will be applied to the selected control.
 */
#define IC4_PROPID_BALANCE_RATIO_SELECTOR "BalanceRatioSelector"

/**
 * @brief Controls the mode for automatic white balancing between the color channels. The white balancing ratios are automatically adjusted.
 */
#define IC4_PROPID_BALANCE_WHITE_AUTO "BalanceWhiteAuto"

/**
 * @brief Selects a specific preset for automatic white balance.
 */
#define IC4_PROPID_BALANCE_WHITE_AUTO_PRESET "BalanceWhiteAutoPreset"

/**
 * @brief Configures the way auto white balance works.
 */
#define IC4_PROPID_BALANCE_WHITE_MODE "BalanceWhiteMode"

/**
 * @brief Adjusts the white balance controls to match the ambient light temperature
 */
#define IC4_PROPID_BALANCE_WHITE_TEMPERATURE "BalanceWhiteTemperature"

/**
 * @brief Selects a specific white balance preset.
 */
#define IC4_PROPID_BALANCE_WHITE_TEMPERATURE_PRESET "BalanceWhiteTemperaturePreset"

/**
 * @brief Number of horizontal photo-sensitive cells to combine together. This increases the intensity (or signal to noise ratio) of the pixels and reduces the horizontal resolution (width) of the image. A value of 1 indicates that no horizontal binning is performed by the camera.
 */
#define IC4_PROPID_BINNING_HORIZONTAL "BinningHorizontal"

/**
 * @brief Number of vertical photo-sensitive cells to combine together. This increases the intensity (or signal to noise ratio) of the pixels and reduces the vertical resolution (height) of the image. A value of 1 indicates that no vertical binning is performed by the camera.
 */
#define IC4_PROPID_BINNING_VERTICAL "BinningVertical"

/**
 * @brief Controls the analog black level as an absolute physical value. This represents a DC offset applied to the video signal
 */
#define IC4_PROPID_BLACK_LEVEL "BlackLevel"

/**
 * @brief Returns the block ID (frame number) of the image
 */
#define IC4_PROPID_CHUNK_BLOCK_ID "ChunkBlockId"

/**
 * @brief Enables the inclusion of the selected Chunk data in the payload of the image.
 */
#define IC4_PROPID_CHUNK_ENABLE "ChunkEnable"

/**
 * @brief Returns the exposure time used to capture the image.
 */
#define IC4_PROPID_CHUNK_EXPOSURE_TIME "ChunkExposureTime"

/**
 * @brief Returns the gain used to capture the image.
 */
#define IC4_PROPID_CHUNK_GAIN "ChunkGain"

/**
 * @brief Returns the entire image data included in the payload.
 */
#define IC4_PROPID_CHUNK_IMAGE "ChunkImage"

/**
 * @brief IMX174 double exposure frame id.
 */
#define IC4_PROPID_CHUNK_IMX174_FRAME_ID "ChunkIMX174FrameId"

/**
 * @brief IMX174 double exposure frame set identifier.
 */
#define IC4_PROPID_CHUNK_IMX174_FRAME_SET "ChunkIMX174FrameSet"

/**
 * @brief Activates the inclusion of Chunk data in the payload of the image.
 */
#define IC4_PROPID_CHUNK_MODE_ACTIVE "ChunkModeActive"

/**
 * @brief Returns the frame id in the frame set in Multi Frame Set Output Mode.
 */
#define IC4_PROPID_CHUNK_MULTI_FRAME_SET_FRAME_ID "ChunkMultiFrameSetFrameId"

/**
 * @brief Returns the id of the frame set in Multi Frame Set Output Mode.
 */
#define IC4_PROPID_CHUNK_MULTI_FRAME_SET_ID "ChunkMultiFrameSetId"

/**
 * @brief Selects which Chunk to enable or control
 */
#define IC4_PROPID_CHUNK_SELECTOR "ChunkSelector"

/**
 * @brief Activates the selected Color Transformation module.
 */
#define IC4_PROPID_COLOR_TRANSFORMATION_ENABLE "ColorTransformationEnable"

/**
 * @brief Selects which Color Transformation module is controlled by the various Color Transformation features.
 */
#define IC4_PROPID_COLOR_TRANSFORMATION_SELECTOR "ColorTransformationSelector"

/**
 * @brief Represents the value of the selected Gain factor or Offset inside the Transformation matrix.
 */
#define IC4_PROPID_COLOR_TRANSFORMATION_VALUE "ColorTransformationValue"

/**
 * @brief Selects the Gain factor or Offset of the Transformation matrix to access in the selected Color Transformation module.
 */
#define IC4_PROPID_COLOR_TRANSFORMATION_VALUE_SELECTOR "ColorTransformationValueSelector"

/**
 * @brief Controls Contrast.
 */
#define IC4_PROPID_CONTRAST "Contrast"

/**
 * @brief Horizontal sub-sampling of the image. This reduces the horizontal resolution (width) of the image by the specified horizontal decimation factor.
 */
#define IC4_PROPID_DECIMATION_HORIZONTAL "DecimationHorizontal"

/**
 * @brief Vertical sub-sampling of the image. This reduces the vertical resolution (height) of the image by the specified vertical decimation factor.
 */
#define IC4_PROPID_DECIMATION_VERTICAL "DecimationVertical"

/**
 * @brief Controls the strength of the denoise algorithm.
 */
#define IC4_PROPID_DENOISE "Denoise"

/**
 * @brief Indicates the number of event channels supported by the device.
 */
#define IC4_PROPID_DEVICE_EVENT_CHANNEL_COUNT "DeviceEventChannelCount"

/**
 * @brief Resets all user-programmable non-volatile memory to default. After reset, the device must be rediscovered.
 */
#define IC4_PROPID_DEVICE_FACTORY_RESET "DeviceFactoryReset"

/**
 * @brief Version of the device.
 */
#define IC4_PROPID_DEVICE_FIRMWARE_VERSION "DeviceFirmwareVersion"

/**
 * @brief Controls the current heartbeat timeout of the specific Link.
 */
#define IC4_PROPID_DEVICE_LINK_HEARTBEAT_TIMEOUT "DeviceLinkHeartbeatTimeout"

/**
 * @brief Selects which Link of the device to control.
 */
#define IC4_PROPID_DEVICE_LINK_SELECTOR "DeviceLinkSelector"

/**
 * @brief Model of the device.
 */
#define IC4_PROPID_DEVICE_MODEL_NAME "DeviceModelName"

/**
 * @brief Resets the device to its power up state. After reset, the device must be rediscovered.
 */
#define IC4_PROPID_DEVICE_RESET "DeviceReset"

/**
 * @brief Returns the scan type of the sensor of the device.
 */
#define IC4_PROPID_DEVICE_SCAN_TYPE "DeviceScanType"

/**
 * @brief Device's serial number. This string is a unique identifier of the device.
 */
#define IC4_PROPID_DEVICE_SERIAL_NUMBER "DeviceSerialNumber"

/**
 * @brief Major version of the Standard Features Naming Convention that was used to create the device's GenICam XML
 */
#define IC4_PROPID_DEVICE_SFNC_VERSION_MAJOR "DeviceSFNCVersionMajor"

/**
 * @brief Minor version of the Standard Features Naming Convention that was used to create the device's GenICam XML
 */
#define IC4_PROPID_DEVICE_SFNC_VERSION_MINOR "DeviceSFNCVersionMinor"

/**
 * @brief Sub minor version of the Standard Features Naming Convention that was used to create the device's GenICam XML
 */
#define IC4_PROPID_DEVICE_SFNC_VERSION_SUB_MINOR "DeviceSFNCVersionSubMinor"

/**
 * @brief Indicates the number of streaming channels supported by the device.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_COUNT "DeviceStreamChannelCount"

/**
 * @brief Endianess of multi-byte pixel data for this stream.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_ENDIANNESS "DeviceStreamChannelEndianness"

/**
 * @brief Index of device`s Link to use for streaming the specifed stream channel.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_LINK "DeviceStreamChannelLink"

/**
 * @brief Specifies the stream packet size, in bytes, to send on the selected channel.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_PACKET_SIZE "DeviceStreamChannelPacketSize"

/**
 * @brief Selects the stream channel to control.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_SELECTOR "DeviceStreamChannelSelector"

/**
 * @brief Reports the type of the stream channel.
 */
#define IC4_PROPID_DEVICE_STREAM_CHANNEL_TYPE "DeviceStreamChannelType"

/**
 * @brief Device temperature in degrees Celsius (°C). It is measured at the location selected by DeviceTemperatureSelector.
 */
#define IC4_PROPID_DEVICE_TEMPERATURE "DeviceTemperature"

/**
 * @brief Selects the location within the device, where the temperature will be measured.
 */
#define IC4_PROPID_DEVICE_TEMPERATURE_SELECTOR "DeviceTemperatureSelector"

/**
 * @brief Transport Layer type of the device.
 */
#define IC4_PROPID_DEVICE_TL_TYPE "DeviceTLType"

/**
 * @brief Major version of the Transport Layer of the device.
 */
#define IC4_PROPID_DEVICE_TL_VERSION_MAJOR "DeviceTLVersionMajor"

/**
 * @brief Minor version of the Transport Layer of the device.
 */
#define IC4_PROPID_DEVICE_TL_VERSION_MINOR "DeviceTLVersionMinor"

/**
 * @brief Sub minor version of the Transport Layer of the device.
 */
#define IC4_PROPID_DEVICE_TL_VERSION_SUB_MINOR "DeviceTLVersionSubMinor"

/**
 * @brief Returns the device type.
 */
#define IC4_PROPID_DEVICE_TYPE "DeviceType"

/**
 * @brief User-programmable device identifier.
 */
#define IC4_PROPID_DEVICE_USER_ID "DeviceUserID"

/**
 * @brief Name of the manufacturer of the device.
 */
#define IC4_PROPID_DEVICE_VENDOR_NAME "DeviceVendorName"

/**
 * @brief Version of the device.
 */
#define IC4_PROPID_DEVICE_VERSION "DeviceVersion"

/**
 * @brief Disables the HDMI format info overlay.
 */
#define IC4_PROPID_DISABLE_INFO_OVERLAY "DisableInfoOverlay"

/**
 * @brief Unique identifier of the Exposure End event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_EXPOSURE_END "EventExposureEnd"

/**
 * @brief Frame ID of the Exposure End event.
 */
#define IC4_PROPID_EVENT_EXPOSURE_END_FRAME_ID "EventExposureEndFrameID"

/**
 * @brief Timestamp of the Exposure End event.
 */
#define IC4_PROPID_EVENT_EXPOSURE_END_TIMESTAMP "EventExposureEndTimestamp"

/**
 * @brief Unique identifier of the Focus Move Completed event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_FOCUS_MOVE_COMPLETED "EventFocusMoveCompleted"

/**
 * @brief Focus value of the Focus Move Completed event.
 */
#define IC4_PROPID_EVENT_FOCUS_MOVE_COMPLETED_FOCUS "EventFocusMoveCompletedFocus"

/**
 * @brief Timestamp of the Focus Move Completed event.
 */
#define IC4_PROPID_EVENT_FOCUS_MOVE_COMPLETED_TIMESTAMP "EventFocusMoveCompletedTimestamp"

/**
 * @brief Unique identifier of the Frame Trigger Missed event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_FRAME_TRIGGER_MISSED "EventFrameTriggerMissed"

/**
 * @brief Timestamp of the Frame Trigger Missed event.
 */
#define IC4_PROPID_EVENT_FRAME_TRIGGER_MISSED_TIMESTAMP "EventFrameTriggerMissedTimestamp"

/**
 * @brief Unique identifier of the Line1 Falling Edge event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_LINE1_FALLING_EDGE "EventLine1FallingEdge"

/**
 * @brief Timestamp of the Line1 Falling Edge event.
 */
#define IC4_PROPID_EVENT_LINE1_FALLING_EDGE_TIMESTAMP "EventLine1FallingEdgeTimestamp"

/**
 * @brief Unique identifier of the Line1 Rising Edge event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_LINE1_RISING_EDGE "EventLine1RisingEdge"

/**
 * @brief Timestamp of the Line1 Rising Edge event.
 */
#define IC4_PROPID_EVENT_LINE1_RISING_EDGE_TIMESTAMP "EventLine1RisingEdgeTimestamp"

/**
 * @brief Enables event notifications for the currently selected event. The event can selected using the Event Selector parameter.
 */
#define IC4_PROPID_EVENT_NOTIFICATION "EventNotification"

/**
 * @brief Sets the event notification to be enabled. The notification can be enabled using the Event Notification parameter.
 */
#define IC4_PROPID_EVENT_SELECTOR "EventSelector"

/**
 * @brief Unique identifier of the Test event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_TEST "EventTest"

/**
 * @brief Timestamp of the test event.
 */
#define IC4_PROPID_EVENT_TEST_TIMESTAMP "EventTestTimestamp"

/**
 * @brief Unique identifier of the Zoom Move Completed event. Use this parameter to get notified when the event occurs.
 */
#define IC4_PROPID_EVENT_ZOOM_MOVE_COMPLETED "EventZoomMoveCompleted"

/**
 * @brief Timestamp of the Zoom Move Completed event.
 */
#define IC4_PROPID_EVENT_ZOOM_MOVE_COMPLETED_TIMESTAMP "EventZoomMoveCompletedTimestamp"

/**
 * @brief Zoom value of the Zoom Move Completed event.
 */
#define IC4_PROPID_EVENT_ZOOM_MOVE_COMPLETED_ZOOM "EventZoomMoveCompletedZoom"

/**
 * @brief Use the full brightness range when receiving YUV data via HDMI.
 */
#define IC4_PROPID_EXPAND_OUTPUT_RANGE "ExpandOutputRange"

/**
 * @brief Sets the automatic exposure mode when ExposureMode is Timed.
 */
#define IC4_PROPID_EXPOSURE_AUTO "ExposureAuto"

/**
 * @brief Lets the ExposureAuto/GainAuto algorithm try to avoid over-exposures.
 */
#define IC4_PROPID_EXPOSURE_AUTO_HIGHLIGH_REDUCTION "ExposureAutoHighlighReduction"

/**
 * @brief Lets the ExposureAuto/GainAuto algorithm try to avoid over-exposures.
 */
#define IC4_PROPID_EXPOSURE_AUTO_HIGHLIGHT_REDUCTION "ExposureAutoHighlightReduction"

/**
 * @brief Lower limit of the ExposureAuto function.
 */
#define IC4_PROPID_EXPOSURE_AUTO_LOWER_LIMIT "ExposureAutoLowerLimit"

/**
 * @brief Configures the target image brightness of the ExposureAuto/GainAuto algorithm.
 */
#define IC4_PROPID_EXPOSURE_AUTO_REFERENCE "ExposureAutoReference"

/**
 * @brief Upper limit of the ExposureAuto function.
 */
#define IC4_PROPID_EXPOSURE_AUTO_UPPER_LIMIT "ExposureAutoUpperLimit"

/**
 * @brief Automatically sets the upper limit to match the Acquisition Frame Rate.
 */
#define IC4_PROPID_EXPOSURE_AUTO_UPPER_LIMIT_AUTO "ExposureAutoUpperLimitAuto"

/**
 * @brief Sets the Exposure time when ExposureMode is Timed and ExposureAuto is Off. This controls the duration where the photosensitive cells are exposed to light.
 */
#define IC4_PROPID_EXPOSURE_TIME "ExposureTime"

/**
 * @brief Defines the intermediate access buffer that allows the exchange of data between the device file storage and the application.
 */
#define IC4_PROPID_FILE_ACCESS_BUFFER "FileAccessBuffer"

/**
 * @brief Controls the Length of the mapping between the device file storage and the FileAccessBuffer.
 */
#define IC4_PROPID_FILE_ACCESS_LENGTH "FileAccessLength"

/**
 * @brief Controls the Offset of the mapping between the device file storage and the FileAccessBuffer.
 */
#define IC4_PROPID_FILE_ACCESS_OFFSET "FileAccessOffset"

/**
 * @brief Selects the access mode in which a file is opened in the device.
 */
#define IC4_PROPID_FILE_OPEN_MODE "FileOpenMode"

/**
 * @brief Executes the operation selected by FileOperationSelector on the selected file.
 */
#define IC4_PROPID_FILE_OPERATION_EXECUTE "FileOperationExecute"

/**
 * @brief Represents the file operation result. For Read or Write operations, the number of successfully read/written bytes is returned.
 */
#define IC4_PROPID_FILE_OPERATION_RESULT "FileOperationResult"

/**
 * @brief Selects the target operation for the selected file in the device. This Operation is executed when the FileOperationExecute feature is called.
 */
#define IC4_PROPID_FILE_OPERATION_SELECTOR "FileOperationSelector"

/**
 * @brief Represents the file operation execution status.
 */
#define IC4_PROPID_FILE_OPERATION_STATUS "FileOperationStatus"

/**
 * @brief Selects the target file in the device.
 */
#define IC4_PROPID_FILE_SELECTOR "FileSelector"

/**
 * @brief Represents the size of the selected file in bytes.
 */
#define IC4_PROPID_FILE_SIZE "FileSize"

/**
 * @brief Changes the Focus setting of the lens.
 */
#define IC4_PROPID_FOCUS "Focus"

/**
 * @brief Enables automatic Focus control.
 */
#define IC4_PROPID_FOCUS_AUTO "FocusAuto"

/**
 * @brief Controls the gain as an absolute physical value. This is an amplification factor applied to he video signal.
 */
#define IC4_PROPID_GAIN "Gain"

/**
 * @brief Sets the automatic gain control mode.
 */
#define IC4_PROPID_GAIN_AUTO "GainAuto"

/**
 * @brief Lower limit of the GainAuto function.
 */
#define IC4_PROPID_GAIN_AUTO_LOWER_LIMIT "GainAutoLowerLimit"

/**
 * @brief Upper limit of the GainAuto function.
 */
#define IC4_PROPID_GAIN_AUTO_UPPER_LIMIT "GainAutoUpperLimit"

/**
 * @brief Configures the sensor-specific Gain (LCG/HCG) setting
 */
#define IC4_PROPID_GAIN_MODE "GainMode"

/**
 * @brief Controls the gamma correction of pixel intensity. This is typically used to compensate for nonlinearity of the display system (such as CRT).
 */
#define IC4_PROPID_GAMMA "Gamma"

/**
 * @brief Enables the extended IDs mode
 */
#define IC4_PROPID_GEV_GVSP_EXTENDED_ID_MODE "GevGVSPExtendedIDMode"

/**
 * @brief The state of this feature is copied into the "do not fragment" bit of IP header of each stream packet. It can be used by the application to prevent IP fragmentation of packets on the stream channel.
 */
#define IC4_PROPID_GEV_SCPS_DO_NOT_FRAGMENT "GevSCPSDoNotFragment"

/**
 * @brief This GigE Vision specific feature corresponds to DeviceStreamChannelPacketSize and should be kept in sync with it. It specifies the stream packet size, in bytes, to send on the selected channel for a GVSP transmitter or specifies the maximum packet size supported by a GVSP receiver.
 */
#define IC4_PROPID_GEV_SCPS_PACKET_SIZE "GevSCPSPacketSize"

/**
 * @brief Status of the digital input pin.
 */
#define IC4_PROPID_GP_IN "GPIn"

/**
 * @brief Status of the digital output pin.
 */
#define IC4_PROPID_GP_OUT "GPOut"

/**
 * @brief Height of the image provided by the device (in pixels).
 */
#define IC4_PROPID_HEIGHT "Height"

/**
 * @brief Maximum height of the image (in pixels). This dimension is calculated after vertical binning, decimation or any other function changing the vertical dimension of the image.
 */
#define IC4_PROPID_HEIGHT_MAX "HeightMax"

/**
 * @brief Color Hue
 */
#define IC4_PROPID_HUE "Hue"

/**
 * @brief Enable IMX174 WDR mode. This mode enables setting a second, longer exposure time (IMX174 WDR Shutter 2), which is used for every other frame.
 */
#define IC4_PROPID_IMX174_HARDWARE_WDR_ENABLE "IMX174HardwareWDREnable"

/**
 * @brief Select the IMX174 WDR shutter mode. This mode decides whether the second shutter time can be choosen freely, or is fixed at a multiple of the configured ExposureTime.
 */
#define IC4_PROPID_IMX174_HARDWARE_WDR_SHUTTER_MODE "IMX174HardwareWDRShutterMode"

/**
 * @brief Exposure time that is used for every other frame, if IMX174 WDR mode is enabled.
 */
#define IC4_PROPID_IMX174_WDR_SHUTTER2 "IMX174WDRShutter2"

/**
 * @brief Enables the low-latency mode for IMX global shutter sensors. This mode reduces the jitter between trigger input and exposure start, but disallows trigger overlap.
 */
#define IC4_PROPID_IMX_LOW_LATENCY_TRIGGER_MODE "IMXLowLatencyTriggerMode"

/**
 * @brief The detected bits per pixel of the HDMI source.
 */
#define IC4_PROPID_INPUT_BITS "InputBits"

/**
 * @brief The detected frame rate of the HDMI source.
 */
#define IC4_PROPID_INPUT_FP1KS "InputFp1ks"

/**
 * @brief The detected width of the HDMI input image.
 */
#define IC4_PROPID_INPUT_HEIGHT "InputHeight"

/**
 * @brief The detected width of the HDMI input image.
 */
#define IC4_PROPID_INPUT_WIDTH "InputWidth"

/**
 * @brief Enables the IRCutFilter in from of the sensor.
 */
#define IC4_PROPID_IR_CUT_FILTER_ENABLE "IRCutFilterEnable"

/**
 * @brief Changes the Iris setting of the lens.
 */
#define IC4_PROPID_IRIS "Iris"

/**
 * @brief Enables automatic Iris control.
 */
#define IC4_PROPID_IRIS_AUTO "IrisAuto"

/**
 * @brief Activates the LUT.
 */
#define IC4_PROPID_LUT_ENABLE "LUTEnable"

/**
 * @brief Control the index (offset) of the coefficient to access in the LUT.
 */
#define IC4_PROPID_LUT_INDEX "LUTIndex"

/**
 * @brief Selects which LUT to control.
 */
#define IC4_PROPID_LUT_SELECTOR "LUTSelector"

/**
 * @brief Returns or sets the Value at entry LUTIndex of the LUT.
 */
#define IC4_PROPID_LUT_VALUE "LUTValue"

/**
 * @brief Accesses all the LUT coefficients in a single access without using individual LUTIndex.
 */
#define IC4_PROPID_LUT_VALUE_ALL "LUTValueAll"

/**
 * @brief Enables the sensor to use customizable gain values for the different frames of a frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_CUSTOM_GAIN "MultiFrameSetOutputModeCustomGain"

/**
 * @brief Enables the sensor to output a sequence of 2 or 4 frames with configurable exposure times and gain values.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_ENABLE "MultiFrameSetOutputModeEnable"

/**
 * @brief Configures the exposure time for frame 0 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_EXPOSURE_TIME0 "MultiFrameSetOutputModeExposureTime0"

/**
 * @brief Configures the exposure time for frame 1 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_EXPOSURE_TIME1 "MultiFrameSetOutputModeExposureTime1"

/**
 * @brief Configures the exposure time for frame 2 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_EXPOSURE_TIME2 "MultiFrameSetOutputModeExposureTime2"

/**
 * @brief Configures the exposure time for frame 3 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_EXPOSURE_TIME3 "MultiFrameSetOutputModeExposureTime3"

/**
 * @brief Selects the number of frames in multi frame set output mode.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_FRAME_COUNT "MultiFrameSetOutputModeFrameCount"

/**
 * @brief Configures the gain for frame 0 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_GAIN0 "MultiFrameSetOutputModeGain0"

/**
 * @brief Configures the gain for frame 1 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_GAIN1 "MultiFrameSetOutputModeGain1"

/**
 * @brief Configures the gain for frame 2 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_GAIN2 "MultiFrameSetOutputModeGain2"

/**
 * @brief Configures the gain for frame 3 of the frame set.
 */
#define IC4_PROPID_MULTI_FRAME_SET_OUTPUT_MODE_GAIN3 "MultiFrameSetOutputModeGain3"

/**
 * @brief Automatically adjust the values of OffsetX and OffsetY to select the center region of the sensor.
 */
#define IC4_PROPID_OFFSET_AUTO_CENTER "OffsetAutoCenter"

/**
 * @brief Horizontal offset from the origin to the region of interest (in pixels).
 */
#define IC4_PROPID_OFFSET_X "OffsetX"

/**
 * @brief Vertical offset from the origin to the region of interest (in pixels).
 */
#define IC4_PROPID_OFFSET_Y "OffsetY"

/**
 * @brief Provides the number of bytes transferred for each image or chunk on the stream channel. This includes any end-of-line, end-of-frame statistics or other stamp data. This is the total size of data payload for a data block.
 */
#define IC4_PROPID_PAYLOAD_SIZE "PayloadSize"

/**
 * @brief Format of the pixels provided by the device
 */
#define IC4_PROPID_PIXEL_FORMAT "PixelFormat"

/**
 * @brief Indicates the expected accuracy of the device PTP clock when it is the grandmaster, or in the event it becomes the grandmaster.
 */
#define IC4_PROPID_PTP_CLOCK_ACCURACY "PtpClockAccuracy"

/**
 * @brief Enables the Precision Time Protocol (PTP).
 */
#define IC4_PROPID_PTP_ENABLE "PtpEnable"

/**
 * @brief Returns the current state of the PTP clock.
 */
#define IC4_PROPID_PTP_STATUS "PtpStatus"

/**
 * @brief Flip the image horizontally.
 */
#define IC4_PROPID_REVERSE_X "ReverseX"

/**
 * @brief Flip the image vertically.
 */
#define IC4_PROPID_REVERSE_Y "ReverseY"

/**
 * @brief Color Saturation
 */
#define IC4_PROPID_SATURATION "Saturation"

/**
 * @brief Effective height of the sensor in pixels.
 */
#define IC4_PROPID_SENSOR_HEIGHT "SensorHeight"

/**
 * @brief Physical size (pitch) in the y direction of a photo sensitive pixel unit.
 */
#define IC4_PROPID_SENSOR_PIXEL_HEIGHT "SensorPixelHeight"

/**
 * @brief Physical size (pitch) in the x direction of a photo sensitive pixel unit.
 */
#define IC4_PROPID_SENSOR_PIXEL_WIDTH "SensorPixelWidth"

/**
 * @brief Effective width of the sensor in pixels.
 */
#define IC4_PROPID_SENSOR_WIDTH "SensorWidth"

/**
 * @brief Controls the strength of the sharpness algorithm.
 */
#define IC4_PROPID_SHARPNESS "Sharpness"

/**
 * @brief Enables Trigger/Strobe over USB-C Sideband Use Pins
 */
#define IC4_PROPID_SIDEBAND_USE "SidebandUse"

/**
 * @brief Indicates whether a signal is detected form the HDMI source.
 */
#define IC4_PROPID_SIGNAL_DETECTED "SignalDetected"

/**
 * @brief Enables Software Transform properties. If enabled, properties like Sharpness, Tone Mapping etc. can be enabled and lead to a software transformation in the driver.
 */
#define IC4_PROPID_SOFTWARE_TRANSFORM_ENABLE "SoftwareTransformEnable"

/**
 * @brief Indicates whether a HDMI source is connected.
 */
#define IC4_PROPID_SOURCE_CONNECTED "SourceConnected"

/**
 * @brief Configures the delay from start of exposure to the generated strobe pulse.
 */
#define IC4_PROPID_STROBE_DELAY "StrobeDelay"

/**
 * @brief If StrobeOperation is FixedDuration, specifies the pulse length.
 */
#define IC4_PROPID_STROBE_DURATION "StrobeDuration"

/**
 * @brief Enable generation of exposure-synchronized output pulses
 */
#define IC4_PROPID_STROBE_ENABLE "StrobeEnable"

/**
 * @brief Configures the mode of specifying the length of exposure-synchronized output pulses
 */
#define IC4_PROPID_STROBE_OPERATION "StrobeOperation"

/**
 * @brief Select level of exposure-synchronized output pulses
 */
#define IC4_PROPID_STROBE_POLARITY "StrobePolarity"

/**
 * @brief Generates a Test event that can be used for testing event notification.
 */
#define IC4_PROPID_TEST_EVENT_GENERATE "TestEventGenerate"

/**
 * @brief Tests the device's pending acknowledge feature.
 */
#define IC4_PROPID_TEST_PENDING_ACK "TestPendingAck"

/**
 * @brief Latches the current timestamp counter into TimestampLatchValue.
 */
#define IC4_PROPID_TIMESTAMP_LATCH "TimestampLatch"

/**
 * @brief Human-readable interpretation of the latched timestamp.
 */
#define IC4_PROPID_TIMESTAMP_LATCH_STRING "TimestampLatchString"

/**
 * @brief Returns the latched value of the timestamp counter. The timestamp must first be latched by using the Timestamp Control Latch command.
 */
#define IC4_PROPID_TIMESTAMP_LATCH_VALUE "TimestampLatchValue"

/**
 * @brief Resets the current value of the device timestamp counter. After executing this command, the timestamp counter restarts automatically.
 */
#define IC4_PROPID_TIMESTAMP_RESET "TimestampReset"

/**
 * @brief Used by the Transport Layer to prevent critical features from changing during acquisition.
 */
#define IC4_PROPID_TL_PARAMS_LOCKED "TLParamsLocked"

/**
 * @brief Enables tone mapping.
 */
#define IC4_PROPID_TONE_MAPPING_ENABLE "ToneMappingEnable"

/**
 * @brief Changes the brightness reference used for a individual pixel, which is interpolated between local and global.
 */
#define IC4_PROPID_TONE_MAPPING_GLOBAL_BRIGHTNESS "ToneMappingGlobalBrightness"

/**
 * @brief Adjusts the intensity of the tonemapping algorithm.
 */
#define IC4_PROPID_TONE_MAPPING_INTENSITY "ToneMappingIntensity"

/**
 * @brief Specifies the activation mode of the trigger
 */
#define IC4_PROPID_TRIGGER_ACTIVATION "TriggerActivation"

/**
 * @brief Specifies the time for which trigger input has to be low in order accept the next trigger signal.
 */
#define IC4_PROPID_TRIGGER_DEBOUNCER "TriggerDebouncer"

/**
 * @brief Specifies the delay to apply after the trigger reception before activating it.
 */
#define IC4_PROPID_TRIGGER_DELAY "TriggerDelay"

/**
 * @brief Specifies the time for which trigger input has to be high in order to be accepted as a trigger signal.
 */
#define IC4_PROPID_TRIGGER_DENOISE "TriggerDenoise"

/**
 * @brief Specifies the time for which trigger pulses are ignored after accepting a trigger signal.
 */
#define IC4_PROPID_TRIGGER_MASK "TriggerMask"

/**
 * @brief Controls if the selected trigger is active.
 */
#define IC4_PROPID_TRIGGER_MODE "TriggerMode"

/**
 * @brief Controls the operation mode of the sensor in trigger mode.
 */
#define IC4_PROPID_TRIGGER_OPERATION "TriggerOperation"

/**
 * @brief Specifies the type trigger overlap permitted with the previous frame. This defines when a valid trigger will be accepted for a new frame.
 */
#define IC4_PROPID_TRIGGER_OVERLAP "TriggerOverlap"

/**
 * @brief Selects the type of trigger to configure.
 */
#define IC4_PROPID_TRIGGER_SELECTOR "TriggerSelector"

/**
 * @brief Generates an internal trigger. TriggerSource must be set to Software or Any.
 */
#define IC4_PROPID_TRIGGER_SOFTWARE "TriggerSoftware"

/**
 * @brief Specifies the internal signal or physical input Line to use as the trigger source.
 */
#define IC4_PROPID_TRIGGER_SOURCE "TriggerSource"

/**
 * @brief Selects the feature User Set to load and make active by default when the device is reset.
 */
#define IC4_PROPID_USER_SET_DEFAULT "UserSetDefault"

/**
 * @brief Loads the User Set specified by UserSetSelector to the device and makes it active.
 */
#define IC4_PROPID_USER_SET_LOAD "UserSetLoad"

/**
 * @brief Save the User Set specified by UserSetSelector to the non-volatile memory of the device.
 */
#define IC4_PROPID_USER_SET_SAVE "UserSetSave"

/**
 * @brief Selects the feature User Set to load, save or configure.
 */
#define IC4_PROPID_USER_SET_SELECTOR "UserSetSelector"

/**
 * @brief Width of the image provided by the device (in pixels).
 */
#define IC4_PROPID_WIDTH "Width"

/**
 * @brief Maximum width of the image (in pixels). The dimension is calculated after horizontal binning, decimation or any other function changing the horizontal dimension of the image.
 */
#define IC4_PROPID_WIDTH_MAX "WidthMax"

/**
 * @brief Changes the zoom setting of the lens.
 */
#define IC4_PROPID_ZOOM "Zoom"
/**
 * @}
 * @}
 */

#endif // IC4_C_PROPERTY_CONSTANTS_H_INC_
