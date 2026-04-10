#ifndef GEOMETRYCASTOR440_HH
#define GEOMETRYCASTOR440_HH 1

#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"
#include <vector>

class G4AssemblyVolume;
class G4VPhysicalVolume;
class G4LogicalVolume;

class GeometryCASTOR440
{
  public:
    GeometryCASTOR440();
    ~GeometryCASTOR440();

    G4int Build();
    void PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate, G4int copyNo);

    G4LogicalVolume* GetCASTORLog() { return fCASTORBodyLog; }
    
    G4ThreeVector GetFuelPosition(G4int index) const;

  private:
    void BuildMaterials();

    G4AssemblyVolume* fCASTORAssembly;

    G4LogicalVolume* fCASTORBodyLog;
    G4LogicalVolume* fCavityLog;
    G4LogicalVolume* fLidLog;
    G4LogicalVolume* fFuelAssemblyLog;
    G4LogicalVolume* fModeratorRodLog;
    G4LogicalVolume* fFinLog;

    G4double fCaskHeight;
    G4double fCaskOuterRadius;
    G4double fCaskInnerRadius;
    G4double fCavityHeight;
    G4double fLidThickness;

    G4String fCastIronMatName;
    G4String fHeliumMatName;
    G4String fPEMatName;
    G4String fFuelMatName;
    G4String fSteelMatName;

    std::vector<G4ThreeVector> fFuelPositions;
};

#endif
