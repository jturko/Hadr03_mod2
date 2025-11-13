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
#include "G4UIcommand.hh"
#include "G4UIdirectory.hh"
#include "G4UIparameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::DetectorMessenger(DetectorConstruction* Det) : fDetector(Det)
{
    // directory
    fDir = new G4UIdirectory("/LDRS/det/");
    fDir->SetGuidance("detector construction commands");
    
    // placement 
    fSetPositionCmd = new G4UIcmdWith3VectorAndUnit("/LDRS/det/setPosition",this);
    fSetPositionCmd->SetGuidance("set the position of the next volume");
    fSetPositionCmd->SetDefaultUnit("mm");
    fSetPositionCmd->AvailableForStates(G4State_Idle);
    
    fSetRotationCmd = new G4UIcmdWith3Vector("/LDRS/det/setRotation",this);
    fSetRotationCmd->SetGuidance("set the rotation of the next volume (in degrees)");
    fSetRotationCmd->AvailableForStates(G4State_Idle);
    
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

    fSetCollimatorZCmd   = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorZ", this);
    fSetCollimatorZCmd->SetGuidance("set collimator Z (thickness) dimension");
    fSetCollimatorZCmd->AvailableForStates(G4State_Idle);
    
    fSetCollimatorInnerXYCmd  = new G4UIcmdWithADoubleAndUnit("/LDRS/det/setCollimatorInnerXY", this);
    fSetCollimatorInnerXYCmd->SetGuidance("set collimator inner XY dimensions");
    fSetCollimatorInnerXYCmd->AvailableForStates(G4State_Idle);

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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
    delete fDir;
    
    delete fSetPositionCmd;
    delete fSetRotationCmd;
    
    delete fSetCatcherRadiusCmd;
    delete fSetCatcherZCmd;
    delete fSetCatcherMaterialNameCmd;
    delete fPlaceCatcherCmd;

    delete fSetCollimatorXYCmd;
    delete fSetCollimatorZCmd;
    delete fSetCollimatorInnerXYCmd;
    delete fPlaceCollimatorCmd;
    
    delete fSetSampleRadiusCmd;
    delete fSetSampleZCmd;
    delete fSetSampleMaterialNameCmd;
    delete fPlaceSampleCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorMessenger::SetNewValue(G4UIcommand* command, G4String value)
{
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
    if(command == fSetCollimatorZCmd) {
        fDetector->SetCollimatorZ(fSetCollimatorZCmd->GetNewDoubleValue(value));
    }
    if(command == fSetCollimatorInnerXYCmd) {
        fDetector->SetCollimatorInnerXY(fSetCollimatorInnerXYCmd->GetNewDoubleValue(value));
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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


