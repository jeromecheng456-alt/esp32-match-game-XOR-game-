#include <unity.h>
#include "MatchGame.h"

static const Inputs NONE_{false, false, false, false};
static Inputs in(bool a1, bool a2, bool b1, bool b2) { return Inputs{a1, a2, b1, b2}; }

// New game with all buttons released so both players are armed.
static MatchGame fresh() {
  MatchGame g(5000, 30000);
  g.update(0, NONE_);
  return g;
}

void setUp() {}
void tearDown() {}

void test_same_option1() {
  MatchGame g = fresh();
  g.update(10, in(true, false, false, false));
  g.update(20, in(false, false, true, false));
  TEST_ASSERT_EQUAL(MatchResult::SAME, g.result());
}
void test_same_option2() {
  MatchGame g = fresh();
  g.update(10, in(false, true, false, false));
  g.update(20, in(false, false, false, true));
  TEST_ASSERT_EQUAL(MatchResult::SAME, g.result());
}
void test_different_1_2() {
  MatchGame g = fresh();
  g.update(10, in(true, false, false, false));
  g.update(20, in(false, false, false, true));
  TEST_ASSERT_EQUAL(MatchResult::DIFFERENT, g.result());
}
void test_different_2_1() {
  MatchGame g = fresh();
  g.update(10, in(false, true, false, false));
  g.update(20, in(false, false, true, false));
  TEST_ASSERT_EQUAL(MatchResult::DIFFERENT, g.result());
}
void test_no_result_until_both_locked() {
  MatchGame g = fresh();
  g.update(10, in(true, false, false, false));
  TEST_ASSERT_EQUAL(MatchResult::NONE, g.result());
  TEST_ASSERT_EQUAL_UINT8(1, g.pickA());
  TEST_ASSERT_EQUAL_UINT8(0, g.pickB());
}
void test_second_press_ignored() {
  MatchGame g = fresh();
  g.update(10, in(true, false, false, false));   // A locks option 1
  g.update(20, NONE_);
  g.update(30, in(false, true, false, false));   // A tries option 2
  TEST_ASSERT_EQUAL_UINT8(1, g.pickA());
}
void test_both_buttons_at_once_is_option1() {
  MatchGame g = fresh();
  g.update(10, in(true, true, false, false));
  TEST_ASSERT_EQUAL_UINT8(1, g.pickA());
}
void test_held_at_power_up_ignored() {
  MatchGame g(5000, 30000);
  g.update(0, in(true, false, false, false));    // A held at boot
  g.update(10, in(true, false, false, false));
  TEST_ASSERT_EQUAL_UINT8(0, g.pickA());
  g.update(20, NONE_);                           // released -> armed
  g.update(30, in(false, true, false, false));
  TEST_ASSERT_EQUAL_UINT8(2, g.pickA());
}
void test_reveal_lasts_5s_then_waits_for_release() {
  MatchGame g = fresh();
  g.update(100, in(true, false, false, false));
  g.update(200, in(true, false, true, false));   // both locked at t=200
  TEST_ASSERT_EQUAL(MatchResult::SAME, g.result());
  g.update(5199, in(true, false, true, false));
  TEST_ASSERT_EQUAL(MatchResult::SAME, g.result());      // still showing
  g.update(5200, in(true, false, true, false));
  TEST_ASSERT_EQUAL(MatchResult::NONE, g.result());      // reveal over
  TEST_ASSERT_EQUAL(MatchState::WAIT_RELEASE, g.state());
  g.update(5300, in(true, false, true, false));          // still held
  TEST_ASSERT_EQUAL(MatchState::WAIT_RELEASE, g.state());
  g.update(5400, NONE_);                                 // released
  TEST_ASSERT_EQUAL(MatchState::WAITING, g.state());
  TEST_ASSERT_EQUAL_UINT8(0, g.pickA());
  TEST_ASSERT_EQUAL_UINT8(0, g.pickB());
}
void test_timeout_clears_half_round() {
  MatchGame g = fresh();
  g.update(1000, in(true, false, false, false));
  g.update(1001, NONE_);
  g.update(30999, NONE_);
  TEST_ASSERT_EQUAL_UINT8(1, g.pickA());                 // 29.999 s, not yet
  g.update(31000, NONE_);                                // 30 s after first lock
  TEST_ASSERT_EQUAL_UINT8(0, g.pickA());
}
void test_held_button_after_timeout_not_relocked() {
  MatchGame g = fresh();
  g.update(1000, in(true, false, false, false));
  g.update(31000, in(true, false, false, false));        // timeout, A still held
  g.update(31010, in(true, false, false, false));
  TEST_ASSERT_EQUAL_UINT8(0, g.pickA());
}
void test_next_round_works() {
  MatchGame g = fresh();
  g.update(10, in(true, false, false, false));
  g.update(20, in(false, false, false, true));
  g.update(6000, NONE_);                                 // reveal over
  g.update(6010, NONE_);                                 // released -> WAITING
  g.update(6020, in(false, true, false, false));
  g.update(6030, in(false, false, false, true));
  TEST_ASSERT_EQUAL(MatchResult::SAME, g.result());
}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_same_option1);
  RUN_TEST(test_same_option2);
  RUN_TEST(test_different_1_2);
  RUN_TEST(test_different_2_1);
  RUN_TEST(test_no_result_until_both_locked);
  RUN_TEST(test_second_press_ignored);
  RUN_TEST(test_both_buttons_at_once_is_option1);
  RUN_TEST(test_held_at_power_up_ignored);
  RUN_TEST(test_reveal_lasts_5s_then_waits_for_release);
  RUN_TEST(test_timeout_clears_half_round);
  RUN_TEST(test_held_button_after_timeout_not_relocked);
  RUN_TEST(test_next_round_works);
  return UNITY_END();
}
