#pragma once

#include <fstream>
#include <mutex>
#include <string>

class CsvWriter {
 public:
  explicit CsvWriter(const std::string& path, int event_offset = 0);
  ~CsvWriter();

  void WriteHit(int event_id, const std::string& detector, int eta_bin,
                int phi_bin, double energy_mev);

 private:
  std::ofstream output_;
  std::mutex mutex_;
  int event_offset_;
};
