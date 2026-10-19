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

#include <cmath>
#include <stdexcept>

PrimaryGeneratorAction::PrimaryGeneratorAction(const std::string& input_path)
    : reader_(std::make_unique<HepMC3::ReaderAscii>(input_path)),
      gun_(std::make_unique<G4ParticleGun>(1)) {
  if (reader_->failed()) {
    throw std::runtime_error("Cannot open HepMC3 input: " + input_path);
  }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() = default;

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event) {
  HepMC3::GenEvent input;
  reader_->read_event(input);
  if (reader_->failed()) {
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

    G4ThreeVector position;
    if (const auto vertex = particle->production_vertex()) {
      const auto& x = vertex->position();
      position = {x.x() * mm, x.y() * mm, x.z() * mm};
    }

    gun_->SetParticleDefinition(definition);
    gun_->SetParticlePosition(position);
    gun_->SetParticleMomentumDirection({p.px(), p.py(), p.pz()});
    gun_->SetParticleMomentum(magnitude * GeV);
    gun_->GeneratePrimaryVertex(event);
  }
}
