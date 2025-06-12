// test_telemetry_query.c
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <mock_csac_interface.h>
#include <telemetry_query.h>
#include "enums.h"
void test_full_telemetry_data() {
    mock_set_telemetry_response("5,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,2.34,123,1,259922,259824,1.09");

    T_CSAC_telemetry telemetry;
    int ret = get_telemetry_data(&telemetry);
    assert(ret == 0);
    assert(telemetry.status == 5);
    assert(telemetry.alarms == 0x0000);
    assert(strcmp(telemetry.SN, "2203CS77980") == 0);
    assert(telemetry.modes == 0x0001);
    assert(telemetry.Contrast == 3452);
    assert(fabs(telemetry.laserI - 1.05) < 1e-6);
    assert(fabs(telemetry.tcxo - 1.492) < 1e-6);
    assert(fabs(telemetry.heatP - 10.69) < 1e-6);
    assert(fabs(telemetry.sigV - 1.339) < 1e-6);
    assert(fabs(telemetry.temp - 27.54) < 1e-6);
    assert(telemetry.steer == 624);
    assert(fabs(telemetry.aTune - 2.34) < 1e-6);
    assert(telemetry.phase == 123);
    assert(telemetry.disOK == 1);
    assert(telemetry.TOD == 259922);
    assert(telemetry.LTime == 259824);
    assert(strcmp(telemetry.fwVer, "1.09") == 0);
    printf("✅ UC1: test_full_telemetry_data data: PASS\n");
}

void test_missing_optional_fields() {
    mock_set_telemetry_response("5,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,---,---,---,259922,259824,1.09");

    T_CSAC_telemetry telemetry;
    int ret = get_telemetry_data(&telemetry);
    assert(ret == 0);
    assert(telemetry.aTune == -1.0); // 未启用或不可用
    assert(telemetry.phase == INT16_MIN);
    assert(telemetry.disOK == -1);

    printf("✅ UC2: test_missing_optional_fields: PASS\n");
}

void test_phase_needrefpps() {
    mock_set_telemetry_response("5,0x0000,2203CS77980,0x0001,3452,1.05,1.492,10.69,1.339,27.54,624,1.23,NEEDREFPPS,---,259922,259824,1.09");

    T_CSAC_telemetry telemetry;
    int ret = get_telemetry_data(&telemetry);
    assert(ret == 0);
    assert(fabs(telemetry.aTune - 1.23) < 1e-6);
    assert(telemetry.phase == INT16_MIN);
    assert(telemetry.disOK == -1);

    printf("✅ UC3: test_phase_needrefpps: PASS\n");
}


void test_all_telemetry_tests() {
    printf("\n====================[ telemetry_query TEST START ]====================\n");
    test_full_telemetry_data();
    test_missing_optional_fields();
    test_phase_needrefpps();
    printf("====================[ telemetry_query TEST PASSED ]====================\n");
}