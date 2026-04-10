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
/// \file PrimaryGeneratorAction.hh
/// \brief Definition of the PrimaryGeneratorAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "globals.hh"

#include "THnSparse.h"
#include "TROOT.h"

class G4Event;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

enum SourceMode {
    kGPS,
    kCASTOR440_surface,
    kCASTOR440_fuel
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*);
    ~PrimaryGeneratorAction() override;

  public:
    void GeneratePrimaries(G4Event*) override;
    
    // guns
    G4ParticleGun* GetParticleGun() { return fParticleGun; };
    G4GeneralParticleSource* GetGPS() { return fGPS; };

    // set the source mode (see enum for types)
    void SetSourceMode(SourceMode mode);
    
    // For uniform distrib. in a specific cask/fuelrod combo
    void SetCaskNum(G4int val) { fCaskNum = val; }
    void SetFuelNum(G4int val) { fFuelNum = val; }
    void SetIsotopeZ(G4int val) { fIsotopeZ = val; }
    void SetIsotopeA(G4int val) { fIsotopeA = val; }

    //void SetProtons();
    //void SetNeutrons();
    //void SetNeutronPhaseSpace(std::shared_ptr<THnSparseD>);

  private:
    G4ParticleGun* fParticleGun = nullptr;
    G4GeneralParticleSource* fGPS = nullptr;
    DetectorConstruction* fDetector = nullptr;
    PrimaryGeneratorMessenger* fPrimaryGeneratorMessenger = nullptr;

    SourceMode fSourceMode;
    
    void GenerateCASTOR440Flux();

    void GenerateCASTOR440FuelFlux();
    G4int fCaskNum = 0;
    G4int fFuelNum = 0;
    G4int fIsotopeZ = 27; // Default Co-60
    G4int fIsotopeA = 60;

    //G4double fNeutronMass;
    //G4bool fUseNeutronPhaseSpace;
    //std::shared_ptr<THnSparseD> fhNeutronPhaseSpace;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
