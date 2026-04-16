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
/// \file DetectorMessenger.cc
/// \brief Implementation of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorMessenger.hh"

#include "DetectorConstruction.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithoutParameter.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det) : fDetector(Det)
{
    // directory
    fDir = new G4UIdirectory("/LDRS/det/");
    fDir->SetGuidance("detector construction commands");
 
    // biasing
    fUseBiasingCmd = new G4UIcmdWithABool("/LDRS/det/useBiasing", this);
    fUseBiasingCmd->SetGuidance("Toggle Geometry Importance Biasing (Weight Windows) on/off.");
    fUseBiasingCmd->SetParameterName("useBiasing", true);
    fUseBiasingCmd->SetDefaultValue(true);
    fUseBiasingCmd->AvailableForStates(G4State_PreInit);

    // placement 
    fSetPositionCmd = new G4UIcmdWith3VectorAndUnit("/LDRS/det/setPosition",this);
    fSetPositionCmd->SetGuidance("set the position of the next volume");
    fSetPositionCmd->SetDefaultUnit("mm");
    //fSetPositionCmd->AvailableForStates(G4State_Idle);
    
    fSetRotationCmd = new G4UIcmdWith3Vector("/LDRS/det/setRotation",this);
    fSetRotationCmd->SetGuidance("set the rotation of the next volume (in degrees)");
    //fSetRotationCmd->AvailableForStates(G4State_Idle);
    
    // catcher
    fSetCatcherRadiusCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCatcherRadius", this);
    fSetCatcherRadiusCmd->SetGuidance("set catcher radius");
    fSetCatcherRadiusCmd->AvailableForStates(G4State_Idle);

    fSetCatcherZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCatcherZ", this);
    fSetCatcherZCmd->SetGuidance("set catcher Z (thickness) dimension");
    fSetCatcherZCmd->AvailableForStates(G4State_Idle);
    
    fSetCatcherMaterialNameCmd  = new G4UIcmdWithAString("/LDRS/det/setCatcherMaterial", this);
    fSetCatcherMaterialNameCmd->SetGuidance("set catcher material name");
    fSetCatcherMaterialNameCmd->AvailableForStates(G4State_Idle);

    fPlaceCatcherCmd = new G4UIcmdWithoutParameter("/LDRS/det/placeCatcher", this);
    fPlaceCatcherCmd->SetGuidance("place a catcher");
    fPlaceCatcherCmd->AvailableForStates(G4State_Idle);
    
    // collimator
    fSetCollimatorXYCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorXY", this);
    fSetCollimatorXYCmd->SetGuidance("set collimator outer XY dimensions");
    fSetCollimatorXYCmd->AvailableForStates(G4State_Idle);
    
    fSetCollimatorInnerXYCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorInnerXY", this);
    fSetCollimatorInnerXYCmd->SetGuidance("set collimator inner XY dimensions");
    fSetCollimatorInnerXYCmd->AvailableForStates(G4State_Idle);

    fSetCollimatorZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorZ", this);
    fSetCollimatorZCmd->SetGuidance("set collimator Z (thickness) dimension");
    fSetCollimatorZCmd->AvailableForStates(G4State_Idle);
    
    fSetCollimatorPbZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorPbZ", this);
    fSetCollimatorPbZCmd->SetGuidance("set collimator Pb Z (thickness) dimension");
    fSetCollimatorPbZCmd->AvailableForStates(G4State_Idle);

    fPlaceCollimatorCmd = new G4UIcmdWithoutParameter("/LDRS/det/placeCollimator", this);
    fPlaceCollimatorCmd->SetGuidance("place a collimator");
    fPlaceCollimatorCmd->AvailableForStates(G4State_Idle);
    
    // sample
    fSetSampleRadiusCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setSampleRadius", this);
    fSetSampleRadiusCmd->SetGuidance("set sample radius");
    fSetSampleRadiusCmd->AvailableForStates(G4State_Idle);

    fSetSampleZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setSampleZ", this);
    fSetSampleZCmd->SetGuidance("set sample Z (thickness) dimension");
    fSetSampleZCmd->AvailableForStates(G4State_Idle);
    
    fSetSampleMaterialNameCmd  = new G4UIcmdWithAString("/LDRS/det/setSampleMaterial", this);
    fSetSampleMaterialNameCmd->SetGuidance("set sample material name");
    fSetSampleMaterialNameCmd->AvailableForStates(G4State_Idle);

    fPlaceSampleCmd = new G4UIcmdWithoutParameter("/LDRS/det/placeSample", this);
    fPlaceSampleCmd->SetGuidance("place a sample");
    fPlaceSampleCmd->AvailableForStates(G4State_Idle);
    
    // detector panel
    fSetDetectorPanelXYCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setPanelXY", this);
    fSetDetectorPanelXYCmd->SetGuidance("set detector panel outer XY dimensions");
    //fSetDetectorPanelXYCmd->AvailableForStates(G4State_Idle);

    fSetDetectorPanelZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setPanelZ", this);
    fSetDetectorPanelZCmd->SetGuidance("set detector panel Z (thickness) dimension");
    //fSetDetectorPanelZCmd->AvailableForStates(G4State_Idle);
    
    //fPlaceDetectorPanelCmd = new G4UIcmdWithoutParameter("/LDRS/det/placePanel", this);
    //fPlaceDetectorPanelCmd->SetGuidance("place a detector panel");
    //fPlaceDetectorPanelCmd->AvailableForStates(G4State_Idle);
    
    // shielding
    fSetShieldingInnerXYCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setShieldingInnerXY", this);
    fSetShieldingInnerXYCmd->SetGuidance("set shielding inner XY dimensions");
    fSetShieldingInnerXYCmd->AvailableForStates(G4State_Idle);
    
    fSetShieldingInnerZCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setShieldingInnerZ", this);
    fSetShieldingInnerZCmd->SetGuidance("set shielding inner Z dimensions");
    fSetShieldingInnerZCmd->AvailableForStates(G4State_Idle);
    
    fSetShieldingBoratedPEThicknessCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setShieldingPEThickness", this);
    fSetShieldingBoratedPEThicknessCmd->SetGuidance("set shielding borated PE thickness");
    fSetShieldingBoratedPEThicknessCmd->AvailableForStates(G4State_Idle);
    
    fSetShieldingPbThicknessCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setShieldingPbThickness", this);
    fSetShieldingPbThicknessCmd->SetGuidance("set shielding Pb thickness");
    fSetShieldingPbThicknessCmd->AvailableForStates(G4State_Idle);

    fPlaceShieldingCmd = new G4UIcmdWithoutParameter("/LDRS/det/placeShielding", this);
    fPlaceShieldingCmd->SetGuidance("place shielding");
    fPlaceShieldingCmd->AvailableForStates(G4State_Idle);

    // DCS monitor
    // CLYC detector 
    fAddCLYCCmd = new G4UIcmdWithoutParameter("/LDRS/det/clyc/add", this);
    fAddCLYCCmd->AvailableForStates(G4State_PreInit);
    fAddCLYCByCrystalCenterCmd = new G4UIcmdWithoutParameter("/LDRS/det/clyc/addByCrystalCenter", this);
    fAddCLYCByCrystalCenterCmd->AvailableForStates(G4State_PreInit);
    // crystal
    fSetCLYCCrystalRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setCrystalRadius", this);
    fSetCLYCCrystalRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCCrystalLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setCrystalLength", this);
    fSetCLYCCrystalLengthCmd->AvailableForStates(G4State_PreInit);
    // aluminum casing
    fSetCLYCAlumThicknessCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setAlumThickness", this);
    fSetCLYCAlumThicknessCmd->AvailableForStates(G4State_PreInit);
    // LiF collimator liner
     fSetCLYCLiFColInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setLiFColInnerRadius", this);
     fSetCLYCLiFColInnerRadiusCmd->AvailableForStates(G4State_PreInit);
     fSetCLYCLiFColOuterRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setLiFColOuterRadius", this);
     fSetCLYCLiFColOuterRadiusCmd->AvailableForStates(G4State_PreInit);
     fSetCLYCLiFColLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setLiFColLength", this);
     fSetCLYCLiFColLengthCmd->AvailableForStates(G4State_PreInit);
     // Pb collimator
    fSetCLYCPbColInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPbColInnerRadius", this);
    fSetCLYCPbColInnerRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPbColOuterRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPbColOuterRadius", this);
    fSetCLYCPbColOuterRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPbColLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPbColLength", this);
    fSetCLYCPbColLengthCmd->AvailableForStates(G4State_PreInit);
    // PE collimator
    fSetCLYCPEHDColInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEHDColInnerRadius", this);
    fSetCLYCPEHDColInnerRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEHDColOuterRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEHDColOuterRadius", this);
    fSetCLYCPEHDColOuterRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEHDColLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEHDColLength", this);
    fSetCLYCPEHDColLengthCmd->AvailableForStates(G4State_PreInit);
    // PE plug
    fSetCLYCPEPlugLipRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEPlugLipRadius", this);
    fSetCLYCPEPlugLipRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEPlugInnerRadiusCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEPlugInnerRadius", this);
    fSetCLYCPEPlugInnerRadiusCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEPlugLipLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEPlugLipLength", this);
    fSetCLYCPEPlugLipLengthCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEPlugInnerLengthCmd = new G4UIcmdWithADoubleAndUnit("/LDRS/det/clyc/setPEPlugInnerLength", this);
    fSetCLYCPEPlugInnerLengthCmd->AvailableForStates(G4State_PreInit);
    // materials
    fSetCLYCCrystalMaterialNameCmd = new G4UIcmdWithAString("/LDRS/det/clyc/setCrystalMaterial", this);
    fSetCLYCCrystalMaterialNameCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCAlumMaterialNameCmd = new G4UIcmdWithAString("/LDRS/det/clyc/setAlumMaterial", this);
    fSetCLYCAlumMaterialNameCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCLiFMaterialNameCmd = new G4UIcmdWithAString("/LDRS/det/clyc/setLiFMaterial", this);
    fSetCLYCLiFMaterialNameCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPbMaterialNameCmd = new G4UIcmdWithAString("/LDRS/det/clyc/setPbMaterial", this);
    fSetCLYCPbMaterialNameCmd->AvailableForStates(G4State_PreInit);
    fSetCLYCPEHDMaterialNameCmd = new G4UIcmdWithAString("/LDRS/det/clyc/setPEHDMaterial", this);
    fSetCLYCPEHDMaterialNameCmd->AvailableForStates(G4State_PreInit);
    
    // CASTOR 440
    fAddCASTOR440Cmd = new G4UIcmdWithoutParameter("/LDRS/det/castor440/add", this);
    fAddCASTOR440Cmd->AvailableForStates(G4State_PreInit);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
    delete fDir;
    
    delete fUseBiasingCmd;

    delete fSetPositionCmd;
    delete fSetRotationCmd;
    
    delete fSetCatcherRadiusCmd;
    delete fSetCatcherZCmd;
    delete fSetCatcherMaterialNameCmd;
    delete fPlaceCatcherCmd;

    delete fSetCollimatorXYCmd;
    delete fSetCollimatorInnerXYCmd;
    delete fSetCollimatorZCmd;
    delete fSetCollimatorPbZCmd;
    delete fPlaceCollimatorCmd;
    
    delete fSetSampleRadiusCmd;
    delete fSetSampleZCmd;
    delete fSetSampleMaterialNameCmd;
    delete fPlaceSampleCmd;
    
    delete fSetDetectorPanelXYCmd;
    delete fSetDetectorPanelZCmd;
    //delete fPlaceDetectorPanelCmd;
    
    delete fSetShieldingInnerXYCmd;
    delete fSetShieldingInnerZCmd;
    delete fSetShieldingBoratedPEThicknessCmd;
    delete fSetShieldingPbThicknessCmd;
    delete fPlaceShieldingCmd;
    
    // DCS monitor
    delete fAddCLYCCmd;
    delete fAddCLYCByCrystalCenterCmd;

    delete fSetCLYCCrystalRadiusCmd;
    delete fSetCLYCCrystalLengthCmd;
    
    delete fSetCLYCAlumThicknessCmd;
    
    delete fSetCLYCLiFColInnerRadiusCmd;
    delete fSetCLYCLiFColOuterRadiusCmd;
    delete fSetCLYCLiFColLengthCmd;
    
    delete fSetCLYCPbColInnerRadiusCmd;
    delete fSetCLYCPbColOuterRadiusCmd;
    delete fSetCLYCPbColLengthCmd;
    
    delete fSetCLYCPEHDColInnerRadiusCmd;
    delete fSetCLYCPEHDColOuterRadiusCmd;
    delete fSetCLYCPEHDColLengthCmd;

    delete fSetCLYCPEPlugLipRadiusCmd;
    delete fSetCLYCPEPlugInnerRadiusCmd;
    delete fSetCLYCPEPlugLipLengthCmd;
    delete fSetCLYCPEPlugInnerLengthCmd;

    delete fSetCLYCCrystalMaterialNameCmd;
    delete fSetCLYCAlumMaterialNameCmd;
    delete fSetCLYCPbMaterialNameCmd;
    delete fSetCLYCPEHDMaterialNameCmd;
    
    // CASTOR 440
    delete fAddCASTOR440Cmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String value)
{
    // biasing
    if (command == fUseBiasingCmd) {
        fDetector->SetUseBiasing(fUseBiasingCmd->GetNewBoolValue(value));
    }

    // placement
    if(command == fSetPositionCmd) {
        fDetector->SetPosition(fSetPositionCmd->GetNew3VectorValue(value));
    }
    if(command == fSetRotationCmd) {
        fDetector->SetRotation(fSetRotationCmd->GetNew3VectorValue(value));
    }
    
    // catcher
    if(command == fSetCatcherRadiusCmd) {
        fDetector->SetCatcherRadius(fSetCatcherRadiusCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCatcherZCmd) {
        fDetector->SetCatcherZ(fSetCatcherZCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCatcherMaterialNameCmd) {
        fDetector->SetCatcherMaterialName(value);
    }
    if(command == fPlaceCatcherCmd) {
        fDetector->PlaceCatcher();
    }
    
    // collimator
    if(command == fSetCollimatorXYCmd) {
        fDetector->SetCollimatorXY(fSetCollimatorXYCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCollimatorInnerXYCmd) {
        fDetector->SetCollimatorInnerXY(fSetCollimatorInnerXYCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCollimatorZCmd) {
        fDetector->SetCollimatorZ(fSetCollimatorZCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCollimatorPbZCmd) {
        fDetector->SetCollimatorPbZ(fSetCollimatorPbZCmd->GetNewDoubleValue(value));
    }
    if(command == fPlaceCollimatorCmd) {
        fDetector->PlaceCollimator();
    }
    
    // sample
    if(command == fSetSampleRadiusCmd) {
        fDetector->SetSampleRadius(fSetSampleRadiusCmd->GetNewDoubleValue(value));
    }
    if(command == fSetSampleZCmd) {
        fDetector->SetSampleZ(fSetSampleZCmd->GetNewDoubleValue(value));
    }
    if(command == fSetSampleMaterialNameCmd) {
        fDetector->SetSampleMaterialName(value);
    }
    if(command == fPlaceSampleCmd) {
        fDetector->PlaceSample();
    }
    
     // detector panel
     if(command == fSetDetectorPanelXYCmd) {
         fDetector->SetDetectorPanelXY(fSetDetectorPanelXYCmd->GetNewDoubleValue(value));
     }
     if(command == fSetDetectorPanelZCmd) {
         fDetector->SetDetectorPanelZ(fSetDetectorPanelZCmd->GetNewDoubleValue(value));
     }
    // if(command == fPlaceDetectorPanelCmd) {
    //     fDetector->PlaceDetectorPanel();
    // }
    
    // shielding
    if(command == fSetShieldingInnerXYCmd) {
        fDetector->SetShieldingInnerXY(fSetShieldingInnerXYCmd->GetNewDoubleValue(value));
    }
    if(command == fSetShieldingInnerZCmd) {
        fDetector->SetShieldingInnerZ(fSetShieldingInnerZCmd->GetNewDoubleValue(value));
    }
    if(command == fSetShieldingBoratedPEThicknessCmd) {
        fDetector->SetShieldingBoratedPEThickness(fSetShieldingBoratedPEThicknessCmd->GetNewDoubleValue(value));
    }
    if(command == fSetShieldingPbThicknessCmd) {
        fDetector->SetShieldingPbThickness(fSetShieldingPbThicknessCmd->GetNewDoubleValue(value));
    }
    if(command == fPlaceShieldingCmd) {
        fDetector->PlaceShielding();
    }

    // DCS monitor
    if(command == fAddCLYCCmd) fDetector->AddCLYC();
    if(command == fAddCLYCByCrystalCenterCmd) fDetector->AddCLYCByCrystalCenter();

    if(command == fSetCLYCCrystalRadiusCmd) fDetector->SetCLYCCrystalRadius(fSetCLYCCrystalRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCCrystalLengthCmd) fDetector->SetCLYCCrystalLength(fSetCLYCCrystalLengthCmd->GetNewDoubleValue(value));
    
    if(command == fSetCLYCAlumThicknessCmd) fDetector->SetCLYCAlumThickness(fSetCLYCAlumThicknessCmd->GetNewDoubleValue(value));
    
    if(command == fSetCLYCLiFColInnerRadiusCmd) fDetector->SetCLYCLiFCollimatorInnerRadius(fSetCLYCLiFColInnerRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCLiFColOuterRadiusCmd) fDetector->SetCLYCLiFCollimatorOuterRadius(fSetCLYCLiFColOuterRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCLiFColLengthCmd) fDetector->SetCLYCLiFCollimatorLength(fSetCLYCLiFColLengthCmd->GetNewDoubleValue(value));
    
    if(command == fSetCLYCPbColInnerRadiusCmd) fDetector->SetCLYCPbCollimatorInnerRadius(fSetCLYCPbColInnerRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPbColOuterRadiusCmd) fDetector->SetCLYCPbCollimatorOuterRadius(fSetCLYCPbColOuterRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPbColLengthCmd) fDetector->SetCLYCPbCollimatorLength(fSetCLYCPbColLengthCmd->GetNewDoubleValue(value));
    
    if(command == fSetCLYCPEHDColInnerRadiusCmd) fDetector->SetCLYCPEHDCollimatorInnerRadius(fSetCLYCPEHDColInnerRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPEHDColOuterRadiusCmd) fDetector->SetCLYCPEHDCollimatorOuterRadius(fSetCLYCPEHDColOuterRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPEHDColLengthCmd) fDetector->SetCLYCPEHDCollimatorLength(fSetCLYCPEHDColLengthCmd->GetNewDoubleValue(value));

    if(command == fSetCLYCPEPlugLipRadiusCmd)   fDetector->SetCLYCPEPlugLipRadius(  fSetCLYCPEPlugLipRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPEPlugInnerRadiusCmd) fDetector->SetCLYCPEPlugInnerRadius(fSetCLYCPEPlugInnerRadiusCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPEPlugLipLengthCmd)   fDetector->SetCLYCPEPlugLipLength(  fSetCLYCPEPlugLipLengthCmd->GetNewDoubleValue(value));
    if(command == fSetCLYCPEPlugInnerLengthCmd) fDetector->SetCLYCPEPlugInnerLength(fSetCLYCPEPlugInnerLengthCmd->GetNewDoubleValue(value));

    if(command == fSetCLYCCrystalMaterialNameCmd) fDetector->SetCLYCCrystalMaterialName(value);
    if(command == fSetCLYCAlumMaterialNameCmd) fDetector->SetCLYCAlumMaterialName(value);
    if(command == fSetCLYCLiFMaterialNameCmd) fDetector->SetCLYCLiFMaterialName(value);
    if(command == fSetCLYCPbMaterialNameCmd) fDetector->SetCLYCPbMaterialName(value);
    if(command == fSetCLYCPEHDMaterialNameCmd) fDetector->SetCLYCPEHDMaterialName(value);
    
    // CASTOR 440
    if(command == fAddCASTOR440Cmd) fDetector->AddCASTOR440();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


