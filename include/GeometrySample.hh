
#ifndef GEOMETRYSAMPLE_HH
#define GEOMETRYSAMPLE_HH 1

//#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"

class G4AssemblyVolume;
class G4VPhysicalVolume;

class GeometrySample
{
  public:
    GeometrySample();
    ~GeometrySample();

    G4int Build();
    G4int PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate);

    G4LogicalVolume* GetSampleLog() { return fSampleLog; };

    void SetSamplePhys(G4VPhysicalVolume* vol) { fSamplePhys = vol; };
    G4VPhysicalVolume* GetSamplePhys() { return fSamplePhys; };

    void SetMaterialName(G4String name) { fMaterialName = name; };
    void SetRadius(G4double rad)        { fRadius = rad; };
    void SetZ(G4double z)               { fZ = z; };

    void BuildMaterials();

  private:
    G4AssemblyVolume* fSampleAssembly;
    G4LogicalVolume* fSampleLog;
    G4VPhysicalVolume* fSamplePhys;

    G4double fRadius;
    G4double fZ;

    G4String fMaterialName;
    G4Colour fColour;

    G4int fCopyNo;
};

#endif

