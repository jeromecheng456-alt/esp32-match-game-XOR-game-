#pragma once
#include <stdint.h>

// Pure game logic, no Arduino calls, so it runs in native unit tests.
// Rule (decided 2026-11-01): same pick = SAME (green), different = DIFFERENT (red).

enum class MatchResult : uint8_t { NONE, SAME, DIFFERENT };
enum class MatchState : uint8_t { WAITING, REVEAL, WAIT_RELEASE };

struct Inputs {
  bool a1, a2;  // player A option 1 / option 2 pressed
  bool b1, b2;  // player B option 1 / option 2 pressed
};

class MatchGame {
 public:
  MatchGame(uint32_t revealMs = 5000, uint32_t timeoutMs = 30000);

  // Call every loop with the current time and button states.
  void update(uint32_t nowMs, const Inputs& in);

  MatchResult result() const;   // NONE except during REVEAL
  MatchState state() const { return state_; }
  uint8_t pickA() const { return pickA_; }  // 0 = none, 1, 2
  uint8_t pickB() const { return pickB_; }

 private:
  void clearRound();

  uint32_t revealMs_, timeoutMs_;
  MatchState state_ = MatchState::WAITING;
  uint8_t pickA_ = 0, pickB_ = 0;
  bool armedA_ = false, armedB_ = false;  // false until player's buttons seen released
  uint32_t firstLockMs_ = 0, revealStartMs_ = 0;
};
