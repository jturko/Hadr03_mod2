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
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class
//
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"

class G4LogicalVolume;
class G4Material;
class DetectorMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
    public:
        DetectorConstruction();
        ~DetectorConstruction() override;

    public:
        G4VPhysicalVolume* Construct() override;
        void ConstructSDandField() override;

    public:
        const G4VPhysicalVolume* GetWorld()     { return fPWorld; };
        const G4VPhysicalVolume* GetCatcher()   { return fPCatcher; };

        // for messenger
        //
        // placement
        void SetPosition(G4ThreeVector pos)     { fPosition = pos; };
        void SetRotation(G4ThreeVector rot)     { fRotation = rot; };
        // catcher
        void SetCatcherRadius(G4double val)         { fCatcherRadius = val; };
        void SetCatcherZ(G4double val)              { fCatcherZ = val; };
        void SetCatcherMaterialName(G4String val)   { fCatcherMaterialName = val; };
        void PlaceCatcher();
        G4double GetCatcherZ()                      { return fCatcherZ; };
        // collimator
        void SetCollimatorXY(G4double val)          { fCollimatorXY = val; };
        void SetCollimatorZ(G4double val)           { fCollimatorZ = val; };
        void SetCollimatorInnerXY(G4double val)     { fCollimatorInnerXY = val; };
        void PlaceCollimator();
        // sample (cylinder)
        void SetSampleRadius(G4double val)         { fSampleRadius = val; };
        void SetSampleZ(G4double val)              { fSampleZ = val; };
        void SetSampleMaterialName(G4String val)   { fSampleMaterialName = val; };
        void PlaceSample();

    private:
        DetectorMessenger* fDetectorMessenger = nullptr;

        // for next placed volume
        G4ThreeVector       fPosition;
        G4ThreeVector       fRotation;

        // world
        G4VPhysicalVolume*  fPWorld = nullptr;
        G4LogicalVolume*    fLWorld = nullptr;
        G4double            fWorldXYZ;

        // catcher
        G4VPhysicalVolume*  fPCatcher = nullptr;
        G4LogicalVolume*    fLCatcher = nullptr;
        G4double            fCatcherRadius;
        G4double            fCatcherZ;
        G4String            fCatcherMaterialName;
        G4Material*         fCatcherMaterial = nullptr;

        // collimator
        G4double            fCollimatorXY;
        G4double            fCollimatorZ;
        G4double            fCollimatorInnerXY;
        G4Material*         fCollimatorMaterial = nullptr;
        
        // sample
        G4double            fSampleRadius;
        G4double            fSampleZ;
        G4String            fSampleMaterialName;
        G4Material*         fSampleMaterial = nullptr;

        // detector
        G4VPhysicalVolume*  fPDetector = nullptr;
        G4LogicalVolume*    fLDetector = nullptr;
        G4double            fDetectorXY;
        G4double            fDetectorZ;
        G4Material*         fDetectorMaterial = nullptr;


    private:
        void DefineMaterials();
        G4VPhysicalVolume* ConstructVolumes();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
