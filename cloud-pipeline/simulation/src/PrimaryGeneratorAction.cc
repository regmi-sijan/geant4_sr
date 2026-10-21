#include "PrimaryGeneratorAction.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"
#include <HepMC3/GenEvent.h>
#include <HepMC3/GenParticle.h>
#include <HepMC3/GenVertex.h>

#include <algorithm>
#include <cmath>
#include <sstream>
#include <stdexcept>

PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& input_path,
                                               const std::string& vertices_path)
    : reader_(std::make_unique<HepMC3::ReaderAscii>(input_path)),
      vertices_(vertices_path),
      gun_(std::make_unique<G4ParticleGun>(1)) {
  if (reader_->failed()) {
    throw std::runtime_error("Cannot open HepMC3 input: " + input_path);
  }
  if (!vertices_) {
    throw std::runtime_error("Cannot open vertex input: " + vertices_path);
  }
  std::string header;
  if (!std::getline(vertices_, header) ||
      header != "event_id,vertex_x_mm,vertex_y_mm,vertex_z_mm") {
    throw std::runtime_error("Invalid vertex CSV header: " + vertices_path);
  }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

bool PrimaryGeneratorAction::ReadVertex(int expected_event_id, double& x_mm,
                                        double& y_mm, double& z_mm) {
  std::string line;
  if (!std::getline(vertices_, line)) return false;
  std::replace(line.begin(), line.end(), ',', ' ');
  std::istringstream values(line);
  int event_id = -1;
  if (!(values >> event_id >> x_mm >> y_mm >> z_mm)) return false;
  return event_id == expected_event_id;
}

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  HepMC3::GenEvent input;
  reader_->read_event(input);
  if (reader_->failed()) {
    failed_ = true;
    failure_reason_ = "HepMC input ended or failed before all requested events";
    G4RunManager::GetRunManager()->AbortRun(true);
    return;
  }

  double vertex_x_mm = 0.;
  double vertex_y_mm = 0.;
  double vertex_z_mm = 0.;
  if (!ReadVertex(input.event_number(), vertex_x_mm, vertex_y_mm, vertex_z_mm)) {
    failed_ = true;
    failure_reason_ = "Vertex CSV is missing or mismatched for event " +
                      std::to_string(input.event_number());
    G4RunManager::GetRunManager()->AbortRun(true);
    return;
  }

  auto* table = G4ParticleTable::GetParticleTable();
  for (const auto& particle : input.particles()) {
    if (particle->status() != 1) continue;
    auto* definition = table->FindParticle(particle->pid());
    if (!definition) continue;

    const auto& p = particle->momentum();
    const auto magnitude = std::sqrt(p.px() * p.px() + p.py() * p.py() +
                                     p.pz() * p.pz());
    if (magnitude <= 0.) continue;

    gun_->SetParticleDefinition(definition);
    gun_->SetParticlePosition(
        {vertex_x_mm * mm, vertex_y_mm * mm, vertex_z_mm * mm});
    gun_->SetParticleMomentumDirection({p.px(), p.py(), p.pz()});
    gun_->SetParticleMomentum(magnitude * GeV);
    gun_->GeneratePrimaryVertex(event);
  }
}
