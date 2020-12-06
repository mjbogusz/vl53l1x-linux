#include <cstdint>

#define SOFT_RESET 0x0000
#define VL53L1_I2C_SLAVE__DEVICE_ADDRESS 0x0001
#define VL53L1_VHV_CONFIG__TIMEOUT_MACROP_LOOP_BOUND 0x0008
#define ALGO__CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS 0x0016
#define ALGO__CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS 0x0018
#define ALGO__CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS 0x001A
#define ALGO__PART_TO_PART_RANGE_OFFSET_MM 0x001E
#define MM_CONFIG__INNER_OFFSET_MM 0x0020
#define MM_CONFIG__OUTER_OFFSET_MM 0x0022
#define GPIO_HV_MUX__CTRL 0x0030
#define GPIO__TIO_HV_STATUS 0x0031
#define SYSTEM__INTERRUPT_CONFIG_GPIO 0x0046
#define PHASECAL_CONFIG__TIMEOUT_MACROP 0x004B
#define RANGE_CONFIG__TIMEOUT_MACROP_A_HI 0x005E
#define RANGE_CONFIG__VCSEL_PERIOD_A 0x0060
#define RANGE_CONFIG__VCSEL_PERIOD_B 0x0063
#define RANGE_CONFIG__TIMEOUT_MACROP_B_HI 0x0061
#define RANGE_CONFIG__TIMEOUT_MACROP_B_LO 0x0062
#define RANGE_CONFIG__SIGMA_THRESH 0x0064
#define RANGE_CONFIG__MIN_COUNT_RATE_RTN_LIMIT_MCPS 0x0066
#define RANGE_CONFIG__VALID_PHASE_HIGH 0x0069
#define VL53L1_SYSTEM__INTERMEASUREMENT_PERIOD 0x006C
#define SYSTEM__THRESH_HIGH 0x0072
#define SYSTEM__THRESH_LOW 0x0074
#define SD_CONFIG__WOI_SD0 0x0078
#define SD_CONFIG__INITIAL_PHASE_SD0 0x007A
#define ROI_CONFIG__USER_ROI_CENTRE_SPAD 0x007F
#define ROI_CONFIG__USER_ROI_REQUESTED_GLOBAL_XY_SIZE 0x0080
#define SYSTEM__SEQUENCE_CONFIG 0x0081
#define VL53L1_SYSTEM__GROUPED_PARAMETER_HOLD 0x0082
#define SYSTEM__INTERRUPT_CLEAR 0x0086
#define SYSTEM__MODE_START 0x0087
#define VL53L1_RESULT__RANGE_STATUS 0x0089
#define VL53L1_RESULT__DSS_ACTUAL_EFFECTIVE_SPADS_SD0 0x008C
#define RESULT__AMBIENT_COUNT_RATE_MCPS_SD 0x0090
#define VL53L1_RESULT__FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0 0x0096
#define VL53L1_RESULT__PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0 0x0098
#define VL53L1_RESULT__OSC_CALIBRATE_VAL 0x00DE
#define VL53L1_FIRMWARE__SYSTEM_STATUS 0x00E5
#define VL53L1_IDENTIFICATION__MODEL_ID 0x010F
#define VL53L1_ROI_CONFIG__MODE_ROI_CENTRE_SPAD 0x013E

#define VL53L1X_DEFAULT_DEVICE_ADDRESS 0x29
#define CMDRECVBUFSIZE 20

enum eVL53L1X_Status {
	eVL53L1X_ok,
	eVL53L1X_InitError,
	eVL53L1X_WriteRegError,
	eVL53L1X_ReadRegError
};

typedef enum {
	eVL53L1X_Below = 0,
	eVL53L1X_Above = 1,
	eVL53L1X_Out = 2,
	eVL53L1X_In = 3
} eWindows;

typedef enum {
	eBudget_15ms = 15,
	eBudget_20ms = 20,
	eBudget_33ms = 33,
	eBudget_50ms = 50,
	eBudget_100ms = 100,
	eBudget_200ms = 200,
	eBudget_500ms = 500
} eTimingBudget;

class VL53L1X {
public:
	VL53L1X();

	/**
	 * Initialize the sensor and check whether the data is ready
	 *
	 * This function loads the 135 bytes default values to initialize the sensor.
	 *
	 * @return true if initialization has succeeded
	 */
	bool begin();

	/**
	 * Set the interrupt polarity to high
	 */
	void setInterruptPolarityHigh();

	/**
	 * Set the interrupt polarity to low
	 */
	void setInterruptPolarityLow();

	/**
	 * Start the continuous ranging operation
	 */
	void startRanging();

	/**
	 * Stop the ranging operation
	 */
	void stopRanging();

	/**
	 * Set the timing budget in ms
	 *
	 * @see VL53L1X::eTimingBudget for possible values
	 *
	 * @param eTimingBudget The timing budget to set
	 */
	void setTimingBudgetInMs(eTimingBudget timingBudget);

	/**
	 * Get the current timing budget in ms
	 *
	 * @return timing budget
	 */
	eTimingBudget getTimingBudgetInMs();

	/**
	 * Get the interrupt polarity.
	 *
	 * @return The interrupt polarity.
	 */
	uint8_t getInterruptPolarity();

	/**
	 * Check whether the distance data is ready
	 *
	 * @return True if the data is ready
	 */
	bool checkForDataReady();

	/**
	 * Set the sensor into the long distance mode
	 *
	 * Long mode can range up to 4 m in the dark with a timing budget of 200 ms
	 */
	void setDistanceModeLong();

	/**
	 * Set the sensor into the short distance mode
	 *
	 * Short mode maximum distance is limited to 2.2m (or 1.3???)
	 */
	void setDistanceModeShort();

	/**
	 * Get the current distance mode.
	 *
	 * @return The distance mode: 1 - short, 2 - long
	 */
	uint8_t getDistanceMode();

	/**
	 * Set the intermeasurement period (IMP) in ms
	 *
	 * @param InterMeasurement must be greater than or equal to the timing budget range (0 ~ 1693)
	 */
	void setInterMeasurementInMs(uint16_t interMeasurement);

	/**
	 * Get the intermeasurement period in ms
	 *
	 * @return The intermeasurement period in ms
	 */
	uint16_t getInterMeasurementInMs();

	/**
	 * Get the distance measured by the sensor in mm
	 *
	 * @return The measured distance
	 */
	uint16_t getDistance();

	/**
	 * Apply the correction offset value (in millimeters) to the sensor
	 *
	 * @param The offset, in mm, to apply to the sensor (range: -1024 ~ 1023)
	 *
	 * @note This value is to be found during calibration, stored in the host system and applied on every startup.
	 */
	void setOffset(int16_t OffsetValue);

	/**
	 * Get the currently set correction offset programmed in the sensor
	 *
	 * @return The saved offset value, in mm
	 */
	int16_t getOffset();

	/**
	 * Apply the crosstalk value (in counts per second) to the sensor
	 *
	 * @param The crosstalk correction to apply
	 *
	 * @note As with the offset, this value is to be found during calibration, stored in the host system and applied on every startup.
	 * @note XtalkValue = 512*(SignalRate*(1-(Distance/targetDistance)))/SpadNb
	 */
	void setXTalk(uint16_t XtalkValue);

	/**
	 * Get the current crosstalk value programmed in the sensor
	 *
	 * @note Zero means there is no crosstalk compensation
	 *
	 * @return The saved crosstalk value, in cps
	 */
	uint16_t getXTalk();

	/**
	 * Set distance threshold
	 *
	 * @param ThreshLow (in mm) is the threshold under which the device raises an interrupt if window == eVL53L1X_Below
	 * @param ThreshHigh (in mm) is the threshold above which the device raises an interrupt if window == eVL53L1X_Above
	 * @param IntOnNoTarget is the window detection mode
	 */
	void setDistanceThreshold(uint16_t ThreshLow, uint16_t ThreshHigh, eWindows Window);

	/**
	 * This function returns the window detection mode where 0 = below, 1 = above, 2 = out and 3 = in
	 *
	 * @return The threshold window, @see VL53L1X::eWindows for values
	 */
	eWindows getDistanceThresholdWindow();

	/**
	 * This function returns the low threshold in mm
	 *
	 * @return The distance low threshold
	 */
	uint16_t getDistanceThresholdLow();

	/**
	 * This function returns the high threshold in mm
	 *
	 * @return The distance high threshold
	 */
	uint16_t getDistanceThresholdHigh();

	/**
	 * This function finds the offset and applies the offset
	 *        The offset correction value must be stored in the host system
	 *
	 * @param The target distance in mm
	 */
	int8_t calibrateOffset(uint16_t targetDistInMm);

	/**
	 * This function finds the crosstalk compensation value and applies the correction
	 *
	 * @return The target distance in mm
	 */
	int8_t calibrateXTalk(uint16_t targetDistInMm);

	void clearInterrupt();

	/**
	 * Status of the sensor.
	 *
	 * @see eVL53L1X_Status for possible values.
	 */
	eVL53L1X_Status lastOperateStatus;

protected:
	bool cmdSerialDataAvailable(); // Return true if it detects that the serial port has data, otherwise it returns false

	int cmdPrase(); // Parsing serial data commands

	void calibration(int mode); // enter calibration mode to calibration sensor

	void setInterruptPolarity(uint8_t NewPolarity); // set Interrupt Polarity

	void setDistanceMode(uint16_t DM); // Set distance mode,long: 0~4m,short: 0~1.3m

private:
	uint8_t addr = 0x29;
	char cmdRecvBuf[CMDRECVBUFSIZE + 1];
	int cmdRecvBufIndex;
	double decimal;

	uint16_t getSignalRate(); // get signal rate

	void setSigmaThreshold(uint16_t Sigma); // set Sigma Threshold

	void writeByteData(uint16_t index, uint8_t data); // write data for 1 bytes

	void writeWordData(uint16_t index, uint16_t data); // write data for 2 bytes

	void writeWordData32(uint16_t index, uint32_t data); // write data for 4 bytes

	void readByteData(uint16_t index, uint8_t* data); // read a byte data

	void readWordData(uint16_t index, uint16_t* data); // Read data for a bytes

	void readWordData32(uint16_t index, uint32_t* data); // Read data for 4 bytes

	void i2CWrite(uint16_t reg, uint8_t* pBuf, uint16_t len); // IIC writes len bytes of data

	void i2CRead(uint16_t reg, uint8_t* pBuf, uint16_t len); // IIC reads len bytes of data
};
