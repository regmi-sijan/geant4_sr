#include "ProgressEventAction.hh"

#include "G4Event.hh"
#include "G4ios.hh"

ProgressEventAction::ProgressEventAction(int total_events, int event_offset)
    : total_events_(total_events), event_offset_(event_offset) {}

void ProgressEventAction::BeginOfEventAction(const G4Event* event) {
  const int local_id = event->GetEventID();
  G4cout << "[Geant4] Starting event " << local_id + 1 << '/'
         << total_events_ << " (global event " << event_offset_ + local_id
         << ')' << G4endl;
}

void ProgressEventAction::EndOfEventAction(const G4Event* event) {
  const int local_id = event->GetEventID();
  G4cout << "[Geant4] Completed event " << local_id + 1 << '/'
         << total_events_ << " (global event " << event_offset_ + local_id
         << ')' << G4endl;
}
