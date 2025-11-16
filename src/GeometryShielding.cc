
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

#include "GeometryShielding.hh"

#include "G4SystemOfUnits.hh"

#include <string>

GeometryShielding::GeometryShielding() :
    fShieldingAssembly(NULL)
{
    // physical dimensions
    fInnerXY = 2.*m;
    fInnerZ = 4.*cm;
    fBoratedPEThickness = 10.*cm;
    fPbThickness = 2.*cm;

    // materials
    fBoratedPEMaterialName = "BoratedPE";
    fPbMaterialName = "G4_Pb";

    // colours
    fBoratedPEColour = G4Colour::White();
    fPbColour = G4Colour::Brown();

    // copy number
    fCopyNo = 0;
}

GeometryShielding::~GeometryShielding()
{
}

G4int GeometryShielding::Build()
{
    BuildMaterials();

    fShieldingAssembly = new G4AssemblyVolume();
    
    G4ThreeVector move, direction;
    G4RotationMatrix* rotate = NULL;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* material;

    // extra parameters 
    G4double cut_extra = 1.*cm;

    // borated PE
    // material
    material = manager->FindOrBuildMaterial(fBoratedPEMaterialName);
    // solid
    G4Box* sPE_PreCut = new G4Box("sPEPrecut", 
            fInnerXY/2. + fBoratedPEThickness, 
            fInnerXY/2. + fBoratedPEThickness, 
            fInnerZ/2. + fBoratedPEThickness);
    G4Box* sPE_Cut = new G4Box("sPECut",          
            fInnerXY/2.,    
            fInnerXY/2.,    
            fInnerZ/2.);
    G4SubtractionSolid* sPE = new G4SubtractionSolid("sPE", sPE_PreCut, sPE_Cut);
    // logical
    fBoratedPELog = new G4LogicalVolume(sPE, // solid volume
            material,                          // material
            "BoratedPELog");                  // name
    // vis attributes                                              
    fBoratedPELog->SetVisAttributes(new G4VisAttributes(true, fBoratedPEColour));
    // add to assembly
    move = G4ThreeVector(0., 0., fInnerZ/2. + fBoratedPEThickness + fPbThickness);
    rotate = new G4RotationMatrix();
    fShieldingAssembly->AddPlacedVolume(fBoratedPELog, move, rotate);
    
    // Pb
    // material
    material = manager->FindOrBuildMaterial(fPbMaterialName);
    // solid
    G4Box* sPb_PreCut = new G4Box("sPbPrecut", 
            fInnerXY/2. + fBoratedPEThickness + fPbThickness, 
            fInnerXY/2. + fBoratedPEThickness + fPbThickness, 
            fInnerZ/2. + fBoratedPEThickness + fPbThickness);
    G4Box* sPb_Cut = new G4Box("sPbCut",          
            fInnerXY/2. + fBoratedPEThickness,    
            fInnerXY/2. + fBoratedPEThickness,    
            fInnerZ/2. + fBoratedPEThickness);
    G4SubtractionSolid* sPb = new G4SubtractionSolid("sPb", sPb_PreCut, sPb_Cut);
    // logical
    fPbLog = new G4LogicalVolume(sPb, // solid volume
            material,                          // material
            "PbLog");                  // name
    // vis attributes                                              
    fPbLog->SetVisAttributes(new G4VisAttributes(true, fPbColour));
    // add to assembly
    move = G4ThreeVector(0., 0., fInnerZ/2. + fBoratedPEThickness + fPbThickness);
    rotate = new G4RotationMatrix();
    fShieldingAssembly->AddPlacedVolume(fPbLog, move, rotate);
    
    // tracker
    // material
    material = manager->FindOrBuildMaterial("G4_AIR");
    // solid
    G4Box* sTracker_PreCut = new G4Box("sPbPrecut", 
            fInnerXY/2. + fBoratedPEThickness + fPbThickness + cut_extra, 
            fInnerXY/2. + fBoratedPEThickness + fPbThickness + cut_extra, 
            fInnerZ/2. + fBoratedPEThickness + fPbThickness + cut_extra);
    G4Box* sTracker_Cut = new G4Box("sTrackerCut",          
            fInnerXY/2. + fBoratedPEThickness + fPbThickness,    
            fInnerXY/2. + fBoratedPEThickness + fPbThickness,    
            fInnerZ/2. + fBoratedPEThickness + fPbThickness);
    G4SubtractionSolid* sTracker = new G4SubtractionSolid("sTracker", sTracker_PreCut, sTracker_Cut);
    // logical
    fTrackerLog = new G4LogicalVolume(sTracker, // solid volume
            material,                          // material
            "TrackerLog");                  // name
    // vis attributes                                              
    fTrackerLog->SetVisAttributes(G4VisAttributes::GetInvisible());
    // add to assembly
    move = G4ThreeVector(0., 0., fInnerZ/2. + fBoratedPEThickness + fPbThickness);
    rotate = new G4RotationMatrix();
    fShieldingAssembly->AddPlacedVolume(fTrackerLog, move, rotate);

    return 1;
}

G4int GeometryShielding::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate) 
{
    G4bool surfCheck = true;
    fShieldingAssembly->MakeImprint(logic_world, move, rotate, fCopyNo, surfCheck);
    
    // loop over PVs, find shielding tracker log, set to member variable (for tracking what leaves the shielding tracker in stepping action)
    auto pvIter = fShieldingAssembly->GetVolumesIterator();
    for (size_t i = 0; i < fShieldingAssembly->TotalImprintedVolumes(); i++) {
        G4VPhysicalVolume* physVol = *pvIter;
        if(physVol->GetName().find("TrackerLog") != G4String::npos) {
            fTrackerPhys = physVol;
        }
        pvIter++;
    }

    return fCopyNo++;
}

void GeometryShielding::BuildMaterials() 
{
    G4double z, a, density;
    G4int ncomp, natoms;

    // borated PE
    // check if boratedPE already exists
    G4NistManager* manager = G4NistManager::Instance();
    if(manager->FindOrBuildMaterial(fBoratedPEMaterialName)) {
        G4cout << " ---> Found " << fBoratedPEMaterialName << ", not re-building..." << G4endl;
        return;
    }
    // elements
    G4Element* H  = new G4Element("Hydrogen", "H", z=1., a=1.0079*g/mole);
    G4Element* C  = new G4Element("Carbon",   "C", z=6., a=12.01*g/mole);
    G4Element* B  = new G4Element("Boron",    "B", z=5., a=10.81*g/mole);
    // Assume 5% boron by mass
    G4double fractionB = 0.05;
    G4double fractionPE = 1.0 - fractionB;
    // Density of borated polyethylene
    density = 0.95*g/cm3;  // typical value
    G4Material* BoratedPE = new G4Material(fBoratedPEMaterialName, density, 3);
    BoratedPE->AddElement(B, fractionB);
    BoratedPE->AddElement(C, 0.857*fractionPE);  // Polyethylene is (CH2)n
    BoratedPE->AddElement(H, 0.143*fractionPE);
}

