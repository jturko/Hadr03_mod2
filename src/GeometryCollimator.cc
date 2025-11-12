
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

#include "GeometryCollimator.hh"

#include "G4SystemOfUnits.hh"

#include <string>

GeometryCollimator::GeometryCollimator() :
    fCollimatorAssembly(NULL)
{
    // physical dimensions
    fXY = 10.*cm;
    fZ = 5.*cm;
    fInnerXY = 5.*cm;

    // material
    fMaterialName = "BoratedPE";

    // colour
    fColour = G4Colour::White();

    // copy number
    fCopyNo = 0;
}

GeometryCollimator::~GeometryCollimator()
{
}

G4int GeometryCollimator::Build()
{
    BuildMaterials();

    fCollimatorAssembly = new G4AssemblyVolume();
    
    G4ThreeVector move, direction;
    G4RotationMatrix* rotate = NULL;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* material = manager->FindOrBuildMaterial(fMaterialName);

    // extra parameters 
    G4double cut_extra = 1.*cm;

    // solid
    G4Box* sCol_PreCut = new G4Box("sColPrecut",    fXY/2.,         fXY/2.,         fZ/2.);
    G4Box* sCol_Cut = new G4Box("sColCut",          fInnerXY/2.,    fInnerXY/2.,    fZ/2. + cut_extra);
    G4SubtractionSolid* sCol = new G4SubtractionSolid("sCol", sCol_PreCut, sCol_Cut);

    // logical
    fCollimatorLog = new G4LogicalVolume(sCol, // solid volume
            material,                          // material
            "CollimatorLog");                  // name
                                               
    // vis attributes                                              
    fCollimatorLog->SetVisAttributes(new G4VisAttributes(true, fColour));

    // add to assembly
    move = G4ThreeVector(0., 0., 0.);
    rotate = new G4RotationMatrix();
    fCollimatorAssembly->AddPlacedVolume(fCollimatorLog, move, rotate);

    return 1;
}

G4int GeometryCollimator::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate) 
{
    G4bool surfCheck = true;
    fCollimatorAssembly->MakeImprint(logic_world, move, rotate, fCopyNo, surfCheck);
    return fCopyNo++;
}

void GeometryCollimator::BuildMaterials() 
{
    G4double z, a, density;
    G4int ncomp, natoms;

    // borated PE
    G4Element* H  = new G4Element("Hydrogen", "H", z=1., a=1.0079*g/mole);
    G4Element* C  = new G4Element("Carbon",   "C", z=6., a=12.01*g/mole);
    G4Element* B  = new G4Element("Boron",    "B", z=5., a=10.81*g/mole);

    // Assume 5% boron by mass
    G4double fractionB = 0.05;
    G4double fractionPE = 1.0 - fractionB;

    // Density of borated polyethylene
    density = 0.95*g/cm3;  // typical value
    G4Material* BoratedPE = new G4Material("BoratedPE", density, 3);
    BoratedPE->AddElement(B, fractionB);
    BoratedPE->AddElement(C, 0.857*fractionPE);  // Polyethylene is (CH2)n
    BoratedPE->AddElement(H, 0.143*fractionPE);
}

