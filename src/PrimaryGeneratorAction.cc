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
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"

#include "DetectorConstruction.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det) : fDetector(det)
{
    fUseGPS = true;

    fParticleGun = new G4ParticleGun(1);
    G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("proton");
    fParticleGun->SetParticleDefinition(particle);
    fParticleGun->SetParticleEnergy(1 * MeV);
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1., 0., 0.));

    fGPS = new G4GeneralParticleSource;
    fGPS->SetParticleDefinition(particle);
    fGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Lin");
    fGPS->GetCurrentSource()->GetEneDist()->SetEmin(1.*MeV);
    fGPS->GetCurrentSource()->GetEneDist()->SetEmax(10.*MeV);
    fGPS->GetCurrentSource()->GetEneDist()->SetInterCept(1);
    fGPS->GetCurrentSource()->GetEneDist()->SetGradient(0);
    auto angDist = fGPS->GetCurrentSource()->GetAngDist();
    angDist->SetAngDistType("planar");
    angDist->SetParticleMomentumDirection(G4ThreeVector(0., 0., 1.));
    auto posDist = fGPS->GetCurrentSource()->GetPosDist();
    posDist->SetPosDisType("Beam");
    //posDist->SetCentreCoords(G4ThreeVector(0., 0., -1*cm));
    //posDist->SetCentreCoords(G4ThreeVector(0., 0., -0.5*fDetector->GetCatcherZ()-10*mm));
    posDist->SetCentreCoords(G4ThreeVector(0., 0., -0.5*fDetector->GetCatcherZ()+1*um)); // start in target
    posDist->SetBeamSigmaInX(2.*mm);
    posDist->SetBeamSigmaInY(2.*mm);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fGPS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
    G4AnalysisManager* analysis = G4AnalysisManager::Instance();
    
    if(!fUseGPS) {
        // this function is called at the begining of event
        //
        G4double halfSize = 0.5 * (fDetector->GetCatcherZ());
        //G4double x0 = -halfSize + 1. * nm;
        G4double x0 = -halfSize - 1. * cm;

        // randomize (y0,z0)
        //
        G4double beam = 0.8 * halfSize;
        G4double y0 = (2 * G4UniformRand() - 1.) * beam;
        G4double z0 = (2 * G4UniformRand() - 1.) * beam;

        fParticleGun->SetParticlePosition(G4ThreeVector(x0, y0, z0));
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    else {
        fGPS->GeneratePrimaryVertex(anEvent);

        double Ep = fGPS->GetParticleEnergy();
        analysis->FillH1(14, Ep);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
