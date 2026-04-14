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

    fTrackHitIndexMap.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool PanelSD::ProcessHits(G4Step* step, G4TouchableHistory*)
{
    G4int trackID = step->GetTrack()->GetTrackID();
    G4int parentID = step->GetTrack()->GetParentID();
    G4int hitIndex = -1;

    // Detect if this track is crossing into the detector from the outside
    G4bool isEntering = (step->GetPreStepPoint()->GetStepStatus() == fGeomBoundary);

    // 1. Resolve Ancestry
    if (fTrackHitIndexMap.find(trackID) != fTrackHitIndexMap.end()) {
        // Track is already registered
        hitIndex = fTrackHitIndexMap[trackID];
    } else if (!isEntering && fTrackHitIndexMap.find(parentID) != fTrackHitIndexMap.end()) {
        // Track was created INSIDE the detector (physical secondary). Map to parent's hit.
        hitIndex = fTrackHitIndexMap[parentID];
        fTrackHitIndexMap[trackID] = hitIndex;
    }

    G4double w = step->GetPreStepPoint()->GetWeight(); 

    // 2. Create the "Shower Container" if no valid hit index exists
    if (hitIndex == -1) {
        auto newHit = new PanelHit();
        newHit->SetTrackID(trackID);
        newHit->SetEdep(0.); // Initialise to zero
        newHit->SetWeight(w);
        
        hitIndex = fHitsCollection->insert(newHit) - 1;
        fTrackHitIndexMap[trackID] = hitIndex;
    } 

    G4double edep = step->GetTotalEnergyDeposit();

    // 3. Record physical data ONLY when energy is deposited
    if (edep > 0.) {
        auto oldHit = (PanelHit*)fHitsCollection->GetHit(hitIndex);
        G4double t = step->GetPreStepPoint()->GetGlobalTime();
        
        if (oldHit->GetEdep() == 0.) {
            oldHit->SetTime(t);
            oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
            oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
        } 
        else if (t < oldHit->GetTime()) {
            oldHit->SetTime(t);
            oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
            oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
        }

        oldHit->AddEdep(edep);
    }

    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//// This implementation yields artificial pile-up, due to inheritance via parent of splitted tracks
//G4bool PanelSD::ProcessHits(G4Step* step, G4TouchableHistory*)
//{
//    G4int trackID = step->GetTrack()->GetTrackID();
//    G4int parentID = step->GetTrack()->GetParentID();
//    G4int hitIndex = -1;
//
//    // 1. Resolve Ancestry first to map intermediate neutral particles
//    if (fTrackHitIndexMap.find(trackID) != fTrackHitIndexMap.end()) {
//        hitIndex = fTrackHitIndexMap[trackID];
//    } else if (fTrackHitIndexMap.find(parentID) != fTrackHitIndexMap.end()) {
//        hitIndex = fTrackHitIndexMap[parentID];
//        fTrackHitIndexMap[trackID] = hitIndex; 
//    }
//
//    G4double w = step->GetPreStepPoint()->GetWeight(); 
//
//    // 2. Create the "Shower Container" immediately to establish the map
//    if (hitIndex == -1) {
//        auto newHit = new PanelHit();
//        newHit->SetTrackID(trackID);
//        newHit->SetEdep(0.); // Initialise to zero
//        newHit->SetWeight(w);
//        
//        hitIndex = fHitsCollection->insert(newHit) - 1;
//        fTrackHitIndexMap[trackID] = hitIndex;
//    } 
//
//    G4double edep = step->GetTotalEnergyDeposit();
//
//    // 3. Record physical data ONLY when energy is deposited
//    if (edep > 0.) {
//        auto oldHit = (PanelHit*)fHitsCollection->GetHit(hitIndex);
//        G4double t = step->GetPreStepPoint()->GetGlobalTime();
//        
//        // If this is the very first energy deposition in this shower, 
//        // lock in the physical observables.
//        if (oldHit->GetEdep() == 0.) {
//            oldHit->SetTime(t);
//            oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
//            oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        } 
//        // If the shower already deposited energy, update only if this step is earlier
//        else if (t < oldHit->GetTime()) {
//            oldHit->SetTime(t);
//            oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
//            oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        }
//
//        oldHit->AddEdep(edep);
//    }
//
//    return true;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//G4bool PanelSD::ProcessHits(G4Step* step, G4TouchableHistory*)
//{
//    G4double edep = step->GetTotalEnergyDeposit();
//    if (edep == 0.) return false; // Skip steps that deposit no energy
//
//    G4double t = step->GetPreStepPoint()->GetGlobalTime();
//    G4double w = step->GetPreStepPoint()->GetWeight(); 
//    G4int trackID = step->GetTrack()->GetTrackID();
//    G4int parentID = step->GetTrack()->GetParentID();
//    G4int hitIndex = -1;
//
//    // --- AVOID ARTIFICIAL PILE-UP ---
//    // 1. Check if this specific track already has a hit recorded.
//    if (fTrackHitIndexMap.find(trackID) != fTrackHitIndexMap.end()) {
//        hitIndex = fTrackHitIndexMap[trackID];
//    } 
//    // 2. Check if this is a secondary particle generated INSIDE the detector 
//    // (e.g., a photoelectron). It belongs to the parent's energy shower, 
//    // so we map it to the parent's existing hit.
//    else if (fTrackHitIndexMap.find(parentID) != fTrackHitIndexMap.end()) {
//        hitIndex = fTrackHitIndexMap[parentID];
//        fTrackHitIndexMap[trackID] = hitIndex; 
//    }
//
//    // 3. If no hit exists for this track branch, create a new one.
//    if (hitIndex == -1) {
//        auto newHit = new PanelHit();
//        newHit->SetTrackID(trackID);
//        newHit->SetEdep(edep); // Record pure physical energy
//        newHit->SetPos(step->GetPostStepPoint()->GetPosition());
//        newHit->SetTime(t);
//        newHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        newHit->SetWeight(w); // Assign the statistical weight to the hit
//        
//        // Insert into collection and save the index to the map
//        hitIndex = fHitsCollection->insert(newHit) - 1;
//        fTrackHitIndexMap[trackID] = hitIndex;
//    } 
//    // 4. If the hit exists, accumulate the energy of this step.
//    else {
//        auto oldHit = (PanelHit*)fHitsCollection->GetHit(hitIndex);
//        oldHit->AddEdep(edep); // Pure physical energy accumulation
//        
//        // CRITICAL: Do NOT modify the hit's weight here. 
//        // The weight represents the statistical probability of this entire shower 
//        // occurring, which was established when the primary particle entered the volume.
//        
//        // Update timing/position if this step happened earlier
//        if (t < oldHit->GetTime()) {
//            oldHit->SetTime(t);
//            oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
//            oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        }
//    }
//    return true;
//}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//G4bool PanelSD::ProcessHits(G4Step* step, G4TouchableHistory*)
//{
//    // energy deposit
//    G4double edep = step->GetTotalEnergyDeposit();
// 
//    //G4cout << " ---> In PanelSD::ProcessHits() ... " << G4endl;
//
//    // if no energy deposited, return
//    if (edep == 0.) 
//        return false;
//
//    // time and weight
//    G4double t = step->GetPreStepPoint()->GetGlobalTime();
//    G4double w = step->GetPreStepPoint()->GetWeight();
//
//    // if not triggered yet
//    if(!fTrig) {
//        auto newHit = new PanelHit();
//        newHit->SetTrackID(step->GetTrack()->GetTrackID());
//        newHit->SetEdep(edep);
//        newHit->SetPos(step->GetPostStepPoint()->GetPosition());
//        newHit->SetTime(t);
//        newHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        newHit->SetWeight(w);
//        fHitsCollection->insert(newHit);
//
//        //G4cout << "inserting new hit" << G4endl;
//    }
//    // if triggered, but this step has smaller timestamp than hit 
//    else if(fTrig && t < ((PanelHit*)fHitsCollection->GetHit(0))->GetTime()) {
//        auto oldHit = (PanelHit*)fHitsCollection->GetHit(0);
//        oldHit->SetTrackID(step->GetTrack()->GetTrackID());
//        oldHit->AddEdep(edep);
//        oldHit->SetPos(step->GetPostStepPoint()->GetPosition());
//        oldHit->SetTime(t);
//        oldHit->SetPID(step->GetTrack()->GetParticleDefinition()->GetPDGEncoding());
//        oldHit->SetWeight(w);
//
//        //G4cout << "updating old hit with new time/pos" << G4endl;
//    }
//    else { // triggered and time greater, just add edep
//        auto oldHit = (PanelHit*)fHitsCollection->GetHit(0);
//        oldHit->AddEdep(edep);
//        
//        //G4cout << "updating old hit by just summing the edeps" << G4endl;
//    }
//
//    // set triggered to true
//    fTrig = true;
//
//    return true;
//}

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
        G4double edep       = (*fHitsCollection)[i]->GetEdep();
        if(edep == 0.) continue; // skip hits which exist only due to the shower tracker,
                                 // which will generate a hit even for edep=0
        G4double pid        = (*fHitsCollection)[i]->GetPID();
        G4double t          = (*fHitsCollection)[i]->GetTime();
        G4ThreeVector pos   = (*fHitsCollection)[i]->GetPos();
        G4int det           = (*fHitsCollection)[i]->GetDetNum();
        G4double weight     = (*fHitsCollection)[i]->GetWeight();
        
        // 2nd ntuple is for panel hits
        G4int idx = 1;
        analysis->FillNtupleDColumn(idx, 0, pid);
        analysis->FillNtupleDColumn(idx, 1, edep);
        analysis->FillNtupleDColumn(idx, 2, t);
        analysis->FillNtupleDColumn(idx, 3, pos.x());
        analysis->FillNtupleDColumn(idx, 4, pos.y());
        analysis->FillNtupleDColumn(idx, 5, pos.z());
        analysis->FillNtupleIColumn(idx, 6, det);
        analysis->FillNtupleDColumn(idx, 7, weight);
        analysis->AddNtupleRow(idx);
    }

    fTrig = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

