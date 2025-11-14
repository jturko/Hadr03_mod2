
#ifndef GEOMETRYDETECTORPANEL_HH
#define GEOMETRYDETECTORPANEL_HH 1

//#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"

class G4AssemblyVolume;
class G4VPhysicalVolume;

class GeometryDetectorPanel
{
  public:
    GeometryDetectorPanel();
    ~GeometryDetectorPanel();

    G4int Build();
    G4int PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate);

    G4LogicalVolume* GetScintiLog() { return fScintiLog; };

    void SetXY(G4double val)        { fXY = val; };
    void SetZ(G4double val)         { fZ = val; };

    void BuildMaterials();

  private:
    G4AssemblyVolume* fDetectorPanelAssembly;
    G4LogicalVolume* fScintiLog;

    G4double fXY;
    G4double fZ;

    G4String fScintiMaterialName;

    G4Colour fScintiColour;

    G4int fCopyNo;
};

#endif

