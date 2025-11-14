
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4SubtractionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "GeometryDetectorPanel.hh"

#include "G4SystemOfUnits.hh"

#include <string>

GeometryDetectorPanel::GeometryDetectorPanel() :
    fDetectorPanelAssembly(NULL)
{
    // physical dimensions
    fXY = 8.*cm;
    fZ = 2.*cm;

    // materials
    fScintiMaterialName = "G4_PLASTIC_SC_VINYLTOLUENE";

    // colours
    fScintiColour = G4Colour::Cyan();

    // copy number
    fCopyNo = 0;
}

GeometryDetectorPanel::~GeometryDetectorPanel()
{
}

G4int GeometryDetectorPanel::Build()
{
    BuildMaterials();

    fDetectorPanelAssembly = new G4AssemblyVolume();
    
    G4ThreeVector move, direction;
    G4RotationMatrix* rotate = NULL;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* material;

    // material
    material = manager->FindOrBuildMaterial(fScintiMaterialName);
    // solid
    G4Box* sScinti = new G4Box("sScinti",    fXY/2.,         fXY/2.,         fZ/2.);
    // logical
    fScintiLog = new G4LogicalVolume(sScinti, // solid volume
            material,                          // material
            "ScintiLog");                  // name
    // vis attributes                                              
    fScintiLog->SetVisAttributes(new G4VisAttributes(true, fScintiColour));
    // add to assembly
    move = G4ThreeVector(0., 0., fZ/2.);
    rotate = new G4RotationMatrix();
    fDetectorPanelAssembly->AddPlacedVolume(fScintiLog, move, rotate);

    return 1;
}

G4int GeometryDetectorPanel::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate) 
{
    G4bool surfCheck = true;
    fDetectorPanelAssembly->MakeImprint(logic_world, move, rotate, fCopyNo, surfCheck);
    return fCopyNo++;
}

void GeometryDetectorPanel::BuildMaterials() 
{
    G4double z, a, density;
    G4int ncomp, natoms;
}

