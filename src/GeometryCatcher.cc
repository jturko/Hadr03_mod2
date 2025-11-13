
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

#include "GeometryCatcher.hh"

#include "G4SystemOfUnits.hh"

#include <string>

GeometryCatcher::GeometryCatcher() :
    fCatcherAssembly(NULL)
{
    // physical dimensions
    fRadius = 5.*cm;
    fZ = 5.*cm;

    // material
    fMaterialName = "G4_Be";

    // colour
    fColour = G4Colour::Green();

    // copy number
    fCopyNo = 0;
}

GeometryCatcher::~GeometryCatcher()
{
}

G4int GeometryCatcher::Build()
{
    BuildMaterials();

    fCatcherAssembly = new G4AssemblyVolume();
    
    G4ThreeVector move, direction;
    G4RotationMatrix* rotate = NULL;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* material = manager->FindOrBuildMaterial(fMaterialName);

    // solid
    G4Tubs* sCatcher = new G4Tubs("sCatcher", 0., fRadius, fZ/2., 0., 2.*M_PI);

    // logical
    fCatcherLog = new G4LogicalVolume(sCatcher, // solid volume
            material,                          // material
            "CatcherLog");                  // name
                                               
    // vis attributes                                              
    fCatcherLog->SetVisAttributes(new G4VisAttributes(true, fColour));

    // add to assembly
    move = G4ThreeVector(0., 0., 0.);
    rotate = new G4RotationMatrix();
    fCatcherAssembly->AddPlacedVolume(fCatcherLog, move, rotate);

    return 1;
}

G4int GeometryCatcher::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate) 
{
    G4bool surfCheck = true;
    fCatcherAssembly->MakeImprint(logic_world, move, rotate, fCopyNo, surfCheck);
    return fCopyNo++;
}

void GeometryCatcher::BuildMaterials() 
{
    G4NistManager* manager = G4NistManager::Instance();
    G4Material* LiF = new G4Material("LiF", 2.635*g/cm3, 2);
    LiF->AddElement(manager->FindOrBuildElement("Li"), 1);
    LiF->AddElement(manager->FindOrBuildElement("F"), 1);
}

