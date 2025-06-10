// ruclock_macros.h
#ifndef RUCLOCK_MACROS_H
#define RUCLOCK_MACROS_H


#ifdef DEBUG
#define ERR_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define DEBUG_LOG(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
#define ERR_LOG(fmt, ...) do {} while (0)
#define DEBUG_LOG(fmt, ...) do {} while (0)
#endif
#define STRING_CONCAT(x, y) x ## y

#define	UART2           2
#define	TIMEOUT_SECONDS 5 // 超时时间，单位为秒
#define	CRLF			  "\n\n"
#define	MAX_REBUFF      135
#define	MAX_COMMAND_LENGTH  500
#define	MAX_RESPONSE_LENGTH 500
#define	COMMAND_LENGTH 20
#define	RESPONSE_LENGTH 20
#define	MAX_FIELDS 17

#define	TELEMETRY_HEAD          "!6\r\n"
#define	TELEMETRY_DATA          "!^\r\n"
#define	TELEMETRY_SET_FREQ_A    "FA"
#define	TELEMETRY_SET_FREQ_D    "FD"
#define	TELEMETRY_
#define	TELEMETRY_GET_FREQ      "!F?\r\n"
#define	TELEMETRY_LATCH_FREQ    "!FL\r\n"
#define	TELEMETRY_LATCH_DC      "!DCL\r\n"


#define TELEMETRY_SYNC          "!S\r\n"
#define TELEMETRY_HELP          "!?\r\n"

#define SYNC_1PPS								"!S\r\n"
#define QUERY_1PPS_TIME_CONST		"!D?\r\n"
#define QUERY_PHASE_THRESHOLD		"!m?\r\n"
#define QUERY_1PPS_CABLE_LEN		"!DC?\r\n"
#define LATCH_1PPS_CABLE_LEN		"!DCL\r\n"


#endif // RUCLOCK_MACROS_H
