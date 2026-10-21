#pragma once

#include "G4VUserPrimaryGeneratorAction.hh"

#include <HepMC3/ReaderAscii.h>
#include <fstream>
#include <memory>
#include <string>

class G4Event;
class G4ParticleGun;

class PrimaryGeneratorAction final : public G4VUserPrimaryGeneratorAction {
 public:
  PrimaryGeneratorAction(const std::string& input_path,
                         const std::string& vertices_path);
  ~PrimaryGeneratorAction() override;

  void GeneratePrimaries(G4Event* event) override;
  bool Failed() const { return failed_; }
  const std::string& FailureReason() const { return failure_reason_; }

 private:
  bool ReadVertex(int expected_event_id, double& x_mm, double& y_mm,
                  double& z_mm);

  std::unique_ptr<HepMC3::ReaderAscii> reader_;
  std::ifstream vertices_;
  std::unique_ptr<G4ParticleGun> gun_;
  bool failed_ = false;
  std::string failure_reason_;
};
