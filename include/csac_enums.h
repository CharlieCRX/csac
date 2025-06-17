#ifndef _ENUMS_H_
#define _ENUMS_H_
#include <stdint.h>
typedef enum {
  Asleep = 9,               // ULP mode only
  Warmup = 8,               // Initial warm-up
  HeaterEquilibration = 7,  // Heater equilibration
  MicrowaveAcquisition = 6, // Microwave power acquisition
  LaserCurrentAcquisition = 5, // Laser current acquisition
  LaserPowerAcquisition = 4,   // Laser power acquisition
  MicrowaveFrequencyAcquisition = 3, // Microwave frequency acquisition
  MicrowaveFrequencyStabilization = 2, // Microwave frequency stabilization
  MicrowaveFrequencySteering = 1,   // Microwave frequency steering
  Locked = 0                // Locked
} E_telemetry_status;

typedef enum {
  Normal = 0x0000,                          // Alarm is empty
  SignalContrastLow = 0x0001,               // Signal contrast low. Contrast <1000
  SynthesizerTuningLimit = 0x0002,          // Synthesizer tuning at limit. Synthesizer detuned from calibration by >30 kHz or <–15 kHz
  TemperatureBridgeUnbalanced = 0x0004,     // Temperature bridge unbalanced. Bridge—set-point >±20 mV
  DCLevelLow = 0x0010,                      // DC light level low. Set-point—DCL > 1.5V
  DCLevelHigh = 0x0020,                     // DC light level high. DCL—set-point > 1.5V
  HeaterVoltageLow = 0x0040,                // Heater voltage low. <30 mV
  HeaterVoltageHigh = 0x0080,               // Heater voltage high. >2.48V
  MicroWattPowerControlLow = 0x0100,        // μW power control low. <20 mV
  MicroWattPowerControlHigh = 0x0200,       // μW power control high. >2.48V
  TCXOControlVoltageLow = 0x0400,           // tcxo control voltage low. <0.1V
  TCXOControlVoltageHigh = 0x0800,          // tcxo control voltage high. >2.4V
  LaserCurrentLow = 0x1000,                 // Laser current low. <0.5 mA
  LaserCurrentHigh = 0x2000,                // Laser current high. >2.3 mA
  StackOverflowError = 0x4000               // Stack overflow (firmware error)
} E_telemetry_alarms;

/**
 * @brief Operating modes for CSAC.
 *
 * Each bit represents a toggleable feature or reserved space.
 * Mode values correspond to enable/disable commands and telemetry reports.
 */
typedef enum {
  MODE_NONE                      = 0x0000,  ///<  Initial state, no modes enabled
  MODE_ANALOG_TUNING             = 0x0001,  ///< 'A' Analog tuning, disable with 'a'
  MODE_1PPS_PHASE_MEASUREMENT    = 0x0004,  ///< 'M' 1PPS phase measurement, disable with 'm'
  MODE_1PPS_AUTO_SYNC            = 0x0008,  ///< 'S' 1PPS auto-sync, disable with 's'
  MODE_DISCIPLINE                = 0x0010,  ///< 'D' Discipline, disable with 'd'
  MODE_ULTRA_LOW_POWER           = 0x0020,  ///< 'U' Ultra-low power mode, disable with 'u'
  MODE_CHECKSUM_REQUIRED         = 0x0040,  ///< 'C' Require checksum on '!' commands, disable with 'c'
} E_CSAC_operating_modes;


// 互斥模式掩码 (M/S/D)
#define MUTEX_MODES_MASK (MODE_1PPS_PHASE_MEASUREMENT | \
  MODE_1PPS_AUTO_SYNC | \
  MODE_DISCIPLINE)


typedef struct CSAC_telemetry{
  E_telemetry_status        status;        // Unit status
  E_telemetry_alarms       alarms;         // Pending unit alarms
  char SN[12];                             // Unit serial number
  E_CSAC_operating_modes  modes;      // Mode of operation
  uint32_t  Contrast;     // Indication of signal level
  double laserI;          // Laser current (mA)
  double tcxo;            // Tuning voltage (V)
  double heatP;           // Physics package heater power (mW)
  double sigV;            // DC signal level (V)
  double temp;            // Unit temperature (°C)
  int32_t steer;          // Frequency adjust
  double aTune;           // Analog tuning voltage input (V)
  int16_t phase;          // Difference between CSAC and external 1PPS (ns)
  int8_t disOK;           // Discipline status
  uint32_t TOD;           // Time (seconds)
  uint32_t LTime;         // Time since lock (seconds)
  char fwVer[5];          // Firmware version
} T_CSAC_telemetry;

typedef enum {
  Telemetry_Headers           = '6',
  Telemetry_Data              = '^',
  Freqyency_Adjustment        = 'F',
  Operating_Modes             = 'M',
  Discipline_Time             = 'D',
  U_L_power_param             = 'U',
  Time_day                    = 'T',
  Discipline_Check            = 'm',
  Pulse_Multiple              = '>',
  Deferred_Command            = '@',
  Help                        = '?'
} Telemetry_COMMAND_type;

/** mode args */
typedef enum {
  Enable_Analog                   = 'A',    //Enable Analog tuning
  Enable_Phase_Measure            = 'M',    //1PPS phase measurement (only available on firmware versions 1.08 and later).
  Enable_Sync                     = 'S',    //1PPS auto-sync.
  Enable_Discipline               = 'D',    //Discipline
  Enable_U_LOW_POWER              = 'U',    //Ultra-Low Power mode
  Enable_Checksum                 = 'C',    //Require checksum on ! command.

  DisEnable_Analog                = 'a',
  DisEnable_Phase_Measure         = 'm',
  DisEnable_Sync                  = 's',
  DisEnable_Discipline            = 'd',
  DisEnable_U_LOW_POWER           = 'u',
  DisEnable_Checksum              = 'c',

  Report_current_settings         = '?'
} Telemetry_Modes_type;
#endif