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
/// \file SteppingAction.cc
/// \brief Implementation of the SteppingAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "SteppingAction.hh"

#include "HistoManager.hh"
#include "Run.hh"
#include "DetectorConstruction.hh"

#include "G4HadronicProcess.hh"
#include "G4ParticleTypes.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4Event.hh"

#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// SteppingAction::SteppingAction()
//: G4UserSteppingAction()
//{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det) : G4UserSteppingAction(), fDetector(det)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* aStep)
{
    G4AnalysisManager* analysis = G4AnalysisManager::Instance();
    
    // check trackID and stepNumber
    G4int trackID = aStep->GetTrack()->GetTrackID();
    G4int stepNb = aStep->GetTrack()->GetCurrentStepNumber();

    //// --- TEMPORARY DEBUG: WATCH THE WEIGHT WINDOWS ---
    //const G4VProcess* process = aStep->GetPostStepPoint()->GetProcessDefinedStep();
    //G4String procName = process ? process->GetProcessName() : "None";
    //if (procName == "ImportanceProcess") {
    //    G4double weight = aStep->GetTrack()->GetWeight();
    //    G4String volName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    //    
    //    G4cout << " [Biasing Triggered] " 
    //           << " Track: " << trackID 
    //           << " | Boundary: " << volName 
    //           << " | New Weight: " << weight 
    //           << G4endl;
    //}
    //// -------------------------------------------------

    G4ParticleDefinition* particle = aStep->GetTrack()->GetDefinition();
    G4String volName = aStep->GetPreStepPoint()->GetPhysicalVolume()->GetName();
    
    G4bool inShielding = (volName.find("CastorBody") != G4String::npos || 
                          volName.find("Cavity") != G4String::npos || 
                          volName.find("FuelPhys") != G4String::npos);
    if (inShielding) {
        // 1. Kill ALL Electrons and Positrons immediately (Perfectly fair to do)
        if (particle == G4Electron::Electron() || particle == G4Positron::Positron()) {
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
            return;
        }
        // 2. Kill "Junk" Gammas that have scattered too much to be useful
        // (e.g., anything below 500 keV). Adjust this threshold depending on 
        // how much of the Compton continuum you actually want to see in the CLYC.
        if (particle == G4Gamma::Gamma() && aStep->GetTrack()->GetKineticEnergy() < 200.0 * keV) {
            aStep->GetTrack()->SetTrackStatus(fStopAndKill);
            return;
        }
    }

    //// CASTOR 440 surface flux tracker
    //// For this implementation to work correctly, the placement of the heat dissipation fins should be commented out / deactivated
    //for(G4int c=0; c<fDetector->GetNumCASTOR440s(); c++) {
    //    if(aStep->GetPreStepPoint()->GetPhysicalVolume()->GetLogicalVolume() == fDetector->GetLCASTOR440s()[c] && 
    //       aStep->GetPostStepPoint()->GetPhysicalVolume() == fDetector->GetWorld() &&
    //       (particle->GetPDGEncoding() == 22 || particle->GetPDGEncoding() == 2112)) // gammas and neutrons only
    //    {
    //        G4int idx = 2; // third  ntuple for castor_surf
    //        analysis->FillNtupleDColumn(idx, 0, particle->GetPDGEncoding());
    //        analysis->FillNtupleDColumn(idx, 1, aStep->GetTrack()->GetKineticEnergy());
    //        analysis->FillNtupleDColumn(idx, 2, aStep->GetPostStepPoint()->GetGlobalTime());
    //        analysis->FillNtupleDColumn(idx, 3, aStep->GetPostStepPoint()->GetPosition().x());
    //        analysis->FillNtupleDColumn(idx, 4, aStep->GetPostStepPoint()->GetPosition().y());
    //        analysis->FillNtupleDColumn(idx, 5, aStep->GetPostStepPoint()->GetPosition().z());
    //        analysis->FillNtupleDColumn(idx, 6, aStep->GetPostStepPoint()->GetMomentum().x());
    //        analysis->FillNtupleDColumn(idx, 7, aStep->GetPostStepPoint()->GetMomentum().y());
    //        analysis->FillNtupleDColumn(idx, 8, aStep->GetPostStepPoint()->GetMomentum().z());
    //        analysis->FillNtupleDColumn(idx, 9, aStep->GetPreStepPoint()->GetWeight());
    //        analysis->FillNtupleDColumn(idx,10, G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID());
    //        analysis->AddNtupleRow(idx);
    //    }
    //}

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


