#include <unity.h>
#include "control_logic/Hysteresis.h"
#include "control_logic/WarmupTimer.h"
#include "control_logic/GasAlarm.h"
#include "control_logic/ClimateController.h"

using namespace control;

void setUp(void) {}
void tearDown(void) {}

// ---- HysteresisChannel ----

void test_hysteresis_activate_above_turns_on_past_upper_threshold() {
    HysteresisChannel ch(HysteresisChannel::Mode::ActivateAbove, 26.0f, 24.0f);
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::Off), static_cast<int>(ch.update(25.0f)));
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::On), static_cast<int>(ch.update(26.5f)));
}

void test_hysteresis_activate_above_does_not_chatter_between_thresholds() {
    HysteresisChannel ch(HysteresisChannel::Mode::ActivateAbove, 26.0f, 24.0f);
    ch.update(27.0f); // turns on
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::On), static_cast<int>(ch.update(25.0f)));
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::Off), static_cast<int>(ch.update(23.5f)));
}

void test_hysteresis_activate_below_turns_on_under_lower_threshold() {
    HysteresisChannel ch(HysteresisChannel::Mode::ActivateBelow, 18.0f, 20.0f);
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::Off), static_cast<int>(ch.update(19.0f)));
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::On), static_cast<int>(ch.update(17.0f)));
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::On), static_cast<int>(ch.update(19.0f)));
    TEST_ASSERT_EQUAL(static_cast<int>(RelayState::Off), static_cast<int>(ch.update(20.5f)));
}

// ---- WarmupTimer ----

void test_warmup_not_complete_before_120s() {
    WarmupTimer t;
    t.start(0);
    TEST_ASSERT_FALSE(t.isComplete(119999));
}

void test_warmup_complete_at_120s() {
    WarmupTimer t;
    t.start(1000);
    TEST_ASSERT_TRUE(t.isComplete(1000 + 120000));
}

void test_warmup_not_started_is_never_complete() {
    WarmupTimer t;
    TEST_ASSERT_FALSE(t.isComplete(1000000));
}

// ---- GasAlarm ----

void test_gas_alarm_triggers_above_threshold() {
    GasAlarm alarm(200.0f, 150.0f, 30000);
    TEST_ASSERT_FALSE(alarm.update(50.0f, 0));
    TEST_ASSERT_TRUE(alarm.update(250.0f, 1000));
}

void test_gas_alarm_requires_30s_safe_hold_before_clearing() {
    GasAlarm alarm(200.0f, 150.0f, 30000);
    alarm.update(250.0f, 0);          // trigger
    TEST_ASSERT_TRUE(alarm.update(100.0f, 10000));   // safe but not held long enough
    TEST_ASSERT_TRUE(alarm.update(100.0f, 29000));   // still under 30s of continuous safe reading
    TEST_ASSERT_FALSE(alarm.update(100.0f, 40001));  // 30s+ since reading first went safe at t=10000
}

void test_gas_alarm_resets_safe_hold_if_it_spikes_again() {
    GasAlarm alarm(200.0f, 150.0f, 30000);
    alarm.update(250.0f, 0);
    alarm.update(100.0f, 1000);   // starts the safe-hold clock
    alarm.update(250.0f, 5000);   // spikes again, resets the clock
    TEST_ASSERT_TRUE(alarm.update(100.0f, 6000 + 29000)); // not 30s since the t=5000 spike yet
}

// ---- ClimateController ----

void test_climate_controller_gas_alarm_forces_ventilation_and_blocks_heating() {
    ClimateThresholds thr;
    ClimateController ctrl(thr);
    // Cold room (would normally call for heating) but gas spikes -> ventilation forced on, heating blocked.
    ClimateOutputs out = ctrl.update(/*tempC=*/10.0f, /*humidityPct=*/50.0f, /*gasPpm=*/300.0f, /*nowMs=*/0);
    TEST_ASSERT_TRUE(out.gasAlarm);
    TEST_ASSERT_TRUE(out.ventilation);
    TEST_ASSERT_FALSE(out.heating);
}

void test_climate_controller_normal_operation_independent_channels() {
    ClimateThresholds thr;
    ClimateController ctrl(thr);
    ClimateOutputs out = ctrl.update(/*tempC=*/27.0f, /*humidityPct=*/30.0f, /*gasPpm=*/0.0f, /*nowMs=*/0);
    TEST_ASSERT_FALSE(out.gasAlarm);
    TEST_ASSERT_TRUE(out.ventilation);   // above tempMaxOn
    TEST_ASSERT_TRUE(out.humidifier);    // below humMinOn
    TEST_ASSERT_FALSE(out.heating);
}

int main(int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_hysteresis_activate_above_turns_on_past_upper_threshold);
    RUN_TEST(test_hysteresis_activate_above_does_not_chatter_between_thresholds);
    RUN_TEST(test_hysteresis_activate_below_turns_on_under_lower_threshold);
    RUN_TEST(test_warmup_not_complete_before_120s);
    RUN_TEST(test_warmup_complete_at_120s);
    RUN_TEST(test_warmup_not_started_is_never_complete);
    RUN_TEST(test_gas_alarm_triggers_above_threshold);
    RUN_TEST(test_gas_alarm_requires_30s_safe_hold_before_clearing);
    RUN_TEST(test_gas_alarm_resets_safe_hold_if_it_spikes_again);
    RUN_TEST(test_climate_controller_gas_alarm_forces_ventilation_and_blocks_heating);
    RUN_TEST(test_climate_controller_normal_operation_independent_channels);
    return UNITY_END();
}
