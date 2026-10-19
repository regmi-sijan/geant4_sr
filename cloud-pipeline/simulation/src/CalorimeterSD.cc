#include "CalorimeterSD.hh"

#include "CsvWriter.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4TouchableHandle.hh"
#include "G4SystemOfUnits.hh"

#include <utility>

CalorimeterSD::CalorimeterSD(const G4String& name, std::string detector,
                             std::shared_ptr<CsvWriter> writer)
    : G4VSensitiveDetector(name),
      detector_(std::move(detector)),
      writer_(std::move(writer)) {}

void CalorimeterSD::Initialize(G4HCofThisEvent*) { energy_by_cell_.clear(); }

G4bool CalorimeterSD::ProcessHits(G4Step* step, G4TouchableHistory*) {
  const auto energy = step->GetTotalEnergyDeposit();
  if (energy <= 0.) return false;

  const auto touchable = step->GetPreStepPoint()->GetTouchableHandle();
  const int eta_bin = touchable->GetCopyNumber(0);
  const int phi_bin = touchable->GetCopyNumber(1);
  energy_by_cell_[{eta_bin, phi_bin}] += energy;
  return true;
}

void CalorimeterSD::EndOfEvent(G4HCofThisEvent*) {
  const auto* event = G4RunManager::GetRunManager()->GetCurrentEvent();
  const int event_id = event ? event->GetEventID() : -1;
  for (const auto& [cell, energy] : energy_by_cell_) {
    writer_->WriteHit(event_id, detector_, cell.first, cell.second,
                      energy / MeV);
  }
}
