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
/// \file DetectorMessenger.hh
/// \brief Definition of the DetectorMessenger class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;
class G4UIcmdWith3Vector;
class G4UIcmdWith3VectorAndUnit;
class G4UIcmdWithABool;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger : public G4UImessenger
{
  public:
    DetectorMessenger(DetectorConstruction*);
    ~DetectorMessenger() override;

    void SetNewValue(G4UIcommand*, G4String) override;

  private:
    DetectorConstruction* fDetector = nullptr;

    // directory
    G4UIdirectory* fDir = nullptr;
    // biasing
    G4UIcmdWithABool* fUseBiasingCmd = nullptr;
    // placement
    G4UIcmdWith3VectorAndUnit*  fSetPositionCmd = nullptr;
    G4UIcmdWith3Vector*         fSetRotationCmd = nullptr;
    // catcher
    G4UIcmdWithADoubleAndUnit*  fSetCatcherRadiusCmd        = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetCatcherZCmd             = nullptr;
    G4UIcmdWithAString*         fSetCatcherMaterialNameCmd  = nullptr;
    G4UIcmdWithoutParameter*    fPlaceCatcherCmd            = nullptr;
    // collimator
    G4UIcmdWithADoubleAndUnit*  fSetSampleRadiusCmd         = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetSampleZCmd              = nullptr;
    G4UIcmdWithAString*         fSetSampleMaterialNameCmd   = nullptr;
    G4UIcmdWithoutParameter*    fPlaceSampleCmd             = nullptr;
    // collimator
    G4UIcmdWithADoubleAndUnit*  fSetCollimatorXYCmd         = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetCollimatorInnerXYCmd    = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetCollimatorZCmd          = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetCollimatorPbZCmd        = nullptr;
    G4UIcmdWithoutParameter*    fPlaceCollimatorCmd         = nullptr;
    // detector panel
    G4UIcmdWithADoubleAndUnit*  fSetDetectorPanelXYCmd         = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetDetectorPanelZCmd          = nullptr;
    G4UIcmdWithoutParameter*    fPlaceDetectorPanelCmd         = nullptr;
    // shielding
    G4UIcmdWithADoubleAndUnit*  fSetShieldingInnerXYCmd             = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetShieldingInnerZCmd              = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetShieldingBoratedPEThicknessCmd  = nullptr;
    G4UIcmdWithADoubleAndUnit*  fSetShieldingPbThicknessCmd         = nullptr;
    G4UIcmdWithoutParameter*    fPlaceShieldingCmd                  = nullptr;

    // DCS monitor
    G4UIcmdWithoutParameter* fAddCLYCCmd = nullptr;
    G4UIcmdWithoutParameter* fAddCLYCByCrystalCenterCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCCrystalRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCCrystalLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCAlumThicknessCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCLiFColInnerRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCLiFColOuterRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCLiFColLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPbColInnerRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPbColOuterRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPbColLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEHDColInnerRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEHDColOuterRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEHDColLengthCmd = nullptr;
    
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEPlugLipRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEPlugInnerRadiusCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEPlugLipLengthCmd = nullptr;
    G4UIcmdWithADoubleAndUnit* fSetCLYCPEPlugInnerLengthCmd = nullptr;

    G4UIcmdWithAString* fSetCLYCCrystalMaterialNameCmd = nullptr;
    G4UIcmdWithAString* fSetCLYCAlumMaterialNameCmd = nullptr;
    G4UIcmdWithAString* fSetCLYCLiFMaterialNameCmd = nullptr;
    G4UIcmdWithAString* fSetCLYCPbMaterialNameCmd = nullptr;
    G4UIcmdWithAString* fSetCLYCPEHDMaterialNameCmd = nullptr;

    G4UIcmdWithoutParameter* fAddCASTOR440Cmd = nullptr;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
