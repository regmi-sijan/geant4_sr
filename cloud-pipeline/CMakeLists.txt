cmake_minimum_required(VERSION 3.20)
project(PythiaGeant4Calorimeter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Geant4 REQUIRED)
find_package(HepMC3 REQUIRED)

add_executable(calo_sim
  simulation/main.cc
  simulation/src/CalorimeterSD.cc
  simulation/src/CsvWriter.cc
  simulation/src/DetectorConstruction.cc
  simulation/src/PrimaryGeneratorAction.cc
)

target_include_directories(calo_sim PRIVATE simulation/include)
target_link_libraries(calo_sim PRIVATE ${Geant4_LIBRARIES} HepMC3::HepMC3)

install(TARGETS calo_sim RUNTIME DESTINATION bin)
