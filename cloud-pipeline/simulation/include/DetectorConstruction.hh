#pragma once

#include "G4VUserDetectorConstruction.hh"

#include <memory>

class CsvWriter;
class G4LogicalVolume;
class G4VPhysicalVolume;

class DetectorConstruction final : public G4VUserDetectorConstruction {
 public:
  explicit DetectorConstruction(std::shared_ptr<CsvWriter> writer);

  G4VPhysicalVolume* Construct() override;
  void ConstructSDandField() override;

 private:
  std::shared_ptr<CsvWriter> writer_;
  G4LogicalVolume* emcal_cell_ = nullptr;
  G4LogicalVolume* hcal_cell_ = nullptr;
};
