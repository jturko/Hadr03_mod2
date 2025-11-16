
#ifndef GEOMETRYSHIELDING_HH
#define GEOMETRYSHIELDING_HH 1

//#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"

class G4AssemblyVolume;
class G4VPhysicalVolume;

class GeometryShielding
{
  public:
    GeometryShielding();
    ~GeometryShielding();

    G4int Build();
    G4int PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate);

    void SetInnerXY(G4double val)           { fInnerXY = val; };
    void SetInnerZ(G4double val)            { fInnerZ = val; };
    void SetBoratedPEThickness(G4double val){ fBoratedPEThickness = val; };
    void SetPbThickness(G4double val)       { fPbThickness = val; };

    G4VPhysicalVolume* GetTrackerPhys()     { return fTrackerPhys; };
        
    void BuildMaterials();

  private:
    G4AssemblyVolume* fShieldingAssembly;
    G4LogicalVolume* fBoratedPELog;
    G4LogicalVolume* fPbLog;
    G4LogicalVolume* fTrackerLog;
    G4VPhysicalVolume* fTrackerPhys;

    G4double fInnerXY;
    G4double fInnerZ;
    G4double fBoratedPEThickness;
    G4double fPbThickness;

    G4String fBoratedPEMaterialName;
    G4String fPbMaterialName;

    G4Colour fBoratedPEColour;
    G4Colour fPbColour;

    G4int fCopyNo;
};

#endif

