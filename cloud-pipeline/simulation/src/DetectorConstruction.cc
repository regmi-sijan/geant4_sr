#include "DetectorConstruction.hh"

#include "CalorimeterSD.hh"
#include "CsvWriter.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PhysicalConstants.hh"
#include "G4PVPlacement.hh"
#include "G4PVReplica.hh"
#include "G4SDManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"

#include <memory>

namespace {
constexpr int kPhiBins = 64;
constexpr int kEtaBins = 24;

G4LogicalVolume* BuildSegmentedBarrel(const G4String& name, G4double r_min,
                                      G4double r_max, G4double half_length,
                                      G4Material* material,
                                      G4LogicalVolume* world,
                                      const G4Colour& colour,
                                      G4LogicalVolume*& cell) {
  auto* air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  auto* envelope_solid =
      new G4Tubs(name + "EnvelopeSolid", r_min, r_max, half_length, 0., CLHEP::twopi);
  auto* envelope =
      new G4LogicalVolume(envelope_solid, air, name + "EnvelopeLogical");
  new G4PVPlacement(nullptr, {}, envelope, name + "Envelope", world, false, 0,
                    true);

  const auto dphi = CLHEP::twopi / kPhiBins;
  auto* phi_solid =
      new G4Tubs(name + "PhiSolid", r_min, r_max, half_length, 0., dphi);
  auto* phi_logical =
      new G4LogicalVolume(phi_solid, air, name + "PhiLogical");
  new G4PVReplica(name + "PhiReplica", phi_logical, envelope, kPhi, kPhiBins,
                  dphi);

  auto* cell_solid = new G4Tubs(name + "CellSolid", r_min, r_max,
                                half_length / kEtaBins, 0., dphi);
  cell = new G4LogicalVolume(cell_solid, material, name + "CellLogical");
  new G4PVReplica(name + "EtaReplica", cell, phi_logical, kZAxis, kEtaBins,
                  2. * half_length / kEtaBins);

  auto* vis = new G4VisAttributes(colour);
  vis->SetForceSolid(true);
  cell->SetVisAttributes(vis);
  envelope->SetVisAttributes(G4VisAttributes::GetInvisible());
  phi_logical->SetVisAttributes(G4VisAttributes::GetInvisible());
  return envelope;
}
}  // namespace

DetectorConstruction::DetectorConstruction(std::shared_ptr<CsvWriter> writer)
    : writer_(std::move(writer)) {}

G4VPhysicalVolume* DetectorConstruction::Construct() {
  auto* nist = G4NistManager::Instance();
  auto* air = nist->FindOrBuildMaterial("G4_AIR");
  auto* aluminum = nist->FindOrBuildMaterial("G4_Al");
  auto* emcal = nist->FindOrBuildMaterial("G4_PbWO4");

  auto* iron = nist->FindOrBuildMaterial("G4_Fe");
  auto* scintillator = nist->FindOrBuildMaterial("G4_POLYSTYRENE");
  auto* hcal = new G4Material("HCalEffective", 3.2 * g / cm3, 2);
  hcal->AddMaterial(iron, 80. * perCent);
  hcal->AddMaterial(scintillator, 20. * perCent);

  auto* world_solid = new G4Box("WorldSolid", 4. * m, 4. * m, 4. * m);
  auto* world_logical =
      new G4LogicalVolume(world_solid, air, "WorldLogical");
  auto* world = new G4PVPlacement(nullptr, {}, world_logical, "World", nullptr,
                                  false, 0, true);
  world_logical->SetVisAttributes(G4VisAttributes::GetInvisible());

  auto* pipe_solid =
      new G4Tubs("BeamPipeSolid", 3.8 * cm, 4.0 * cm, 150. * cm, 0., CLHEP::twopi);
  auto* pipe_logical =
      new G4LogicalVolume(pipe_solid, aluminum, "BeamPipeLogical");
  new G4PVPlacement(nullptr, {}, pipe_logical, "BeamPipe", world_logical,
                    false, 0, true);

  BuildSegmentedBarrel("EMCal", 90. * cm, 110. * cm, 150. * cm, emcal,
                       world_logical, G4Colour(0.2, 0.6, 1.0), emcal_cell_);
  BuildSegmentedBarrel("HCal", 120. * cm, 170. * cm, 180. * cm, hcal,
                       world_logical, G4Colour(1.0, 0.5, 0.1), hcal_cell_);
  return world;
}

void DetectorConstruction::ConstructSDandField() {
  auto* manager = G4SDManager::GetSDMpointer();
  auto* emcal_sd = new CalorimeterSD("EMCalSD", "emcal", writer_);
  auto* hcal_sd = new CalorimeterSD("HCalSD", "hcal", writer_);
  manager->AddNewDetector(emcal_sd);
  manager->AddNewDetector(hcal_sd);
  SetSensitiveDetector(emcal_cell_, emcal_sd);
  SetSensitiveDetector(hcal_cell_, hcal_sd);
}
