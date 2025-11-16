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
#include "GeometrySample.hh"   
#include "GeometryDetectorPanel.hh"   
#include "GeometryShielding.hh"   

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

    fWorldXYZ = 10 * m;

    // default catcher params
    fCatcherRadius = 2.5 * cm;
    fCatcherZ = 2 * mm;
    fCatcherMaterialName = "G4_Be";

    // default collimator params
    fCollimatorXY = 20 * cm;
    fCollimatorZ = 20 * cm;
    fCollimatorInnerXY = 5 * cm;
    fCollimatorPbZ = 2 * cm;

    // default sample params
    fSampleRadius = 5. * mm; 
    fSampleZ = 5. * cm;
    fSampleMaterialName = "G4_Pb";

    // default detector params
    fDetectorPanelXY = 5.*cm;
    fDetectorPanelZ = 1.*cm;

    // default shielding params
    fShieldingInnerXY = 2.*m;
    fShieldingInnerZ = 4.*m;
    fShieldingBoratedPEThickness = 10.*cm;
    fShieldingPbThickness = 2.*cm;

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


    // detector panel
    GeometryDetectorPanel* panel = new GeometryDetectorPanel();
    panel->SetXY(fDetectorPanelXY);
    panel->SetZ(fDetectorPanelZ);
    panel->Build();
    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    panel->PlaceDetector(fLWorld, fPosition, rotate);
    fLDetectorPanel = panel->GetScintiLog();

    //PrintParameters();
    //G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    // always return the root volume
    return fPWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors
    G4String panelSDname = "PanelSD";
    auto panelSD = new PanelSD(panelSDname, "PanelHitsCollection");
    G4SDManager::GetSDMpointer()->AddNewDetector(panelSD);
    SetSensitiveDetector(fLDetectorPanel, panelSD);
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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
void DetectorConstruction::PlaceCollimator() 
{
    G4cout << " ---> Placing a collimator... " << G4endl;
    
    GeometryCollimator* col = new GeometryCollimator();
    col->SetXY      (fCollimatorXY);    
    col->SetZ       (fCollimatorZ);    
    col->SetInnerXY (fCollimatorInnerXY);
    col->SetPbZ     (fCollimatorPbZ);
    col->Build();

    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    
    col->PlaceDetector(fLWorld, fPosition, rotate);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PlaceSample() 
{
    G4cout << " ---> Placing a sample... " << G4endl;
    
    GeometrySample* sample = new GeometrySample();
    sample->SetRadius      (fSampleRadius);    
    sample->SetZ           (fSampleZ);    
    sample->SetMaterialName(fSampleMaterialName);    
    sample->Build();

    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    
    sample->PlaceDetector(fLWorld, fPosition, rotate);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PlaceShielding() 
{
    G4cout << " ---> Placing shielding ... " << G4endl;
    
    GeometryShielding* shield = new GeometryShielding();
    shield->SetInnerXY              (fShieldingInnerXY);    
    shield->SetInnerZ               (fShieldingInnerZ);    
    shield->SetBoratedPEThickness   (fShieldingBoratedPEThickness);    
    shield->SetPbThickness          (fShieldingPbThickness);    
    shield->Build();

    G4RotationMatrix* rotate = new G4RotationMatrix();
    rotate->rotateX(fRotation.x()*M_PI/180.);
    rotate->rotateY(fRotation.y()*M_PI/180.);
    rotate->rotateZ(fRotation.z()*M_PI/180.);    
    
    shield->PlaceDetector(fLWorld, fPosition, rotate);
    
    // after shielding is placed, we can set the PV
    G4cout << " ---> Trying to assign shielding tracker physical volume: " << shield->GetTrackerPhys() << G4endl;
    fPShieldingTracker = shield->GetTrackerPhys();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
