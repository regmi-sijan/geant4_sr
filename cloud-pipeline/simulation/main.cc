#include "CsvWriter.hh"
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"

#include "FTFP_BERT.hh"
#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

namespace {
struct Options {
  std::string input;
  std::string output = "hits.csv";
  int events = 10;
  int event_offset = 0;
};

Options ParseOptions(int argc, char** argv) {
  Options options;
  for (int i = 1; i < argc; ++i) {
    const std::string arg = argv[i];
    auto value = [&](const std::string& flag) {
      if (++i >= argc) throw std::runtime_error("Missing value for " + flag);
      return std::string(argv[i]);
    };
    if (arg == "--input") options.input = value(arg);
    else if (arg == "--output") options.output = value(arg);
    else if (arg == "--events") options.events = std::stoi(value(arg));
    else if (arg == "--event-offset") options.event_offset = std::stoi(value(arg));
    else if (arg == "--help") {
      std::cout << "Usage: calo_sim --input events.hepmc [--output hits.csv] "
                   "[--events N] [--event-offset N]\n";
      std::exit(0);
    } else {
      throw std::runtime_error("Unknown option: " + arg);
    }
  }
  if (options.input.empty()) throw std::runtime_error("--input is required");
  if (options.events < 1) throw std::runtime_error("--events must be positive");
  if (options.event_offset < 0)
    throw std::runtime_error("--event-offset must be non-negative");
  return options;
}
}  // namespace

int main(int argc, char** argv) {
  try {
    const auto options = ParseOptions(argc, argv);
    auto writer =
        std::make_shared<CsvWriter>(options.output, options.event_offset);

    auto run_manager = std::make_unique<G4RunManager>();
    run_manager->SetUserInitialization(new DetectorConstruction(writer));
    run_manager->SetUserInitialization(new FTFP_BERT());
    run_manager->SetUserAction(new PrimaryGeneratorAction(options.input));
    run_manager->Initialize();
    run_manager->BeamOn(options.events);
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "calo_sim: " << error.what() << '\n';
    return 1;
  }
}
