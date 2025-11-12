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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorMessenger::~DetectorMessenger()
{
    delete fDir;
    
    delete fSetPositionCmd;
    delete fSetRotationCmd;

    delete fSetCollimatorXYCmd;
    delete fSetCollimatorZCmd;
    delete fSetCollimatorInnerXYCmd;
    delete fPlaceCollimatorCmd;
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

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


