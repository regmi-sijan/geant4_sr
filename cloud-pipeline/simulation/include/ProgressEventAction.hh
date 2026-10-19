#pragma once

#include "G4UserEventAction.hh"

class G4Event;

class ProgressEventAction final : public G4UserEventAction {
 public:
  ProgressEventAction(int total_events, int event_offset);

  void BeginOfEventAction(const G4Event* event) override;
  void EndOfEventAction(const G4Event* event) override;

 private:
  int total_events_;
  int event_offset_;
};
