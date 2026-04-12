#ifndef GeometryParallelBiasing_h
#define GeometryParallelBiasing_h 1

#include "G4VUserParallelWorld.hh"
#include "globals.hh"

class DetectorConstruction; // Need this to access dynamic CASTOR placements

class GeometryParallelBiasing : public G4VUserParallelWorld {
  public:
    // Constructor takes the world name (required by Geant4) and a pointer 
    // to the mass geometry so we can align our ghost shells perfectly.
    GeometryParallelBiasing(G4String worldName, DetectorConstruction* det);
    ~GeometryParallelBiasing() override = default;

  protected:
    // This is called automatically by Geant4 during geometry initialization
    void Construct() override;

  private:
    // Store the pointer to access the CASTOR positions/rotations
    DetectorConstruction* fDetector;
};

#endif
