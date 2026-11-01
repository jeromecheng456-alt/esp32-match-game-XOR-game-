#include "MatchGame.h"

MatchGame::MatchGame(uint32_t revealMs, uint32_t timeoutMs)
    : revealMs_(revealMs), timeoutMs_(timeoutMs) {}

void MatchGame::clearRound() {
  pickA_ = 0;
  pickB_ = 0;
}

MatchResult MatchGame::result() const {
  if (state_ != MatchState::REVEAL) return MatchResult::NONE;
  return pickA_ == pickB_ ? MatchResult::SAME : MatchResult::DIFFERENT;
}

void MatchGame::update(uint32_t now, const Inputs& in) {
  const bool aDown = in.a1 || in.a2;
  const bool bDown = in.b1 || in.b2;

  // A button held at power-up, after a timeout or across a reset is ignored
  // until it has been seen released once.
  if (!aDown) armedA_ = true;
  if (!bDown) armedB_ = true;

  switch (state_) {
    case MatchState::WAITING: {
      const bool noneBefore = (pickA_ == 0 && pickB_ == 0);
      if (pickA_ == 0 && armedA_ && aDown) pickA_ = in.a1 ? 1 : 2;  // both at once = option 1
      if (pickB_ == 0 && armedB_ && bDown) pickB_ = in.b1 ? 1 : 2;
      if (noneBefore && (pickA_ != 0 || pickB_ != 0)) firstLockMs_ = now;

      if (pickA_ != 0 && pickB_ != 0) {
        state_ = MatchState::REVEAL;
        revealStartMs_ = now;
      } else if ((pickA_ != 0 || pickB_ != 0) && (now - firstLockMs_) >= timeoutMs_) {
        clearRound();
        armedA_ = false;  // a still-held button must not instantly re-lock
        armedB_ = false;
      }
      break;
    }
    case MatchState::REVEAL:
      if ((now - revealStartMs_) >= revealMs_) state_ = MatchState::WAIT_RELEASE;
      break;
    case MatchState::WAIT_RELEASE:
      if (!aDown && !bDown) {
        clearRound();
        state_ = MatchState::WAITING;
      }
      break;
  }
}
