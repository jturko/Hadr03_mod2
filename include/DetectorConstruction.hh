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
#include "G4RotationMatrix.hh"

class G4LogicalVolume;
class G4Material;

class DetectorMessenger;
class GeometryCLYC;
class GeometryCASTOR440;

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
        void SetUseBiasing(G4bool val) { fUseBiasing = val; }
        G4bool GetUseBiasing() const { return fUseBiasing; }

        const G4VPhysicalVolume* GetWorld()     { return fPWorld; };

        // for messenger
        
        // placement
        void SetPosition(G4ThreeVector pos)     { fPosition = pos; };
        void SetRotation(G4ThreeVector rot)     { fRotation = rot; };
        
        // CLYC
        void AddCLYC();
        void AddCLYCByCrystalCenter();
        G4int GetNumCLYC() const { return fCLYCDetectors.size(); }
        G4ThreeVector GetCLYCPosition(G4int index) const { return fCLYCPositions[index]; }

        void SetCLYCCrystalRadius(G4double val);
        void SetCLYCCrystalLength(G4double val);

        void SetCLYCAlumThickness(G4double val);

        void SetCLYCLiFCollimatorInnerRadius(G4double val);
        void SetCLYCLiFCollimatorOuterRadius(G4double val);
        void SetCLYCLiFCollimatorLength(G4double val);

        void SetCLYCPbCollimatorInnerRadius(G4double val);
        void SetCLYCPbCollimatorOuterRadius(G4double val);
        void SetCLYCPbCollimatorLength(G4double val);

        void SetCLYCPEHDCollimatorInnerRadius(G4double val);
        void SetCLYCPEHDCollimatorOuterRadius(G4double val);
        void SetCLYCPEHDCollimatorLength(G4double val);

        void SetCLYCPEPlugLipRadius(G4double val);
        void SetCLYCPEPlugInnerRadius(G4double val);
        void SetCLYCPEPlugLipLength(G4double val);
        void SetCLYCPEPlugInnerLength(G4double val);

        void SetCLYCCrystalMaterialName(G4String val);
        void SetCLYCAlumMaterialName(G4String val);
        void SetCLYCLiFMaterialName(G4String val);
        void SetCLYCPbMaterialName(G4String val);
        void SetCLYCPEHDMaterialName(G4String val);
    
        // CASTOR 440
        void AddCASTOR440();
        
        G4int GetNumCASTOR440s() const { return fCASTOR440Detectors.size(); }
        G4ThreeVector GetCASTOR440Position(G4int index) const { return fCASTOR440Positions[index]; }
        G4RotationMatrix* GetCASTOR440Rotation(G4int index) const { return fCASTOR440Rotations[index]; }
        
        G4ThreeVector GetCASTOR440FuelGlobalPosition(G4int caskIndex, G4int fuelIndex, G4ThreeVector pointInFuel) const;

    private:
        DetectorMessenger* fDetectorMessenger = nullptr;

        // for biasing
        G4bool fUseBiasing = false;

        // for next placed volume
        G4ThreeVector       fPosition;
        G4ThreeVector       fRotation;

        // world
        G4VPhysicalVolume*  fPWorld = nullptr;
        G4LogicalVolume*    fLWorld = nullptr;
        G4double            fWorldXYZ;

        // CLYC
        std::vector<GeometryCLYC*> fCLYCDetectors;
        std::vector<G4ThreeVector> fCLYCPositions;
        std::vector<G4RotationMatrix*> fCLYCRotations;
        std::vector<G4bool> fCLYCPlaceByCrystalCenter;

        // castor 440
        std::vector<GeometryCASTOR440*> fCASTOR440Detectors;
        std::vector<G4ThreeVector> fCASTOR440Positions;
        std::vector<G4RotationMatrix*> fCASTOR440Rotations;

    private:
        void DefineMaterials();
        G4VPhysicalVolume* ConstructVolumes();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
