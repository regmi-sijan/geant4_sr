#pragma once

#include "G4VSensitiveDetector.hh"

#include <map>
#include <memory>
#include <string>

class CsvWriter;
class G4HCofThisEvent;
class G4Step;
class G4TouchableHistory;

class CalorimeterSD final : public G4VSensitiveDetector {
 public:
  CalorimeterSD(const G4String& name, std::string detector,
                std::shared_ptr<CsvWriter> writer);

  void Initialize(G4HCofThisEvent*) override;
  G4bool ProcessHits(G4Step*, G4TouchableHistory*) override;
  void EndOfEvent(G4HCofThisEvent*) override;

 private:
  std::string detector_;
  std::shared_ptr<CsvWriter> writer_;
  std::map<std::pair<int, int>, double> energy_by_cell_;
};
