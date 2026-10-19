#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

#include <HepMC3/ReaderAscii.h>
#include <memory>
#include <string>

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction final : public G4VUserPrimaryGeneratorAction {
 public:
  explicit PrimaryGeneratorAction(const std::string& input_path);
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;

 private:
  std::unique_ptr<HepMC3::ReaderAscii> reader_;
  std::unique_ptr<G4ParticleGun> gun_;
};
