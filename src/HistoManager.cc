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
/// \file HistoManager.cc
/// \brief Implementation of the HistoManager class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "HistoManager.hh"

#include "RootManager.hh"

#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

HistoManager::HistoManager()
{
    Book();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void HistoManager::Book()
{
    // Create or get analysis manager
    // The choice of analysis technology is done via selection of a namespace
    // in HistoManager.hh
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetDefaultFileType("root");
    analysisManager->SetFileName(fFileName);
    analysisManager->SetVerboseLevel(1);
    analysisManager->SetActivation(true);  // enable inactivation of histograms
    analysisManager->SetNtupleMerging(true);

    // check if file number set
    RootManager& rootManager = RootManager::GetInstance();
    if(rootManager.GetFileNum() >= 0) {
        std::ostringstream oss;
        oss << fFileName << "_" << std::setw(6) << std::setfill('0') << rootManager.GetFileNum();
        analysisManager->SetFileName(oss.str());
    }

    G4int idx;

    // Ep
    idx = analysisManager->CreateH1("hEp", "incident proton energy distrib.", 2000, 0, 20);
    analysisManager->SetH1Activation(idx, true);

    // ntuple for generating phase space
    idx = analysisManager->CreateNtuple("tree", "spectrum of outgoing particles");
    analysisManager->CreateNtupleDColumn("particle");
    analysisManager->CreateNtupleDColumn("Ekin");
    analysisManager->CreateNtupleDColumn("t");
    analysisManager->CreateNtupleDColumn("x");
    analysisManager->CreateNtupleDColumn("y");
    analysisManager->CreateNtupleDColumn("z");
    analysisManager->CreateNtupleDColumn("px");
    analysisManager->CreateNtupleDColumn("py");
    analysisManager->CreateNtupleDColumn("pz");
    analysisManager->FinishNtuple();
    G4cout << " Created ntuple \"tree\" (id " << idx << ") for neutron phase space" << G4endl;
    
    // ntuple for detector hits
    idx = analysisManager->CreateNtuple("hits", "detector hits");
    analysisManager->CreateNtupleDColumn("particle");
    analysisManager->CreateNtupleDColumn("Edep");
    analysisManager->CreateNtupleDColumn("t");
    analysisManager->CreateNtupleDColumn("x");
    analysisManager->CreateNtupleDColumn("y");
    analysisManager->CreateNtupleDColumn("z");
    analysisManager->FinishNtuple();
    G4cout << " Created ntuple \"hits\" (id " << idx << ") for detector hits" << G4endl;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
