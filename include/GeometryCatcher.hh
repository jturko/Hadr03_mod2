
#ifndef GEOMETRYCATCHER_HH
#define GEOMETRYCATCHER_HH 1

//#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"

class G4AssemblyVolume;
class G4VPhysicalVolume;

class GeometryCatcher
{
  public:
    GeometryCatcher();
    ~GeometryCatcher();

    G4int Build();
    G4int PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate);

    void SetMaterialName(G4String name) { fMaterialName = name; };
    void SetRadius(G4double rad)        { fRadius = rad; };
    void SetZ(G4double z)               { fZ = z; };

    void BuildMaterials();

  private:
    G4AssemblyVolume* fCatcherAssembly;
    G4LogicalVolume* fCatcherLog;

    G4double fRadius;
    G4double fZ;

    G4String fMaterialName;
    G4Colour fColour;

    G4int fCopyNo;
};

#endif

