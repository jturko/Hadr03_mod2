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
//
/// \file B2/B2a/src/TrackerSD.cc
/// \brief Implementation of the B2::TrackerSD class

#include "PanelSD.hh"

#include "G4HCofThisEvent.hh"
#include "G4SDManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"

#include "G4AnalysisManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PanelSD::PanelSD(const G4String& name, const G4String& hitsCollectionName)
    : G4VSensitiveDetector(name)
{
    collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PanelSD::Initialize(G4HCofThisEvent* hce)
{
    // Create hits collection
    fHitsCollection = new PanelHitsCollection(SensitiveDetectorName, collectionName[0]);

    // Add this collection in hce
    G4int hcID = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
    hce->AddHitsCollection(hcID, fHitsCollection);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PanelSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    // energy deposit
    G4double edep = step->GetTotalEnergyDeposit();
    
    // if no energy deposited, return
    if (edep == 0.) 
        return false;

    // time
    G4double t = step->GetPreStepPoint()->GetGlobalTime();

    // if not triggered yet
    if(!fTrig) {
        auto newHit = new PanelHit();
        newHit->SetTrackID(step->GetTrack()->GetTrackID());
        newHit->SetEdep(edep);
        newHit->SetPos(step->GetPostStepPoint()->GetPosition());
        newHit->SetTime(t);
        newHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
        fHitsCollection->insert(newHit);

        //G4cout << "inserting new hit" << G4endl;
    }
    // if triggered, but this step has smaller timestamp than hit 
    else if(fTrig && t < ((PanelHit*)fHitsCollection->GetHit(0))->GetTime()) {
        auto oldHit = (PanelHit*)fHitsCollection->GetHit(0);
        oldHit->SetTrackID(step->GetTrack()->GetTrackID());
        oldHit->AddEdep(edep);
        oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
        oldHit->SetTime(t);
        oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
        
        //G4cout << "updating old hit with new time/pos" << G4endl;
    }
    else { // triggered and time greater, just add edep
        auto oldHit = (PanelHit*)fHitsCollection->GetHit(0);
        oldHit->AddEdep(edep);
        
        //G4cout << "updating old hit by just summing the edeps" << G4endl;
    }

    // set triggered to true
    fTrig = true;

    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PanelSD::EndOfEvent(G4HCofThisEvent*)
{
    std::size_t nofHits = fHitsCollection->entries();
    
    if (verboseLevel > 1) {
        G4cout << G4endl << "-------->Hits Collection: in this event they are " << nofHits
            << " hits in the tracker chambers: " << G4endl;
        for (std::size_t i = 0; i < nofHits; i++)
            (*fHitsCollection)[i]->Print();
    }

    G4AnalysisManager* analysis = G4AnalysisManager::Instance();
    for (std::size_t i = 0; i < nofHits; i++) {
        G4double pid        = (*fHitsCollection)[i]->GetPID();
        G4double edep       = (*fHitsCollection)[i]->GetEdep();
        G4double t          = (*fHitsCollection)[i]->GetTime();
        G4ThreeVector pos   = (*fHitsCollection)[i]->GetPos();
        
        // 2nd ntuple is for panel hits
        G4int idx = 1;
        analysis->FillNtupleDColumn(idx, 0, pid);
        analysis->FillNtupleDColumn(idx, 1, edep);
        analysis->FillNtupleDColumn(idx, 2, t);
        analysis->FillNtupleDColumn(idx, 3, pos.x());
        analysis->FillNtupleDColumn(idx, 4, pos.y());
        analysis->FillNtupleDColumn(idx, 5, pos.z());
        analysis->AddNtupleRow(idx);
    }

    fTrig = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

