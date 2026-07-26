
#ifndef IC4_PROPERTY_CONSTANTS_H_INC_
#define IC4_PROPERTY_CONSTANTS_H_INC_

#pragma once

#include "PropId.h"

namespace ic4
{
    /**
     * @brief Contains definitions for common device property identifiers.
     */
    namespace PropId
    {

        /**
         * @brief Number of frames to acquire for each trigger.
         */
        constexpr PropIdInteger AcquisitionBurstFrameCount = { "AcquisitionBurstFrameCount" };

        /**
         * @brief Minimum interval between frames in an acquisition burst.
         */
        constexpr PropIdInteger AcquisitionBurstInterval = { "AcquisitionBurstInterval" };

        /**
         * @brief Controls the acquisition rate at which the frames are captured.
         */
        constexpr PropIdFloat AcquisitionFrameRate = { "AcquisitionFrameRate" };

        /**
         * @brief Sets the acquisition mode of the device. It defines mainly the number of frames to capture during an acquisition and the way the acquisition stops.
         */
        constexpr PropIdEnumeration AcquisitionMode = { "AcquisitionMode" };

        /**
         * @brief Starts the Acquisition of the device. The number of frames captured is specified by AcquisitionMode.
         */
        constexpr PropIdCommand AcquisitionStart = { "AcquisitionStart" };

        /**
         * @brief Stops the Acquisition of the device at the end of the current Frame. It is mainly used when AcquisitionMode is Continuous but can be used in any acquisition mode.
         */
        constexpr PropIdCommand AcquisitionStop = { "AcquisitionStop" };

        /**
         * @brief Provides the device key that allows the device to check the validity of action commands. The device internal assertion of an action signal is only authorized if the ActionDeviceKey and the action device key value in the protocol message are equal.
         */
        constexpr PropIdInteger ActionDeviceKey = { "ActionDeviceKey" };

        /**
         * @brief Provides the key that the device will use to validate the action on reception of the action protocol message.
         */
        constexpr PropIdInteger ActionGroupKey = { "ActionGroupKey" };

        /**
         * @brief Provides the mask that the device will use to validate the action on reception of the action protocol message.
         */
        constexpr PropIdInteger ActionGroupMask = { "ActionGroupMask" };

        /**
         * @brief Indicates the size of the scheduled action commands queue. This number represents the maximum number of scheduled action commands that can be pending at a given point in time.
         */
        constexpr PropIdInteger ActionQueueSize = { "ActionQueueSize" };

        /**
         * @brief Cancels all scheduled actions.
         */
        constexpr PropIdCommand ActionSchedulerCancel = { "ActionSchedulerCancel" };

        /**
         * @brief Schedules the action to be executed time specified by ActionSchedulerTime, with optional repetition in intervals of ActionSchedulerInterval.
         */
        constexpr PropIdCommand ActionSchedulerCommit = { "ActionSchedulerCommit" };

        /**
         * @brief Action repetition interval. A value of 0 schedules the action to be executed only once.
         */
        constexpr PropIdInteger ActionSchedulerInterval = { "ActionSchedulerInterval" };

        /**
         * @brief Indicates whether there are actions scheduled.
         */
        constexpr PropIdEnumeration ActionSchedulerStatus = { "ActionSchedulerStatus" };

        /**
         * @brief Camera time for when the action is to be scheduled.
         */
        constexpr PropIdInteger ActionSchedulerTime = { "ActionSchedulerTime" };

        /**
         * @brief Selects to which Action Signal further Action settings apply.
         */
        constexpr PropIdInteger ActionSelector = { "ActionSelector" };

        /**
         * @brief Enable the region of interest for auto focus.
         */
        constexpr PropIdBoolean AutoFocusROIEnable = { "AutoFocusROIEnable" };

        /**
         * @brief Vertical size of the auto focus region of interest.
         */
        constexpr PropIdInteger AutoFocusROIHeight = { "AutoFocusROIHeight" };

        /**
         * @brief Horizontal offset of the auto focus region of interest.
         */
        constexpr PropIdInteger AutoFocusROILeft = { "AutoFocusROILeft" };

        /**
         * @brief Vertical offset of the auto focus region of interest.
         */
        constexpr PropIdInteger AutoFocusROITop = { "AutoFocusROITop" };

        /**
         * @brief Horizontal size of the auto focus region of interest.
         */
        constexpr PropIdInteger AutoFocusROIWidth = { "AutoFocusROIWidth" };

        /**
         * @brief Enable the region of interest for auto functions.
         */
        constexpr PropIdBoolean AutoFunctionsROIEnable = { "AutoFunctionsROIEnable" };

        /**
         * @brief Vertical size of the auto functions region of interest.
         */
        constexpr PropIdInteger AutoFunctionsROIHeight = { "AutoFunctionsROIHeight" };

        /**
         * @brief Horizontal offset of the auto functions region of interest.
         */
        constexpr PropIdInteger AutoFunctionsROILeft = { "AutoFunctionsROILeft" };

        /**
         * @brief Select a predefined region of interest for auto functions.
         */
        constexpr PropIdEnumeration AutoFunctionsROIPreset = { "AutoFunctionsROIPreset" };

        /**
         * @brief Vertical offset of the auto functions region of interest.
         */
        constexpr PropIdInteger AutoFunctionsROITop = { "AutoFunctionsROITop" };

        /**
         * @brief Horizontal size of the auto functions region of interest.
         */
        constexpr PropIdInteger AutoFunctionsROIWidth = { "AutoFunctionsROIWidth" };

        /**
         * @brief Controls ratio of the selected color component to a reference color component. It is used for white balancing.
         */
        constexpr PropIdFloat BalanceRatio = { "BalanceRatio" };

        /**
         * @brief This enumeration selects a balance ratio control to configuration. Once a balance ratio control has been selected, all changes to the balance ratio settings will be applied to the selected control.
         */
        constexpr PropIdEnumeration BalanceRatioSelector = { "BalanceRatioSelector" };

        /**
         * @brief Controls the mode for automatic white balancing between the color channels. The white balancing ratios are automatically adjusted.
         */
        constexpr PropIdEnumeration BalanceWhiteAuto = { "BalanceWhiteAuto" };

        /**
         * @brief Selects a specific preset for automatic white balance.
         */
        constexpr PropIdEnumeration BalanceWhiteAutoPreset = { "BalanceWhiteAutoPreset" };

        /**
         * @brief Configures the way auto white balance works.
         */
        constexpr PropIdEnumeration BalanceWhiteMode = { "BalanceWhiteMode" };

        /**
         * @brief Adjusts the white balance controls to match the ambient light temperature
         */
        constexpr PropIdInteger BalanceWhiteTemperature = { "BalanceWhiteTemperature" };

        /**
         * @brief Selects a specific white balance preset.
         */
        constexpr PropIdEnumeration BalanceWhiteTemperaturePreset = { "BalanceWhiteTemperaturePreset" };

        /**
         * @brief Number of horizontal photo-sensitive cells to combine together. This increases the intensity (or signal to noise ratio) of the pixels and reduces the horizontal resolution (width) of the image. A value of 1 indicates that no horizontal binning is performed by the camera.
         */
        constexpr PropIdInteger BinningHorizontal = { "BinningHorizontal" };

        /**
         * @brief Number of vertical photo-sensitive cells to combine together. This increases the intensity (or signal to noise ratio) of the pixels and reduces the vertical resolution (height) of the image. A value of 1 indicates that no vertical binning is performed by the camera.
         */
        constexpr PropIdInteger BinningVertical = { "BinningVertical" };

        /**
         * @brief Controls the analog black level as an absolute physical value. This represents a DC offset applied to the video signal
         */
        constexpr PropIdFloat BlackLevel = { "BlackLevel" };

        /**
         * @brief Returns the block ID (frame number) of the image
         */
        constexpr PropIdRegister ChunkBlockId = { "ChunkBlockId" };

        /**
         * @brief Enables the inclusion of the selected Chunk data in the payload of the image.
         */
        constexpr PropIdBoolean ChunkEnable = { "ChunkEnable" };

        /**
         * @brief Returns the exposure time used to capture the image.
         */
        constexpr PropIdFloat ChunkExposureTime = { "ChunkExposureTime" };

        /**
         * @brief Returns the gain used to capture the image.
         */
        constexpr PropIdFloat ChunkGain = { "ChunkGain" };

        /**
         * @brief Returns the entire image data included in the payload.
         */
        constexpr PropIdRegister ChunkImage = { "ChunkImage" };

        /**
         * @brief IMX174 double exposure frame id.
         */
        constexpr PropIdInteger ChunkIMX174FrameId = { "ChunkIMX174FrameId" };

        /**
         * @brief IMX174 double exposure frame set identifier.
         */
        constexpr PropIdInteger ChunkIMX174FrameSet = { "ChunkIMX174FrameSet" };

        /**
         * @brief Activates the inclusion of Chunk data in the payload of the image.
         */
        constexpr PropIdBoolean ChunkModeActive = { "ChunkModeActive" };

        /**
         * @brief Returns the frame id in the frame set in Multi Frame Set Output Mode.
         */
        constexpr PropIdInteger ChunkMultiFrameSetFrameId = { "ChunkMultiFrameSetFrameId" };

        /**
         * @brief Returns the id of the frame set in Multi Frame Set Output Mode.
         */
        constexpr PropIdInteger ChunkMultiFrameSetId = { "ChunkMultiFrameSetId" };

        /**
         * @brief Selects which Chunk to enable or control
         */
        constexpr PropIdEnumeration ChunkSelector = { "ChunkSelector" };

        /**
         * @brief Activates the selected Color Transformation module.
         */
        constexpr PropIdBoolean ColorTransformationEnable = { "ColorTransformationEnable" };

        /**
         * @brief Selects which Color Transformation module is controlled by the various Color Transformation features.
         */
        constexpr PropIdEnumeration ColorTransformationSelector = { "ColorTransformationSelector" };

        /**
         * @brief Represents the value of the selected Gain factor or Offset inside the Transformation matrix.
         */
        constexpr PropIdFloat ColorTransformationValue = { "ColorTransformationValue" };

        /**
         * @brief Selects the Gain factor or Offset of the Transformation matrix to access in the selected Color Transformation module.
         */
        constexpr PropIdEnumeration ColorTransformationValueSelector = { "ColorTransformationValueSelector" };

        /**
         * @brief Controls Contrast.
         */
        constexpr PropIdInteger Contrast = { "Contrast" };

        /**
         * @brief Horizontal sub-sampling of the image. This reduces the horizontal resolution (width) of the image by the specified horizontal decimation factor.
         */
        constexpr PropIdInteger DecimationHorizontal = { "DecimationHorizontal" };

        /**
         * @brief Vertical sub-sampling of the image. This reduces the vertical resolution (height) of the image by the specified vertical decimation factor.
         */
        constexpr PropIdInteger DecimationVertical = { "DecimationVertical" };

        /**
         * @brief Controls the strength of the denoise algorithm.
         */
        constexpr PropIdInteger Denoise = { "Denoise" };

        /**
         * @brief Indicates the number of event channels supported by the device.
         */
        constexpr PropIdInteger DeviceEventChannelCount = { "DeviceEventChannelCount" };

        /**
         * @brief Resets all user-programmable non-volatile memory to default. After reset, the device must be rediscovered.
         */
        constexpr PropIdCommand DeviceFactoryReset = { "DeviceFactoryReset" };

        /**
         * @brief Version of the device.
         */
        constexpr PropIdString DeviceFirmwareVersion = { "DeviceFirmwareVersion" };

        /**
         * @brief Controls the current heartbeat timeout of the specific Link.
         */
        constexpr PropIdFloat DeviceLinkHeartbeatTimeout = { "DeviceLinkHeartbeatTimeout" };

        /**
         * @brief Selects which Link of the device to control.
         */
        constexpr PropIdInteger DeviceLinkSelector = { "DeviceLinkSelector" };

        /**
         * @brief Model of the device.
         */
        constexpr PropIdString DeviceModelName = { "DeviceModelName" };

        /**
         * @brief Resets the device to its power up state. After reset, the device must be rediscovered.
         */
        constexpr PropIdCommand DeviceReset = { "DeviceReset" };

        /**
         * @brief Returns the scan type of the sensor of the device.
         */
        constexpr PropIdEnumeration DeviceScanType = { "DeviceScanType" };

        /**
         * @brief Device's serial number. This string is a unique identifier of the device.
         */
        constexpr PropIdString DeviceSerialNumber = { "DeviceSerialNumber" };

        /**
         * @brief Major version of the Standard Features Naming Convention that was used to create the device's GenICam XML
         */
        constexpr PropIdInteger DeviceSFNCVersionMajor = { "DeviceSFNCVersionMajor" };

        /**
         * @brief Minor version of the Standard Features Naming Convention that was used to create the device's GenICam XML
         */
        constexpr PropIdInteger DeviceSFNCVersionMinor = { "DeviceSFNCVersionMinor" };

        /**
         * @brief Sub minor version of the Standard Features Naming Convention that was used to create the device's GenICam XML
         */
        constexpr PropIdInteger DeviceSFNCVersionSubMinor = { "DeviceSFNCVersionSubMinor" };

        /**
         * @brief Indicates the number of streaming channels supported by the device.
         */
        constexpr PropIdInteger DeviceStreamChannelCount = { "DeviceStreamChannelCount" };

        /**
         * @brief Endianess of multi-byte pixel data for this stream.
         */
        constexpr PropIdEnumeration DeviceStreamChannelEndianness = { "DeviceStreamChannelEndianness" };

        /**
         * @brief Index of device`s Link to use for streaming the specifed stream channel.
         */
        constexpr PropIdInteger DeviceStreamChannelLink = { "DeviceStreamChannelLink" };

        /**
         * @brief Specifies the stream packet size, in bytes, to send on the selected channel.
         */
        constexpr PropIdInteger DeviceStreamChannelPacketSize = { "DeviceStreamChannelPacketSize" };

        /**
         * @brief Selects the stream channel to control.
         */
        constexpr PropIdInteger DeviceStreamChannelSelector = { "DeviceStreamChannelSelector" };

        /**
         * @brief Reports the type of the stream channel.
         */
        constexpr PropIdEnumeration DeviceStreamChannelType = { "DeviceStreamChannelType" };

        /**
         * @brief Device temperature in degrees Celsius (°C). It is measured at the location selected by DeviceTemperatureSelector.
         */
        constexpr PropIdFloat DeviceTemperature = { "DeviceTemperature" };

        /**
         * @brief Selects the location within the device, where the temperature will be measured.
         */
        constexpr PropIdEnumeration DeviceTemperatureSelector = { "DeviceTemperatureSelector" };

        /**
         * @brief Transport Layer type of the device.
         */
        constexpr PropIdEnumeration DeviceTLType = { "DeviceTLType" };

        /**
         * @brief Major version of the Transport Layer of the device.
         */
        constexpr PropIdInteger DeviceTLVersionMajor = { "DeviceTLVersionMajor" };

        /**
         * @brief Minor version of the Transport Layer of the device.
         */
        constexpr PropIdInteger DeviceTLVersionMinor = { "DeviceTLVersionMinor" };

        /**
         * @brief Sub minor version of the Transport Layer of the device.
         */
        constexpr PropIdInteger DeviceTLVersionSubMinor = { "DeviceTLVersionSubMinor" };

        /**
         * @brief Returns the device type.
         */
        constexpr PropIdEnumeration DeviceType = { "DeviceType" };

        /**
         * @brief User-programmable device identifier.
         */
        constexpr PropIdString DeviceUserID = { "DeviceUserID" };

        /**
         * @brief Name of the manufacturer of the device.
         */
        constexpr PropIdString DeviceVendorName = { "DeviceVendorName" };

        /**
         * @brief Version of the device.
         */
        constexpr PropIdString DeviceVersion = { "DeviceVersion" };

        /**
         * @brief Disables the HDMI format info overlay.
         */
        constexpr PropIdBoolean DisableInfoOverlay = { "DisableInfoOverlay" };

        /**
         * @brief Unique identifier of the Exposure End event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventExposureEnd = { "EventExposureEnd" };

        /**
         * @brief Frame ID of the Exposure End event.
         */
        constexpr PropIdInteger EventExposureEndFrameID = { "EventExposureEndFrameID" };

        /**
         * @brief Timestamp of the Exposure End event.
         */
        constexpr PropIdInteger EventExposureEndTimestamp = { "EventExposureEndTimestamp" };

        /**
         * @brief Unique identifier of the Focus Move Completed event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventFocusMoveCompleted = { "EventFocusMoveCompleted" };

        /**
         * @brief Focus value of the Focus Move Completed event.
         */
        constexpr PropIdInteger EventFocusMoveCompletedFocus = { "EventFocusMoveCompletedFocus" };

        /**
         * @brief Timestamp of the Focus Move Completed event.
         */
        constexpr PropIdInteger EventFocusMoveCompletedTimestamp = { "EventFocusMoveCompletedTimestamp" };

        /**
         * @brief Unique identifier of the Frame Trigger Missed event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventFrameTriggerMissed = { "EventFrameTriggerMissed" };

        /**
         * @brief Timestamp of the Frame Trigger Missed event.
         */
        constexpr PropIdInteger EventFrameTriggerMissedTimestamp = { "EventFrameTriggerMissedTimestamp" };

        /**
         * @brief Unique identifier of the Line1 Falling Edge event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventLine1FallingEdge = { "EventLine1FallingEdge" };

        /**
         * @brief Timestamp of the Line1 Falling Edge event.
         */
        constexpr PropIdInteger EventLine1FallingEdgeTimestamp = { "EventLine1FallingEdgeTimestamp" };

        /**
         * @brief Unique identifier of the Line1 Rising Edge event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventLine1RisingEdge = { "EventLine1RisingEdge" };

        /**
         * @brief Timestamp of the Line1 Rising Edge event.
         */
        constexpr PropIdInteger EventLine1RisingEdgeTimestamp = { "EventLine1RisingEdgeTimestamp" };

        /**
         * @brief Enables event notifications for the currently selected event. The event can selected using the Event Selector parameter.
         */
        constexpr PropIdEnumeration EventNotification = { "EventNotification" };

        /**
         * @brief Sets the event notification to be enabled. The notification can be enabled using the Event Notification parameter.
         */
        constexpr PropIdEnumeration EventSelector = { "EventSelector" };

        /**
         * @brief Unique identifier of the Test event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventTest = { "EventTest" };

        /**
         * @brief Timestamp of the test event.
         */
        constexpr PropIdInteger EventTestTimestamp = { "EventTestTimestamp" };

        /**
         * @brief Unique identifier of the Zoom Move Completed event. Use this parameter to get notified when the event occurs.
         */
        constexpr PropIdInteger EventZoomMoveCompleted = { "EventZoomMoveCompleted" };

        /**
         * @brief Timestamp of the Zoom Move Completed event.
         */
        constexpr PropIdInteger EventZoomMoveCompletedTimestamp = { "EventZoomMoveCompletedTimestamp" };

        /**
         * @brief Zoom value of the Zoom Move Completed event.
         */
        constexpr PropIdInteger EventZoomMoveCompletedZoom = { "EventZoomMoveCompletedZoom" };

        /**
         * @brief Use the full brightness range when receiving YUV data via HDMI.
         */
        constexpr PropIdBoolean ExpandOutputRange = { "ExpandOutputRange" };

        /**
         * @brief Sets the automatic exposure mode when ExposureMode is Timed.
         */
        constexpr PropIdEnumeration ExposureAuto = { "ExposureAuto" };

        /**
         * @brief Lets the ExposureAuto/GainAuto algorithm try to avoid over-exposures.
         */
        constexpr PropIdBoolean ExposureAutoHighlighReduction = { "ExposureAutoHighlighReduction" };

        /**
         * @brief Lets the ExposureAuto/GainAuto algorithm try to avoid over-exposures.
         */
        constexpr PropIdBoolean ExposureAutoHighlightReduction = { "ExposureAutoHighlightReduction" };

        /**
         * @brief Lower limit of the ExposureAuto function.
         */
        constexpr PropIdFloat ExposureAutoLowerLimit = { "ExposureAutoLowerLimit" };

        /**
         * @brief Configures the target image brightness of the ExposureAuto/GainAuto algorithm.
         */
        constexpr PropIdInteger ExposureAutoReference = { "ExposureAutoReference" };

        /**
         * @brief Upper limit of the ExposureAuto function.
         */
        constexpr PropIdFloat ExposureAutoUpperLimit = { "ExposureAutoUpperLimit" };

        /**
         * @brief Automatically sets the upper limit to match the Acquisition Frame Rate.
         */
        constexpr PropIdBoolean ExposureAutoUpperLimitAuto = { "ExposureAutoUpperLimitAuto" };

        /**
         * @brief Sets the Exposure time when ExposureMode is Timed and ExposureAuto is Off. This controls the duration where the photosensitive cells are exposed to light.
         */
        constexpr PropIdFloat ExposureTime = { "ExposureTime" };

        /**
         * @brief Defines the intermediate access buffer that allows the exchange of data between the device file storage and the application.
         */
        constexpr PropIdRegister FileAccessBuffer = { "FileAccessBuffer" };

        /**
         * @brief Controls the Length of the mapping between the device file storage and the FileAccessBuffer.
         */
        constexpr PropIdInteger FileAccessLength = { "FileAccessLength" };

        /**
         * @brief Controls the Offset of the mapping between the device file storage and the FileAccessBuffer.
         */
        constexpr PropIdInteger FileAccessOffset = { "FileAccessOffset" };

        /**
         * @brief Selects the access mode in which a file is opened in the device.
         */
        constexpr PropIdEnumeration FileOpenMode = { "FileOpenMode" };

        /**
         * @brief Executes the operation selected by FileOperationSelector on the selected file.
         */
        constexpr PropIdCommand FileOperationExecute = { "FileOperationExecute" };

        /**
         * @brief Represents the file operation result. For Read or Write operations, the number of successfully read/written bytes is returned.
         */
        constexpr PropIdInteger FileOperationResult = { "FileOperationResult" };

        /**
         * @brief Selects the target operation for the selected file in the device. This Operation is executed when the FileOperationExecute feature is called.
         */
        constexpr PropIdEnumeration FileOperationSelector = { "FileOperationSelector" };

        /**
         * @brief Represents the file operation execution status.
         */
        constexpr PropIdEnumeration FileOperationStatus = { "FileOperationStatus" };

        /**
         * @brief Selects the target file in the device.
         */
        constexpr PropIdEnumeration FileSelector = { "FileSelector" };

        /**
         * @brief Represents the size of the selected file in bytes.
         */
        constexpr PropIdInteger FileSize = { "FileSize" };

        /**
         * @brief Changes the Focus setting of the lens.
         */
        constexpr PropIdInteger Focus = { "Focus" };

        /**
         * @brief Enables automatic Focus control.
         */
        constexpr PropIdCommand FocusAuto = { "FocusAuto" };

        /**
         * @brief Controls the gain as an absolute physical value. This is an amplification factor applied to he video signal.
         */
        constexpr PropIdFloat Gain = { "Gain" };

        /**
         * @brief Sets the automatic gain control mode.
         */
        constexpr PropIdEnumeration GainAuto = { "GainAuto" };

        /**
         * @brief Lower limit of the GainAuto function.
         */
        constexpr PropIdFloat GainAutoLowerLimit = { "GainAutoLowerLimit" };

        /**
         * @brief Upper limit of the GainAuto function.
         */
        constexpr PropIdFloat GainAutoUpperLimit = { "GainAutoUpperLimit" };

        /**
         * @brief Configures the sensor-specific Gain (LCG/HCG) setting
         */
        constexpr PropIdEnumeration GainMode = { "GainMode" };

        /**
         * @brief Controls the gamma correction of pixel intensity. This is typically used to compensate for nonlinearity of the display system (such as CRT).
         */
        constexpr PropIdFloat Gamma = { "Gamma" };

        /**
         * @brief Enables the extended IDs mode
         */
        constexpr PropIdEnumeration GevGVSPExtendedIDMode = { "GevGVSPExtendedIDMode" };

        /**
         * @brief The state of this feature is copied into the "do not fragment" bit of IP header of each stream packet. It can be used by the application to prevent IP fragmentation of packets on the stream channel.
         */
        constexpr PropIdBoolean GevSCPSDoNotFragment = { "GevSCPSDoNotFragment" };

        /**
         * @brief This GigE Vision specific feature corresponds to DeviceStreamChannelPacketSize and should be kept in sync with it. It specifies the stream packet size, in bytes, to send on the selected channel for a GVSP transmitter or specifies the maximum packet size supported by a GVSP receiver.
         */
        constexpr PropIdInteger GevSCPSPacketSize = { "GevSCPSPacketSize" };

        /**
         * @brief Status of the digital input pin.
         */
        constexpr PropIdInteger GPIn = { "GPIn" };

        /**
         * @brief Status of the digital output pin.
         */
        constexpr PropIdInteger GPOut = { "GPOut" };

        /**
         * @brief Height of the image provided by the device (in pixels).
         */
        constexpr PropIdInteger Height = { "Height" };

        /**
         * @brief Maximum height of the image (in pixels). This dimension is calculated after vertical binning, decimation or any other function changing the vertical dimension of the image.
         */
        constexpr PropIdInteger HeightMax = { "HeightMax" };

        /**
         * @brief Color Hue
         */
        constexpr PropIdFloat Hue = { "Hue" };

        /**
         * @brief Enable IMX174 WDR mode. This mode enables setting a second, longer exposure time (IMX174 WDR Shutter 2), which is used for every other frame.
         */
        constexpr PropIdBoolean IMX174HardwareWDREnable = { "IMX174HardwareWDREnable" };

        /**
         * @brief Select the IMX174 WDR shutter mode. This mode decides whether the second shutter time can be choosen freely, or is fixed at a multiple of the configured ExposureTime.
         */
        constexpr PropIdEnumeration IMX174HardwareWDRShutterMode = { "IMX174HardwareWDRShutterMode" };

        /**
         * @brief Exposure time that is used for every other frame, if IMX174 WDR mode is enabled.
         */
        constexpr PropIdFloat IMX174WDRShutter2 = { "IMX174WDRShutter2" };

        /**
         * @brief Enables the low-latency mode for IMX global shutter sensors. This mode reduces the jitter between trigger input and exposure start, but disallows trigger overlap.
         */
        constexpr PropIdBoolean IMXLowLatencyTriggerMode = { "IMXLowLatencyTriggerMode" };

        /**
         * @brief The detected bits per pixel of the HDMI source.
         */
        constexpr PropIdInteger InputBits = { "InputBits" };

        /**
         * @brief The detected frame rate of the HDMI source.
         */
        constexpr PropIdInteger InputFp1ks = { "InputFp1ks" };

        /**
         * @brief The detected width of the HDMI input image.
         */
        constexpr PropIdInteger InputHeight = { "InputHeight" };

        /**
         * @brief The detected width of the HDMI input image.
         */
        constexpr PropIdInteger InputWidth = { "InputWidth" };

        /**
         * @brief Enables the IRCutFilter in from of the sensor.
         */
        constexpr PropIdBoolean IRCutFilterEnable = { "IRCutFilterEnable" };

        /**
         * @brief Changes the Iris setting of the lens.
         */
        constexpr PropIdInteger Iris = { "Iris" };

        /**
         * @brief Enables automatic Iris control.
         */
        constexpr PropIdBoolean IrisAuto = { "IrisAuto" };

        /**
         * @brief Activates the LUT.
         */
        constexpr PropIdBoolean LUTEnable = { "LUTEnable" };

        /**
         * @brief Control the index (offset) of the coefficient to access in the LUT.
         */
        constexpr PropIdInteger LUTIndex = { "LUTIndex" };

        /**
         * @brief Selects which LUT to control.
         */
        constexpr PropIdEnumeration LUTSelector = { "LUTSelector" };

        /**
         * @brief Returns or sets the Value at entry LUTIndex of the LUT.
         */
        constexpr PropIdInteger LUTValue = { "LUTValue" };

        /**
         * @brief Accesses all the LUT coefficients in a single access without using individual LUTIndex.
         */
        constexpr PropIdRegister LUTValueAll = { "LUTValueAll" };

        /**
         * @brief Enables the sensor to use customizable gain values for the different frames of a frame set.
         */
        constexpr PropIdBoolean MultiFrameSetOutputModeCustomGain = { "MultiFrameSetOutputModeCustomGain" };

        /**
         * @brief Enables the sensor to output a sequence of 2 or 4 frames with configurable exposure times and gain values.
         */
        constexpr PropIdBoolean MultiFrameSetOutputModeEnable = { "MultiFrameSetOutputModeEnable" };

        /**
         * @brief Configures the exposure time for frame 0 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeExposureTime0 = { "MultiFrameSetOutputModeExposureTime0" };

        /**
         * @brief Configures the exposure time for frame 1 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeExposureTime1 = { "MultiFrameSetOutputModeExposureTime1" };

        /**
         * @brief Configures the exposure time for frame 2 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeExposureTime2 = { "MultiFrameSetOutputModeExposureTime2" };

        /**
         * @brief Configures the exposure time for frame 3 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeExposureTime3 = { "MultiFrameSetOutputModeExposureTime3" };

        /**
         * @brief Selects the number of frames in multi frame set output mode.
         */
        constexpr PropIdEnumeration MultiFrameSetOutputModeFrameCount = { "MultiFrameSetOutputModeFrameCount" };

        /**
         * @brief Configures the gain for frame 0 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeGain0 = { "MultiFrameSetOutputModeGain0" };

        /**
         * @brief Configures the gain for frame 1 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeGain1 = { "MultiFrameSetOutputModeGain1" };

        /**
         * @brief Configures the gain for frame 2 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeGain2 = { "MultiFrameSetOutputModeGain2" };

        /**
         * @brief Configures the gain for frame 3 of the frame set.
         */
        constexpr PropIdFloat MultiFrameSetOutputModeGain3 = { "MultiFrameSetOutputModeGain3" };

        /**
         * @brief Automatically adjust the values of OffsetX and OffsetY to select the center region of the sensor.
         */
        constexpr PropIdEnumeration OffsetAutoCenter = { "OffsetAutoCenter" };

        /**
         * @brief Horizontal offset from the origin to the region of interest (in pixels).
         */
        constexpr PropIdInteger OffsetX = { "OffsetX" };

        /**
         * @brief Vertical offset from the origin to the region of interest (in pixels).
         */
        constexpr PropIdInteger OffsetY = { "OffsetY" };

        /**
         * @brief Provides the number of bytes transferred for each image or chunk on the stream channel. This includes any end-of-line, end-of-frame statistics or other stamp data. This is the total size of data payload for a data block.
         */
        constexpr PropIdInteger PayloadSize = { "PayloadSize" };

        /**
         * @brief Format of the pixels provided by the device
         */
        constexpr PropIdEnumeration PixelFormat = { "PixelFormat" };

        /**
         * @brief Indicates the expected accuracy of the device PTP clock when it is the grandmaster, or in the event it becomes the grandmaster.
         */
        constexpr PropIdEnumeration PtpClockAccuracy = { "PtpClockAccuracy" };

        /**
         * @brief Enables the Precision Time Protocol (PTP).
         */
        constexpr PropIdBoolean PtpEnable = { "PtpEnable" };

        /**
         * @brief Returns the current state of the PTP clock.
         */
        constexpr PropIdEnumeration PtpStatus = { "PtpStatus" };

        /**
         * @brief Flip the image horizontally.
         */
        constexpr PropIdBoolean ReverseX = { "ReverseX" };

        /**
         * @brief Flip the image vertically.
         */
        constexpr PropIdBoolean ReverseY = { "ReverseY" };

        /**
         * @brief Color Saturation
         */
        constexpr PropIdFloat Saturation = { "Saturation" };

        /**
         * @brief Effective height of the sensor in pixels.
         */
        constexpr PropIdInteger SensorHeight = { "SensorHeight" };

        /**
         * @brief Physical size (pitch) in the y direction of a photo sensitive pixel unit.
         */
        constexpr PropIdFloat SensorPixelHeight = { "SensorPixelHeight" };

        /**
         * @brief Physical size (pitch) in the x direction of a photo sensitive pixel unit.
         */
        constexpr PropIdFloat SensorPixelWidth = { "SensorPixelWidth" };

        /**
         * @brief Effective width of the sensor in pixels.
         */
        constexpr PropIdInteger SensorWidth = { "SensorWidth" };

        /**
         * @brief Controls the strength of the sharpness algorithm.
         */
        constexpr PropIdInteger Sharpness = { "Sharpness" };

        /**
         * @brief Enables Trigger/Strobe over USB-C Sideband Use Pins
         */
        constexpr PropIdBoolean SidebandUse = { "SidebandUse" };

        /**
         * @brief Indicates whether a signal is detected form the HDMI source.
         */
        constexpr PropIdBoolean SignalDetected = { "SignalDetected" };

        /**
         * @brief Enables Software Transform properties. If enabled, properties like Sharpness, Tone Mapping etc. can be enabled and lead to a software transformation in the driver.
         */
        constexpr PropIdBoolean SoftwareTransformEnable = { "SoftwareTransformEnable" };

        /**
         * @brief Indicates whether a HDMI source is connected.
         */
        constexpr PropIdBoolean SourceConnected = { "SourceConnected" };

        /**
         * @brief Configures the delay from start of exposure to the generated strobe pulse.
         */
        constexpr PropIdInteger StrobeDelay = { "StrobeDelay" };

        /**
         * @brief If StrobeOperation is FixedDuration, specifies the pulse length.
         */
        constexpr PropIdInteger StrobeDuration = { "StrobeDuration" };

        /**
         * @brief Enable generation of exposure-synchronized output pulses
         */
        constexpr PropIdEnumeration StrobeEnable = { "StrobeEnable" };

        /**
         * @brief Configures the mode of specifying the length of exposure-synchronized output pulses
         */
        constexpr PropIdEnumeration StrobeOperation = { "StrobeOperation" };

        /**
         * @brief Select level of exposure-synchronized output pulses
         */
        constexpr PropIdEnumeration StrobePolarity = { "StrobePolarity" };

        /**
         * @brief Generates a Test event that can be used for testing event notification.
         */
        constexpr PropIdCommand TestEventGenerate = { "TestEventGenerate" };

        /**
         * @brief Tests the device's pending acknowledge feature.
         */
        constexpr PropIdInteger TestPendingAck = { "TestPendingAck" };

        /**
         * @brief Latches the current timestamp counter into TimestampLatchValue.
         */
        constexpr PropIdCommand TimestampLatch = { "TimestampLatch" };

        /**
         * @brief Human-readable interpretation of the latched timestamp.
         */
        constexpr PropIdString TimestampLatchString = { "TimestampLatchString" };

        /**
         * @brief Returns the latched value of the timestamp counter. The timestamp must first be latched by using the Timestamp Control Latch command.
         */
        constexpr PropIdInteger TimestampLatchValue = { "TimestampLatchValue" };

        /**
         * @brief Resets the current value of the device timestamp counter. After executing this command, the timestamp counter restarts automatically.
         */
        constexpr PropIdCommand TimestampReset = { "TimestampReset" };

        /**
         * @brief Used by the Transport Layer to prevent critical features from changing during acquisition.
         */
        constexpr PropIdInteger TLParamsLocked = { "TLParamsLocked" };

        /**
         * @brief Enables tone mapping.
         */
        constexpr PropIdBoolean ToneMappingEnable = { "ToneMappingEnable" };

        /**
         * @brief Changes the brightness reference used for a individual pixel, which is interpolated between local and global.
         */
        constexpr PropIdFloat ToneMappingGlobalBrightness = { "ToneMappingGlobalBrightness" };

        /**
         * @brief Adjusts the intensity of the tonemapping algorithm.
         */
        constexpr PropIdFloat ToneMappingIntensity = { "ToneMappingIntensity" };

        /**
         * @brief Specifies the activation mode of the trigger
         */
        constexpr PropIdEnumeration TriggerActivation = { "TriggerActivation" };

        /**
         * @brief Specifies the time for which trigger input has to be low in order accept the next trigger signal.
         */
        constexpr PropIdFloat TriggerDebouncer = { "TriggerDebouncer" };

        /**
         * @brief Specifies the delay to apply after the trigger reception before activating it.
         */
        constexpr PropIdFloat TriggerDelay = { "TriggerDelay" };

        /**
         * @brief Specifies the time for which trigger input has to be high in order to be accepted as a trigger signal.
         */
        constexpr PropIdFloat TriggerDenoise = { "TriggerDenoise" };

        /**
         * @brief Specifies the time for which trigger pulses are ignored after accepting a trigger signal.
         */
        constexpr PropIdFloat TriggerMask = { "TriggerMask" };

        /**
         * @brief Controls if the selected trigger is active.
         */
        constexpr PropIdEnumeration TriggerMode = { "TriggerMode" };

        /**
         * @brief Controls the operation mode of the sensor in trigger mode.
         */
        constexpr PropIdEnumeration TriggerOperation = { "TriggerOperation" };

        /**
         * @brief Specifies the type trigger overlap permitted with the previous frame. This defines when a valid trigger will be accepted for a new frame.
         */
        constexpr PropIdEnumeration TriggerOverlap = { "TriggerOverlap" };

        /**
         * @brief Selects the type of trigger to configure.
         */
        constexpr PropIdEnumeration TriggerSelector = { "TriggerSelector" };

        /**
         * @brief Generates an internal trigger. TriggerSource must be set to Software or Any.
         */
        constexpr PropIdCommand TriggerSoftware = { "TriggerSoftware" };

        /**
         * @brief Specifies the internal signal or physical input Line to use as the trigger source.
         */
        constexpr PropIdEnumeration TriggerSource = { "TriggerSource" };

        /**
         * @brief Selects the feature User Set to load and make active by default when the device is reset.
         */
        constexpr PropIdEnumeration UserSetDefault = { "UserSetDefault" };

        /**
         * @brief Loads the User Set specified by UserSetSelector to the device and makes it active.
         */
        constexpr PropIdCommand UserSetLoad = { "UserSetLoad" };

        /**
         * @brief Save the User Set specified by UserSetSelector to the non-volatile memory of the device.
         */
        constexpr PropIdCommand UserSetSave = { "UserSetSave" };

        /**
         * @brief Selects the feature User Set to load, save or configure.
         */
        constexpr PropIdEnumeration UserSetSelector = { "UserSetSelector" };

        /**
         * @brief Width of the image provided by the device (in pixels).
         */
        constexpr PropIdInteger Width = { "Width" };

        /**
         * @brief Maximum width of the image (in pixels). The dimension is calculated after horizontal binning, decimation or any other function changing the horizontal dimension of the image.
         */
        constexpr PropIdInteger WidthMax = { "WidthMax" };

        /**
         * @brief Changes the zoom setting of the lens.
         */
        constexpr PropIdInteger Zoom = { "Zoom" };

    }
}

#endif // IC4_PROPERTY_CONSTANTS_H_INC_
