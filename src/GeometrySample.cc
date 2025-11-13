
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

#include "GeometrySample.hh"

#include "G4SystemOfUnits.hh"

#include <string>

GeometrySample::GeometrySample() :
    fSampleAssembly(NULL)
{
    // physical dimensions
    fRadius = 1.*cm;
    fZ = 5.*cm;

    // material
    fMaterialName = "G4_Pb";

    // colour
    fColour = G4Colour::Red();

    // copy number
    fCopyNo = 0;
}

GeometrySample::~GeometrySample()
{
}

G4int GeometrySample::Build()
{
    BuildMaterials();

    fSampleAssembly = new G4AssemblyVolume();

    G4ThreeVector move, direction;
    G4RotationMatrix* rotate = NULL;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* material = manager->FindOrBuildMaterial(fMaterialName);

    // solid
    G4Tubs* sSample = new G4Tubs("sSample", 0., fRadius, fZ/2., 0., 2.*M_PI);

    // logical
    fSampleLog = new G4LogicalVolume(sSample, // solid volume
            material,                         // material
            "SampleLog");                     // name

    // vis attributes                                              
    fSampleLog->SetVisAttributes(new G4VisAttributes(true, fColour));

    // add to assembly
    move = G4ThreeVector(0., 0., fZ/2.);
    rotate = new G4RotationMatrix();
    fSampleAssembly->AddPlacedVolume(fSampleLog, move, rotate);

    return 1;
}

G4int GeometrySample::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate) 
{
    G4bool surfCheck = true;
    fSampleAssembly->MakeImprint(logic_world, move, rotate, fCopyNo, surfCheck);

    // loop over PVs, find catcher log, set to member variable (for tracking what leaves the catcher in stepping action)
    auto pvIter = fSampleAssembly->GetVolumesIterator();
    for (size_t i = 0; i < fSampleAssembly->TotalImprintedVolumes(); i++) {
        G4VPhysicalVolume* physVol = *pvIter;
        if(physVol->GetName().find("SampleLog") != G4String::npos) {
            fSamplePhys = physVol;
        }
        pvIter++;
    }

    return fCopyNo++;
}

void GeometrySample::BuildMaterials() 
{
}

