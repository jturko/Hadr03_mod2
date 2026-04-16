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
#include "HistoManager.hh"
#include "RootManager.hh"
#include "PrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh" 
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

#include "G4PhysicalVolumeStore.hh"
#include "RunAction.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction(DetectorConstruction* det) : fDetector(det)
{
    // setup messenger
    fPrimaryGeneratorMessenger = new PrimaryGeneratorMessenger(this);

    // guns
    fParticleGun = new G4ParticleGun(1);
    fGPS = new G4GeneralParticleSource;

    // source mode
    fSourceMode = kGPS;
    
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

    // variables to save primary info
    G4double ekin, time;
    G4ThreeVector pos, mom;
    G4int particle;

    
    if(fSourceMode == kGPS) { // GPS-based
        fGPS->GeneratePrimaryVertex(anEvent);
    } 
    else { // particle gun-based
        if(fSourceMode == kCASTOR440_surface) {
            GenerateCASTOR440Flux();
            fParticleGun->GeneratePrimaryVertex(anEvent); 
        }
        else if(fSourceMode == kCASTOR440_fuel) { 
            GenerateCASTOR440FuelFlux();
            fParticleGun->GeneratePrimaryVertex(anEvent);
        }
        else if(fSourceMode == kCASTOR440_fuel_biased) {
            GenerateCASTOR440FuelFlux_GeometricCLYCbias(anEvent); // vertex created inside
        }
        else {
            G4Exception("PrimaryGeneratorAction::GeneratePrimaries()",
                        "UnknownSourceMode",
                        FatalException,
                        Form("The source mode '%s' is unknown", fSourceMode));
        }

        ekin =  fParticleGun->GetParticleEnergy();
        time =  fParticleGun->GetParticleTime();
        pos =   fParticleGun->GetParticlePosition();
        mom =   fParticleGun->GetParticleMomentumDirection();
        particle = fParticleGun->GetParticleDefinition()->GetPDGEncoding();
    }

    // save particle info to tree
    // [0]: 1st ntuple is for primaries
    if(RunAction::WritePrimaryTree) {
        G4int idx = 0;
        analysis->FillNtupleDColumn(idx, 0, particle);
        analysis->FillNtupleDColumn(idx, 1, ekin);
        analysis->FillNtupleDColumn(idx, 2, time);
        analysis->FillNtupleDColumn(idx, 3, pos.x());
        analysis->FillNtupleDColumn(idx, 4, pos.y());
        analysis->FillNtupleDColumn(idx, 5, pos.z());
        analysis->FillNtupleDColumn(idx, 6, mom.x());
        analysis->FillNtupleDColumn(idx, 7, mom.y());
        analysis->FillNtupleDColumn(idx, 8, mom.z());
        analysis->AddNtupleRow(idx);
    }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::SetSourceMode(SourceMode mode) {
    fSourceMode = mode; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// particle gun implementation
void PrimaryGeneratorAction::GenerateCASTOR440Flux()
{
    G4int numCasks = fDetector->GetNumCASTOR440s();
    if (fCaskNum < 0 || fCaskNum >= numCasks || fFuelNum < 0 || fFuelNum >= 84) {
        G4Exception("PrimaryGeneratorAction", "InvalidSelection", FatalException, "Invalid cask or fuel element index.");
        return;
    }

    G4int caskIndex = (G4int)(G4UniformRand() * numCasks);
    G4ThreeVector caskPos = fDetector->GetCASTOR440Position(caskIndex);
    G4RotationMatrix* caskRot = fDetector->GetCASTOR440Rotation(caskIndex);

    G4double R = 1330.0 * mm;
    G4double H = 4080.0 * mm;
    G4double areaTop = M_PI * R * R;
    G4double areaSide = 2.0 * M_PI * R * H;
    G4double areaTotal = 2.0 * areaTop + areaSide;

    G4double randArea = G4UniformRand() * areaTotal;
    G4ThreeVector localPos, localNormal;

    if (randArea < areaTop) {
        G4double r = R * std::sqrt(G4UniformRand());
        G4double phi = 2.0 * M_PI * G4UniformRand();
        localPos = G4ThreeVector(r * std::cos(phi), r * std::sin(phi), H / 2.0);
        localNormal = G4ThreeVector(0., 0., 1.);
    } else if (randArea < 2.0 * areaTop) {
        G4double r = R * std::sqrt(G4UniformRand());
        G4double phi = 2.0 * M_PI * G4UniformRand();
        localPos = G4ThreeVector(r * std::cos(phi), r * std::sin(phi), -H / 2.0);
        localNormal = G4ThreeVector(0., 0., -1.);
    } else {
        G4double z = H * (G4UniformRand() - 0.5);
        G4double phi = 2.0 * M_PI * G4UniformRand();
        localPos = G4ThreeVector(R * std::cos(phi), R * std::sin(phi), z);
        localNormal = G4ThreeVector(std::cos(phi), std::sin(phi), 0.);
    }

    G4ThreeVector globalPos = localPos;
    G4ThreeVector globalNormal = localNormal;

    if (caskRot) {
        globalPos.transform(*caskRot);
        globalNormal.transform(*caskRot);
    }
    globalPos += caskPos;

    G4double cosTheta = G4UniformRand(); 
    G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    G4double phiEmit = 2.0 * M_PI * G4UniformRand();

    G4ThreeVector globalDir(sinTheta * std::cos(phiEmit), sinTheta * std::sin(phiEmit), cosTheta);
    globalDir.rotateUz(globalNormal); 

    fParticleGun->SetParticlePosition(globalPos);
    fParticleGun->SetParticleMomentumDirection(globalDir);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateCASTOR440FuelFlux()
{
    G4int numCasks = fDetector->GetNumCASTOR440s();
    if (fCaskNum < 0 || fCaskNum >= numCasks || fFuelNum < 0 || fFuelNum >= 84) {
        G4Exception("PrimaryGeneratorAction", "InvalidSelection", FatalException, "Invalid cask or fuel element index.");
        return;
    }

    // 2. Uniform Hexagon Sampling (via Triangular Decomposition)
    G4double R = 70.0 * mm; // Circumscribed radius of the hexagonal G4Polyhedra
    G4double r1 = G4UniformRand();
    G4double r2 = G4UniformRand();
    if (r1 + r2 > 1.0) {
        r1 = 1.0 - r1;
        r2 = 1.0 - r2;
    }

    // Sample within one foundational equilateral triangle
    G4double x0 = R * r1 + (R / 2.0) * r2;
    G4double y0 = (R * std::sqrt(3.0) / 2.0) * r2;

    // Rotate into one of the 6 sectors uniformly
    G4int sector = (G4int)(G4UniformRand() * 6);
    G4double angle = sector * 60. * deg;
    G4double localX = x0 * std::cos(angle) - y0 * std::sin(angle);
    G4double localY = x0 * std::sin(angle) + y0 * std::cos(angle);

    // 3. Uniform Z Sampling
    G4double cavityHeight = 3500. * mm;
    G4double fuelHeight = cavityHeight - 200. * mm; // Based on zPlanes in GeometryCASTOR440
    G4double localZ = (G4UniformRand() - 0.5) * fuelHeight;

    G4ThreeVector sampledLocalPos(localX, localY, localZ);

    // 4. Transform to global and apply
    G4ThreeVector globalPos = fDetector->GetCASTOR440FuelGlobalPosition(fCaskNum, fFuelNum, sampledLocalPos);
    fParticleGun->SetParticlePosition(globalPos);

    // Provide generic isotropic momentum direction (required by gun, but physics dictates decay isotropy)
    G4double cosTheta = 2.0 * G4UniformRand() - 1.0;
    G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    G4double phiEmit = 2.0 * M_PI * G4UniformRand();
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(sinTheta * std::cos(phiEmit), sinTheta * std::sin(phiEmit), cosTheta));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GenerateCASTOR440FuelFlux_GeometricCLYCbias(G4Event* anEvent)
{
    G4int numCasks = fDetector->GetNumCASTOR440s();
    if (fCaskNum < 0 || fCaskNum >= numCasks || fFuelNum < 0 || fFuelNum >= 84) {
        G4Exception("PrimaryGeneratorAction", "InvalidSelection", FatalException, "Invalid cask or fuel element index.");
        return;
    }

    // 1. Uniform Hexagon Sampling (via Triangular Decomposition)
    G4double R = 70.0 * mm; 
    G4double r1 = G4UniformRand();
    G4double r2 = G4UniformRand();
    if (r1 + r2 > 1.0) {
        r1 = 1.0 - r1;
        r2 = 1.0 - r2;
    }

    G4double x0 = R * r1 + (R / 2.0) * r2;
    G4double y0 = (R * std::sqrt(3.0) / 2.0) * r2;

    G4int sector = (G4int)(G4UniformRand() * 6);
    G4double angle = sector * 60. * deg;
    G4double localX = x0 * std::cos(angle) - y0 * std::sin(angle);
    G4double localY = x0 * std::sin(angle) + y0 * std::cos(angle);

    // 2. Uniform Z Sampling
    G4double cavityHeight = 3500. * mm;
    G4double fuelHeight = cavityHeight - 200. * mm; 
    G4double localZ = (G4UniformRand() - 0.5) * fuelHeight;

    G4ThreeVector sampledLocalPos(localX, localY, localZ);

    // 3. Transform to global position
    G4ThreeVector globalPos = fDetector->GetCASTOR440FuelGlobalPosition(fCaskNum, fFuelNum, sampledLocalPos);
    fParticleGun->SetParticlePosition(globalPos);

    // --- DIRECTIONAL BIASING ---
    G4int numCLYC = fDetector->GetNumCLYC();
    if (numCLYC == 0) {
        G4Exception("PrimaryGeneratorAction", "NoCLYC", FatalException, "No CLYC detectors found to bias towards.");
        return;
    }

    // Uniformly select one of the CLYC detectors
    G4int clycIndex = (G4int)(G4UniformRand() * numCLYC);
    G4ThreeVector clycPos = fDetector->GetCLYCPosition(clycIndex);
    
    // Bounding sphere radius encompassing the CLYC assembly length and width
    G4double boundingRadius = 150.0 * mm; 

    G4ThreeVector directionToDetector = clycPos - globalPos;
    G4double distance = directionToDetector.mag();

    // Calculate maximum cone angle using arcsin (handles cases where origin is close to detector)
    G4double sinThetaMax = boundingRadius / distance;
    if (sinThetaMax > 1.0) sinThetaMax = 1.0; 
    G4double cosThetaMax = std::sqrt(1.0 - sinThetaMax * sinThetaMax);

    // Sample uniformly within the cone
    G4double cosTheta = 1.0 - G4UniformRand() * (1.0 - cosThetaMax);
    G4double sinTheta = std::sqrt(1.0 - cosTheta * cosTheta);
    G4double phiEmit = 2.0 * M_PI * G4UniformRand();

    // Local direction vector
    G4ThreeVector localDir(sinTheta * std::cos(phiEmit), sinTheta * std::sin(phiEmit), cosTheta);
    
    // Rotate to point centrally at the chosen detector
    localDir.rotateUz(directionToDetector.unit());
    fParticleGun->SetParticleMomentumDirection(localDir);

    // 4. Generate the vertex in the event
    fParticleGun->GeneratePrimaryVertex(anEvent);

    // 5. Compute and Apply Statistical Weight
    // The probability of choosing a specific detector is (1 / numCLYC).
    // The fractional solid angle of the cone relative to 4*PI is (1 - cosThetaMax) / 2.
    // Weight = (True PDF) / (Biased PDF)
    G4double fractionalSolidAngle = (1.0 - cosThetaMax) / 2.0;
    G4double weight = numCLYC * fractionalSolidAngle;

    // Apply the weight to the vertex just created
    G4PrimaryVertex* vertex = anEvent->GetPrimaryVertex(anEvent->GetNumberOfPrimaryVertex() - 1);
    if (vertex) {
        vertex->SetWeight(weight);
    }
}

////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
