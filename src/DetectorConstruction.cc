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
#include "GeometryCLYC.hh"
#include "GeometryCASTOR440.hh"

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

#include "GeometryParallelBiasing.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
{
    fPosition = G4ThreeVector(0., 0., 0.);
    fRotation = G4ThreeVector(0., 0., 0.);

    fWorldXYZ = 25 * m;

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

    // biasing
    RegisterParallelWorld(new GeometryParallelBiasing("ParallelBiasingWorld", this));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
    delete fDetectorMessenger;

    for (auto clyc : fCLYCDetectors) {
        delete clyc;
    }
    for (auto rot : fCLYCRotations) {
        delete rot;
    }

    for (auto castor : fCASTOR440Detectors) {
        delete castor;
    }
    for (auto rot : fCASTOR440Rotations) {
        delete rot;
    }
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


    //// detector panel
    //GeometryDetectorPanel* panel = new GeometryDetectorPanel();
    //panel->SetXY(fDetectorPanelXY);
    //panel->SetZ(fDetectorPanelZ);
    //panel->Build();
    //G4RotationMatrix* rotate = new G4RotationMatrix();
    //rotate->rotateX(fRotation.x()*M_PI/180.);
    //rotate->rotateY(fRotation.y()*M_PI/180.);
    //rotate->rotateZ(fRotation.z()*M_PI/180.);    
    //panel->PlaceDetector(fLWorld, fPosition, rotate);
    //fLDetectorPanel = panel->GetScintiLog();

    for (size_t i = 0; i < fCLYCDetectors.size(); ++i) {
        fCLYCDetectors[i]->Build();

        if (fCLYCPlaceByCrystalCenter[i]) {
            fCLYCDetectors[i]->PlaceDetectorByCrystalCenter(fLWorld, fCLYCPositions[i], fCLYCRotations[i], i);
        } else {
            fCLYCDetectors[i]->PlaceDetector(fLWorld, fCLYCPositions[i], fCLYCRotations[i], i);
        }
    }

    
    for (size_t i = 0; i < fCASTOR440Detectors.size(); ++i) {
        fCASTOR440Detectors[i]->Build();
        fCASTOR440Detectors[i]->PlaceDetector(fLWorld, fCASTOR440Positions[i], fCASTOR440Rotations[i], i);
    }

    //PrintParameters();
    //G4cout << *(G4Material::GetMaterialTable()) << G4endl;

    // always return the root volume
    return fPWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructSDandField()
{
    // Sensitive detectors
    //G4String panelSDname = "PanelSD";
    //auto panelSD = new PanelSD(panelSDname, "PanelHitsCollection");
    //G4SDManager::GetSDMpointer()->AddNewDetector(panelSD);
    //SetSensitiveDetector(fLDetectorPanel, panelSD);

    if (!fCLYCDetectors.empty()) {
        G4String clycSDname = "ClycSD";
        auto clycSD = new PanelSD(clycSDname, "ClycHitsCollection"); 
        G4SDManager::GetSDMpointer()->AddNewDetector(clycSD);
        
        for (auto clyc : fCLYCDetectors) {
            G4LogicalVolume* logVol = clyc->GetCLYCLog();
            if (logVol) {
                SetSensitiveDetector(logVol, clycSD);
            }
        }
    }

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

void DetectorConstruction::AddCLYC() 
{
    fCLYCDetectors.push_back(new GeometryCLYC());
    fCLYCPositions.push_back(fPosition);
    
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(fRotation.x()*M_PI/180.);
    rot->rotateY(fRotation.y()*M_PI/180.);
    rot->rotateZ(fRotation.z()*M_PI/180.); 
    fCLYCRotations.push_back(rot);
    
    fCLYCPlaceByCrystalCenter.push_back(false); // Flag for default placement
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::AddCLYCByCrystalCenter() 
{
    fCLYCDetectors.push_back(new GeometryCLYC());
    fCLYCPositions.push_back(fPosition);
    
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(fRotation.x()*M_PI/180.);
    rot->rotateY(fRotation.y()*M_PI/180.);
    rot->rotateZ(fRotation.z()*M_PI/180.); 
    fCLYCRotations.push_back(rot);
    
    fCLYCPlaceByCrystalCenter.push_back(true); // Flag for COM placement
}

void DetectorConstruction::SetCLYCCrystalRadius(G4double val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetCrystalRadius(val); }
void DetectorConstruction::SetCLYCCrystalLength(G4double val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetCrystalLength(val); }
void DetectorConstruction::SetCLYCAlumThickness(G4double val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetAlumCasingThickness(val); }

void DetectorConstruction::SetCLYCLiFCollimatorInnerRadius(G4double val)    { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetLiFCollimatorInnerRadius(val); }
void DetectorConstruction::SetCLYCLiFCollimatorOuterRadius(G4double val)    { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetLiFCollimatorOuterRadius(val); }
void DetectorConstruction::SetCLYCLiFCollimatorLength(G4double val)         { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetLiFCollimatorLength(val); }

void DetectorConstruction::SetCLYCPbCollimatorInnerRadius(G4double val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPbCollimatorInnerRadius(val); }
void DetectorConstruction::SetCLYCPbCollimatorOuterRadius(G4double val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPbCollimatorOuterRadius(val); }
void DetectorConstruction::SetCLYCPbCollimatorLength(G4double val)      { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPbCollimatorLength(val); }

void DetectorConstruction::SetCLYCPEHDCollimatorInnerRadius(G4double val)   { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEHDCollimatorInnerRadius(val); }
void DetectorConstruction::SetCLYCPEHDCollimatorOuterRadius(G4double val)   { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEHDCollimatorOuterRadius(val); }
void DetectorConstruction::SetCLYCPEHDCollimatorLength(G4double val)        { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEHDCollimatorLength(val); }

void DetectorConstruction::SetCLYCPEPlugLipRadius(G4double val)     { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEPlugLipRadius(val); }
void DetectorConstruction::SetCLYCPEPlugInnerRadius(G4double val)   { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEPlugInnerRadius(val); }
void DetectorConstruction::SetCLYCPEPlugLipLength(G4double val)     { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEPlugLipLength(val); }
void DetectorConstruction::SetCLYCPEPlugInnerLength(G4double val)   { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEPlugInnerLength(val); }

void DetectorConstruction::SetCLYCCrystalMaterialName(G4String val) { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetCrystalMaterialName(val); }
void DetectorConstruction::SetCLYCAlumMaterialName(G4String val)    { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetAlumMaterialName(val); }
void DetectorConstruction::SetCLYCLiFMaterialName(G4String val)     { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetLiFMaterialName(val); }
void DetectorConstruction::SetCLYCPbMaterialName(G4String val)      { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPbMaterialName(val); }
void DetectorConstruction::SetCLYCPEHDMaterialName(G4String val)    { if (!fCLYCDetectors.empty()) fCLYCDetectors.back()->SetPEHDMaterialName(val); }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::AddCASTOR440() 
{
    fCASTOR440Detectors.push_back(new GeometryCASTOR440());
    fCASTOR440Positions.push_back(fPosition);
    
    G4RotationMatrix* rot = new G4RotationMatrix();
    rot->rotateX(fRotation.x()*M_PI/180.);
    rot->rotateY(fRotation.y()*M_PI/180.);
    rot->rotateZ(fRotation.z()*M_PI/180.); 
    fCASTOR440Rotations.push_back(rot);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// Add at the bottom of the file:
G4ThreeVector DetectorConstruction::GetCASTOR440FuelGlobalPosition(G4int caskIndex, G4int fuelIndex, G4ThreeVector pointInFuel) const {
    if (caskIndex < 0 || caskIndex >= fCASTOR440Detectors.size()) return G4ThreeVector();

    G4ThreeVector fuelCenter = fCASTOR440Detectors[caskIndex]->GetFuelPosition(fuelIndex);

    // The cavity is shifted by Z = -150 mm relative to the cask body.
    G4ThreeVector caskLocalPos = pointInFuel + fuelCenter + G4ThreeVector(0., 0., -150. * mm);

    G4ThreeVector globalPos = caskLocalPos;
    G4RotationMatrix* rot = fCASTOR440Rotations[caskIndex];
    if (rot) globalPos.transform(*rot);

    globalPos += fCASTOR440Positions[caskIndex];
    return globalPos;
}
