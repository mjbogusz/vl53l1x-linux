#pragma once

#include <GPIOPin.hpp>
#include <I2CBus.hpp>

#include <chrono>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>

class VL53L1X: public std::enable_shared_from_this<VL53L1X> {
public:
	/**
	 * A shared_ptr alias (use as VL53L1X::SharedPtr)
	 */
	using SharedPtr = std::shared_ptr<VL53L1X>;

	/**
	 * A shared_ptr to a constant alias (use as VL53L1X::ConstSharedPtr)
	 */
	using ConstSharedPtr = std::shared_ptr<const VL53L1X>;

	/**
	 * Available distance measuring modes, used in VL53L1X::setDistanceMode()
	 */
	enum DistanceMode : uint8_t {
		DISTANCE_MODE_SHORT,
		DISTANCE_MODE_LONG,
		DISTANCE_MODE_UNKNOWN
		// Medium: TBD
	};

	/**
	 * Available measurement timing budgets (milliseconds), used in VL53L1X::setTimingBudget()
	 */
	enum TimingBudget : uint16_t {
		TIMING_BUDGET_15_MS = 15,
		TIMING_BUDGET_20_MS = 20,
		TIMING_BUDGET_33_MS = 33,
		TIMING_BUDGET_50_MS = 50,
		TIMING_BUDGET_100_MS = 100,
		TIMING_BUDGET_200_MS = 200,
		TIMING_BUDGET_500_MS = 500
	};

	/**
	 * Create a new VL53L1X sensor instance.
	 *
	 * @param i2cBus The I2C bus to use
	 * @param gpioPin The GPIO pin, connected to the sensor's XSHUT pin (nullptr disables shutting down the sensor)
	 * @param address The sensor's address (only needed if already set to other than the default)
	 * @param timeout The measurement timeout (default = 0 means no timeout)
	 */
	explicit VL53L1X(
		I2CBus::SharedPtr i2cBus,
		GPIOPin::SharedPtr gpioPin = nullptr,
		uint8_t address = VL53L1X::DEFAULT_DEVICE_ADDRESS,
		std::chrono::milliseconds timeout = std::chrono::milliseconds(0)
	);

	/**
	 * Initialize the sensor and check whether the data is ready
	 *
	 * This function loads the 135 bytes default values to initialize the sensor.
	 */
	void initialize();

	/**
	 * Power on the sensor by setting its XSHUT pin to high via host's GPIO.
	 */
	void powerOn();

	/**
	 * Power off the sensor by setting its XSHUT pin to low via host's GPIO.
	 */
	void powerOff();

	/**
	 * Change sensor's I2C address (sets both the address on the physical sensor and within sensor's object).
	 */
	void setAddress(uint8_t newAddress);

	/**
	 * Start the continuous ranging operation
	 */
	void startRanging();

	/**
	 * Stop the ranging operation
	 */
	void stopRanging();

	/**
	 * Check whether the distance data is ready
	 *
	 * @return True if the data is ready
	 */
	bool isDataReady();

	/**
	 * Get the distance measured by the sensor in mm.
	 *
	 * This method can return 2 special values:
	 *  - 65535 means a timeout has occured
	 *  - 16384 means an out-of-range measurement (>4m)
	 *
	 * @return The measured distance
	 */
	uint16_t getDistance();

	/**
	 * Clear the interrupt flag of the sensor
	 */
	void clearInterrupt();

	/**
	 * Set the distance mode, long: 0~4m, short: 0~1.3m
	 *
	 * @param mode The new mode
	 */
	void setDistanceMode(VL53L1X::DistanceMode mode);

	/**
	 * Get the current distance mode.
	 *
	 * @return The distance mode
	 */
	VL53L1X::DistanceMode getDistanceMode();

	/**
	 * Set the timing budget
	 *
	 * @see VL53L1X::TimingBudget for possible values
	 *
	 * @param timingBudget The timing budget to set
	 */
	void setTimingBudget(VL53L1X::TimingBudget timingBudget);

	/**
	 * Get the current timing budget in ms
	 *
	 * @return timing budget
	 */
	VL53L1X::TimingBudget getTimingBudget();

	/**
	 * Set the inter-measurement period (IMP) in ms
	 *
	 * @param period must be greater than or equal to the timing budget range (0 ~ 1693)
	 */
	void setInterMeasurementPeriod(uint16_t period);

	/**
	 * Get the inter-measurement period in ms
	 *
	 * @return The inter-measurement period
	 */
	uint16_t getInterMeasurementPeriod();

	/**
	 * Apply the correction offset value (in millimeters) to the sensor
	 *
	 * @param offsetValue The offset, in mm, to apply to the sensor (range: -1024 ~ 1023)
	 *
	 * @note This value is to be found during calibration, stored in the host system and applied on every startup.
	 */
	void setOffset(int16_t offsetValue);

	/**
	 * Get the currently set correction offset programmed in the sensor
	 *
	 * @return The saved offset value, in mm
	 */
	int16_t getOffset();

	/**
	 * Apply the crosstalk value (in counts per second) to the sensor
	 *
	 * @param crosstalkValue The crosstalk correction to apply
	 *
	 * @note As with the offset, this value is to be found during calibration, stored in the host system and applied on every startup.
	 * @note crosstalkValue = 512*(SignalRate*(1-(Distance/targetDistance)))/SpadNb
	 */
	void setCrosstalk(uint16_t crosstalkValue);

	/**
	 * Get the current crosstalk value programmed in the sensor
	 *
	 * @note Zero means there is no crosstalk compensation
	 *
	 * @return The saved crosstalk value, in cps
	 */
	uint16_t getCrosstalk();

	/**
	 * Get the low threshold in mm
	 *
	 * @return The distance low threshold
	 */
	uint16_t getDistanceThresholdLow();

	/**
	 * Get the high threshold in mm
	 *
	 * @return The distance high threshold
	 */
	uint16_t getDistanceThresholdHigh();

	/**
	 * Find and apply the offset correction value
	 *
	 * @note The offset correction value must be stored in the host system
	 *
	 * @param targetDistance The target distance in mm to calibrate against
	 */
	int8_t calibrateOffset(uint16_t targetDistance);

	/**
	 * Find and apply the crosstalk compensation value
	 *
	 * @note as with calibrateOffset(), the value must be stored by the host
	 *
	 * @param targetDistance The target distance in mm to calibrate against
	 */
	int8_t calibrateCrosstalk(uint16_t targetDistance);

	/**
	 * Create a SharedPtr instance of the VL53L1X.
	 *
	 * Usage: `VL53L1X::makeShared(args...)`.
	 * See constructor (@ref VL53L1X::VL53L1X()) for details.
	 */
	template<typename ... Args>
	static VL53L1X::SharedPtr makeShared(Args&& ... args) {
		return std::make_shared<VL53L1X>(std::forward<Args>(args) ...);
	}

private:
	static constexpr uint8_t DEFAULT_DEVICE_ADDRESS = 0x29;

	static const uint8_t DEFAULT_CONFIGURATION[91];

	enum RegisterAddresses : uint16_t;

	I2CBus::SharedPtr i2cBus;

	GPIOPin::SharedPtr gpioPin;

	/**
	 * I2C address of the sensor
	 */
	uint8_t address;

	const std::chrono::milliseconds timeout;

	uint8_t interruptPolarity;

	/**
	 * wtf?
	 */
	double decimal;

	// get signal rate
	uint16_t getSignalRate();

	// set Sigma Threshold
	void setSigmaThreshold(uint16_t Sigma);
};

enum VL53L1X::RegisterAddresses : uint16_t {
	SOFT_RESET = 0x0000,
	I2C_SLAVE_DEVICE_ADDRESS = 0x0001,
	VHV_CONFIG_TIMEOUT_MACROP_LOOP_BOUND = 0x0008,
	VHV_CONFIG_INIT = 0x000B,
	ALGO_CROSSTALK_COMPENSATION_PLANE_OFFSET_KCPS = 0x0016,
	ALGO_CROSSTALK_COMPENSATION_X_PLANE_GRADIENT_KCPS = 0x0018,
	ALGO_CROSSTALK_COMPENSATION_Y_PLANE_GRADIENT_KCPS = 0x001A,
	ALGO_PART_TO_PART_RANGE_OFFSET_MM = 0x001E,
	MM_CONFIG_INNER_OFFSET_MM = 0x0020,
	MM_CONFIG_OUTER_OFFSET_MM = 0x0022,
	GPIO_HV_MUX_CTRL = 0x0030,
	GPIO_TIO_HV_STATUS = 0x0031,
	SYSTEM_INTERRUPT_CONFIG_GPIO = 0x0046,
	PHASECAL_CONFIG_TIMEOUT_MACROP = 0x004B,
	RANGE_CONFIG_TIMEOUT_MACROP_A_HI = 0x005E,
	RANGE_CONFIG_VCSEL_PERIOD_A = 0x0060,
	RANGE_CONFIG_VCSEL_PERIOD_B = 0x0063,
	RANGE_CONFIG_TIMEOUT_MACROP_B_HI = 0x0061,
	RANGE_CONFIG_TIMEOUT_MACROP_B_LO = 0x0062,
	RANGE_CONFIG_SIGMA_THRESH = 0x0064,
	RANGE_CONFIG_MIN_COUNT_RATE_RTN_LIMIT_MCPS = 0x0066,
	RANGE_CONFIG_VALID_PHASE_HIGH = 0x0069,
	VL53L1_SYSTEM_INTERMEASUREMENT_PERIOD = 0x006C,
	SYSTEM_THRESH_HIGH = 0x0072,
	SYSTEM_THRESH_LOW = 0x0074,
	SD_CONFIG_WOI_SD0 = 0x0078,
	SD_CONFIG_INITIAL_PHASE_SD0 = 0x007A,
	ROI_CONFIG_USER_ROI_CENTRE_SPAD = 0x007F,
	ROI_CONFIG_USER_ROI_REQUESTED_GLOBAL_XY_SIZE = 0x0080,
	SYSTEM_SEQUENCE_CONFIG = 0x0081,
	VL53L1_SYSTEM_GROUPED_PARAMETER_HOLD = 0x0082,
	SYSTEM_INTERRUPT_CLEAR = 0x0086,
	SYSTEM_MODE_START = 0x0087,
	VL53L1_RESULT_RANGE_STATUS = 0x0089,
	VL53L1_RESULT_DSS_ACTUAL_EFFECTIVE_SPADS_SD0 = 0x008C,
	RESULT_AMBIENT_COUNT_RATE_MCPS_SD = 0x0090,
	VL53L1_RESULT_FINAL_CROSSTALK_CORRECTED_RANGE_MM_SD0 = 0x0096,
	VL53L1_RESULT_PEAK_SIGNAL_COUNT_RATE_CROSSTALK_CORRECTED_MCPS_SD0 = 0x0098,
	VL53L1_RESULT_OSC_CALIBRATE_VAL = 0x00DE,
	VL53L1_FIRMWARE_SYSTEM_STATUS = 0x00E5,
	VL53L1_IDENTIFICATION_MODEL_ID = 0x010F,
	VL53L1_ROI_CONFIG_MODE_ROI_CENTRE_SPAD = 0x013E,
};
