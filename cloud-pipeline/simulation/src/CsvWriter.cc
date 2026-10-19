#include "CsvWriter.hh"

#include <stdexcept>

CsvWriter::CsvWriter(const std::string& path) : output_(path) {
  if (!output_) {
    throw std::runtime_error("Cannot open hit output: " + path);
  }
  output_ << "event_id,detector,eta_bin,phi_bin,energy_mev\n";
}

CsvWriter::~CsvWriter() = default;

void CsvWriter::WriteHit(int event_id, const std::string& detector, int eta_bin,
                         int phi_bin, double energy_mev) {
  std::lock_guard<std::mutex> lock(mutex_);
  output_ << event_id << ',' << detector << ',' << eta_bin << ',' << phi_bin
          << ',' << energy_mev << '\n';
}
