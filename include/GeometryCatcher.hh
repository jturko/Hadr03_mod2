
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

    G4LogicalVolume* GetCatcherLog() { return fCatcherLog; };

    void SetCatcherPhys(G4VPhysicalVolume* vol) { fCatcherPhys = vol; };
    G4VPhysicalVolume* GetCatcherPhys() { return fCatcherPhys; };

    void SetMaterialName(G4String name) { fMaterialName = name; };
    void SetRadius(G4double rad)        { fRadius = rad; };
    void SetZ(G4double z)               { fZ = z; };

    void BuildMaterials();

  private:
    G4AssemblyVolume* fCatcherAssembly;
    G4LogicalVolume* fCatcherLog;
    G4VPhysicalVolume* fCatcherPhys;

    G4double fRadius;
    G4double fZ;

    G4String fMaterialName;
    G4Colour fColour;

    G4int fCopyNo;
};

#endif

