
#ifndef GEOMETRYCOLLIMATOR_HH
#define GEOMETRYCOLLIMATOR_HH 1

//#include "G4SystemOfUnits.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4Colour.hh"

class G4AssemblyVolume;
class G4VPhysicalVolume;

class GeometryCollimator
{
  public:
    GeometryCollimator();
    ~GeometryCollimator();

    G4int Build();
    G4int PlaceDetector(G4LogicalVolume* expHallLog, G4ThreeVector move, G4RotationMatrix* rotate);

    void SetXY(G4double val)        { fXY = val; };
    void SetZ(G4double val)         { fZ = val; };
    void SetInnerXY(G4double val)   { fInnerXY = val; };
    void SetPbZ(G4double val)       { fPbZ = val; };

    void BuildMaterials();

  private:
    G4AssemblyVolume* fCollimatorAssembly;
    G4LogicalVolume* fBoratedPELog;
    G4LogicalVolume* fPbLog;

    G4double fXY;
    G4double fZ;
    G4double fInnerXY;
    G4double fPbZ;

    G4String fBoratedPEMaterialName;
    G4String fPbMaterialName;

    G4Colour fBoratedPEColour;
    G4Colour fPbColour;

    G4int fCopyNo;
};

#endif

