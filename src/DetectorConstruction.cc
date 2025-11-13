//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "DetectorMessenger.hh"
#include "PanelSD.hh"

#include "GeometryCollimator.hh"   
#include "GeometryCatcher.hh"   

#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4SubtractionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4RunManager.hh"
#include "G4SolidStore.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SDManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
    fPosition = G4ThreeVector(0., 0., 0.);
    fRotation = G4ThreeVector(0., 0., 0.);

    fWorldXYZ = 5 * m;

    // default catcher params
    fCatcherRadius = 2.5 * cm;
    fCatcherZ = 2 * mm;
    fCatcherMaterialName = "G4_Be";

    // default collimator params
    fCollimatorXY = 5 * cm;
    fCollimatorZ = 5 * cm;

    // default sample params

    // default detector params
    fDetectorXY = 20.*cm;
    fDetectorZ = 1.*cm;

    DefineMaterials();
    fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
    return ConstructVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::DefineMaterials()
{
    //// borated PE
    //G4Element* H  = new G4Element("Hydrogen", "H", 1., 1.0079*g/mole);
    //G4Element* C  = new G4Element("Carbon",   "C", 6., 12.01*g/mole);
    //G4Element* B  = new G4Element("Boron",    "B", 5., 10.81*g/mole);
    //// Assume 5% boron by mass
    //G4double fractionB = 0.05;
    //G4double fractionPE = 1.0 - fractionB;
    //// Density of borated polyethylene
    //G4double density = 0.95*g/cm3;  // typical value
    //G4Material* BoratedPE = new G4Material("BoratedPE", density, 3);
    //BoratedPE->AddElement(B, fractionB);
    //BoratedPE->AddElement(C, 0.857*fractionPE);  // Polyethylene is (CH2)n
    //BoratedPE->AddElement(H, 0.143*fractionPE);

    /// G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::ConstructVolumes()
{
    // Cleanup old geometry
    G4GeometryManager::GetInstance()->OpenGeometry();
    G4PhysicalVolumeStore::GetInstance()->Clean();
    G4LogicalVolumeStore::GetInstance()->Clean();
    G4SolidStore::GetInstance()->Clean();

    // materials
    G4NistManager* man = G4NistManager::Instance();
    G4Material * material; 

    // world volume
    G4Box* sWorld = new G4Box("sWorld", 
            fWorldXYZ/2., fWorldXYZ/2., fWorldXYZ/2.);
    material = man->FindOrBuildMaterial("G4_AIR");
    fLWorld = new G4LogicalVolume(sWorld, 
            material,
            material->GetName());
    fLWorld->SetVisAttributes(G4VisAttributes::GetInvisible());
    fPWorld = new G4PVPlacement(0,  // no rotation
            G4ThreeVector(),        // at (0,0,0)
            fLWorld,                // its logical volume
            "pWorld",               // its name
            0,                      // its mother  volume
            false,                  // no boolean operation
            0);                     // copy number

    // catcher volume
    //G4Box* sCatcher = new G4Box("sCatcher",  // its name
    //        fCatcherXY/2., fCatcherXY/2., fCatcherZ/2.);  // its dimensions
    //fCatcherMaterial = man->FindOrBuildMaterial("G4_Li");
    //fLCatcher = new G4LogicalVolume(sCatcher,   // its shape
    //        fCatcherMaterial,                      // its material
    //        "lCatcher");          // its name
    //fLCatcher->SetVisAttributes(new G4VisAttributes(true, G4Colour::Green()));
    //fPCatcher = new G4PVPlacement(0,// no rotation
    //        G4ThreeVector(),        // at (0,0,0)
    //        fLCatcher,              // its logical volume
    //        "pCatcher",             // its name
    //        fLWorld,                // its mother  volume
    //        false,                  // no boolean operation
    //        0);                     // copy number

    // extra parameters for collimator
    //G4double cut_extra = 1*cm;
    //
    //// collimator solid and logical
    //G4Box* sCol_PreCut = new G4Box("sColPrecut",
    //        4.*fCollimatorXY, 4.*fCollimatorXY, fCollimatorZ/2.);
    //G4Box* sCol_Cut = new G4Box("sColCut",
    //        fCollimatorXY/2., fCollimatorXY/2., fCollimatorZ/2. + cut_extra);
    //G4SubtractionSolid* sCol = new G4SubtractionSolid("sCol",
    //        sCol_PreCut, sCol_Cut);
    //material = man->FindOrBuildMaterial("BoratedPE");
    //G4LogicalVolume* lCol = new G4LogicalVolume(sCol,
    //        material,
    //        "lCol");
    //lCol->SetVisAttributes(new G4VisAttributes(true, G4Colour::White()));

    //// first collimator
    //G4VPhysicalVolume* pCol1 = new G4PVPlacement(0,
    //        G4ThreeVector(0., 0., fCollimatorDistance),
    //        lCol,
    //        "pCol1",
    //        fLWorld,
    //        false,
    //        0);

    //// second collimator
    //G4VPhysicalVolume* pCol2 = new G4PVPlacement(0,
    //        G4ThreeVector(0., 0., fCollimatorDistance + fCollimatorSpacing),
    //        lCol,
    //        "pCol2",
    //        fLWorld,
    //        false,
    //        1);

    //// third collimator
    //G4VPhysicalVolume* pCol3 = new G4PVPlacement(0,
    //        G4ThreeVector(0., 0., fCollimatorDistance + 2*fCollimatorSpacing),
    //        lCol,
    //        "pCol3",
    //        fLWorld,
    //        false,
    //        2);

    // sample
    G4Tubs * sSampleCyl = new G4Tubs("sSampleCyl", 0, 1.*cm, 5.*cm / 2., 0., 2.*M_PI);
    material = man->FindOrBuildMaterial("G4_Pb");
    G4LogicalVolume * lSampleCyl = new G4LogicalVolume(sSampleCyl,
            material,
            "lSampleCyl");
    lSampleCyl->SetVisAttributes(new G4VisAttributes(true, G4Colour::Red()));
    G4VPhysicalVolume* pSampleCyl = new G4PVPlacement(0,
            //G4ThreeVector(1.*cm, 1.*cm, fCollimatorDistance + 2*fCollimatorSpacing + 10.*cm),
            G4ThreeVector(1.*cm, 1.*cm, 50.*cm),
            lSampleCyl,
            "pSampleCyl",
            fLWorld,
            false,
            0); 

    // detector 
    G4Box* sDetector = new G4Box("sPanel",  // its name
            fDetectorXY/2., fDetectorXY/2., fDetectorZ/2.);  // its dimensions
    fDetectorMaterial = man->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
    fLDetector = new G4LogicalVolume(sDetector,   // its shape
            fDetectorMaterial,                      // its material
            "lPanel");          // its name
    fLDetector->SetVisAttributes(new G4VisAttributes(true, G4Colour::Blue()));
    fPDetector = new G4PVPlacement(0,// no rotation
            //G4ThreeVector(0, 0, fCollimatorDistance + 2*fCollimatorSpacing + 10.*cm + 10.*cm),
            G4ThreeVector(0, 0, 60.*cm),
            fLDetector,             // its logical volume
            "pPanel",               // its name
            fLWorld,                // its mother  volume
            false,                  // no boolean operation
            0);                     // copy number

    //PrintParameters();
    G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    // always return the root volume
    return fPWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors
    G4String panelSDname = "/PanelSD";
    auto panelSD = new PanelSD(panelSDname, "PanelHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(panelSD);
    // Setting trackerSD to all logical volumes with the same name
    // of "Chamber_LV".
    SetSensitiveDetector("lPanel", panelSD);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PlaceCollimator() 
{
    G4cout << " ---> Placing a collimator... " << G4endl;
    
    GeometryCollimator* col = new GeometryCollimator();
    col->SetXY      (fCollimatorXY);    
    col->SetZ       (fCollimatorZ);    
    col->SetInnerXY (fCollimatorInnerXY);
    col->Build();

    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    
    col->PlaceDetector(fLWorld, fPosition, rotate);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PlaceCatcher() 
{
    G4cout << " ---> Placing a catcher... " << G4endl;
    
    GeometryCatcher* catcher = new GeometryCatcher();
    catcher->SetRadius      (fCatcherRadius);    
    catcher->SetZ           (fCatcherZ);    
    catcher->SetMaterialName(fCatcherMaterialName);    
    catcher->Build();

    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    
    catcher->PlaceDetector(fLWorld, fPosition, rotate);

    // after catcher placed, we can set the PV
    G4cout << " ---> Trying to assign catcher physical volume: " << catcher->GetCatcherPhys() << G4endl;
    fPCatcher = catcher->GetCatcherPhys();
    //G4cout << " ---> Set the DetectorConstruction's catcher PV to: " << fPCatcher->GetName() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
