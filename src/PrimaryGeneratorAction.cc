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
    

    // use phase space?
    //fUseNeutronPhaseSpace = true;

    // configured for neutrons generated from phase space file
    //G4ParticleDefinition* particle = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
    //fParticleGun->SetParticleDefinition(particle);
    //fNeutronMass = particle->GetPDGMass();

    // configured for protons incident on the catcher
    //// set particle
    //particle = G4ParticleTable::GetParticleTable()->FindParticle("proton");
    //fGPS->SetParticleDefinition(particle);
    //// set energy dist.
    //fGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Lin");
    //fGPS->GetCurrentSource()->GetEneDist()->SetEmin(1.*MeV);
    //fGPS->GetCurrentSource()->GetEneDist()->SetEmax(10.*MeV);
    //fGPS->GetCurrentSource()->GetEneDist()->SetInterCept(1);
    //fGPS->GetCurrentSource()->GetEneDist()->SetGradient(0);
    //// set angular dist.
    //fGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("cos");
    //fGPS->GetCurrentSource()->GetAngDist()->SetMinTheta(0.*deg);
    //fGPS->GetCurrentSource()->GetAngDist()->SetMaxTheta(20.*deg);
    //fGPS->GetCurrentSource()->GetAngDist()->SetMinPhi(0.*deg);
    //fGPS->GetCurrentSource()->GetAngDist()->SetMaxPhi(360.*deg);
    ////fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(G4ThreeVector(0.,1.,0.));
    //fGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref1", G4ThreeVector(-1, 0, 0));
    //fGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref2", G4ThreeVector(0, 1, 0));
    //// set pos dist.
    //fGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    //fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0.,0.,0.));
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

    if(fSourceMode == kCASTOR440_surface) {
        GenerateCASTOR440Flux();
        fParticleGun->GeneratePrimaryVertex(anEvent); 
    }
    else if(fSourceMode == kCASTOR440_fuel) { 
        GenerateCASTOR440FuelFlux();
        fParticleGun->GeneratePrimaryVertex(anEvent);
    }
    else {
        fGPS->GeneratePrimaryVertex(anEvent);
    }

    //// using neutron file phase space
    //if(fUseNeutronPhaseSpace) {
    //    // 7-d
    //    //double val[7]; // time, x, y, z, px, py, pz
    //    // 3-d
    //    double val[3]; // t, Ekin, theta

    //                   // get the ROOT manager
    //    RootManager& rootManager = RootManager::GetInstance();
    //    if (!rootManager.IsInitialized()) {
    //        G4ExceptionDescription desc;
    //        desc << "RootManager not initialized!";
    //        G4Exception("YourPrimaryGeneratorAction::GeneratePrimaries", 
    //                "RootNotInitialized", FatalException, desc);
    //        return;
    //    }
    //    rootManager.SampleEvent(val);

    //    //G4cout << " ---> sampled " << val[0] << ", " << val[1] << ", " << val[2] << G4endl;

    //    // first implementation:
    //    // 7-d phase space (t, x, y, z, px, py, pz)
    //    //   // set time
    //    //   G4double time = val[0];
    //    //   fParticleGun->SetParticleTime(time);
    //    //   //fGPS->SetParticleTime(time);

    //    //   // set position
    //    //   G4ThreeVector pos(val[1], val[2], val[3]);
    //    //   fParticleGun->SetParticlePosition(pos);
    //    //   //fGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
    //    //   //fGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(pos);

    //    //   // set momentum
    //    //   G4ThreeVector mom(val[4], val[5], val[6]);
    //    //   fParticleGun->SetParticleMomentumDirection(mom.unit());
    //    //   //fGPS->GetCurrentSource()->GetAngDist()->SetParticleMomentumDirection(mom.unit());

    //    //   // set kinetic energy
    //    //   G4double ekin = sqrt(mom.mag()*mom.mag() + fNeutronMass*fNeutronMass) - fNeutronMass; // kinetic energy (MeV)
    //    //   fParticleGun->SetParticleEnergy(ekin);
    //    //   //G4double m_n = particle->GetPDGMass();
    //    //   //G4double ekin = sqrt(mom.mag()*mom.mag() + m_n*m_n) - m_n; // kinetic energy (MeV)
    //    //   //fGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(ekin);

    //    //   // print values
    //    //   //G4cout << G4endl<< G4endl<< G4endl<< G4endl<< G4endl<< G4endl;
    //    //   //G4cout  << "gonna generate a neutron at t = " << val[0] << " ns at (" 
    //    //   //        << val[1] << ", " << val[2] << ", " << val[3] << ") mm with p = ("
    //    //   //        << val[4] << ", " << val[5] << ", " << val[6] << ") MeV/c, "
    //    //   //        << "Ekin = " << ekin << " MeV" << G4endl;

    //    // second implementation:
    //    // 3-d phase space (t, Ekin, theta)
    //    //
    //    // set time
    //    fParticleGun->SetParticleTime(val[0]);
    //    // set energy
    //    fParticleGun->SetParticleEnergy(val[1]);
    //    // set position
    //    G4double phi = G4UniformRand() * 2. * M_PI;
    //    G4double rad = fDetector->GetCatcherRadius() * sqrt(G4UniformRand());
    //    //G4double zz = 5.*cm + 2.*mm * G4UniformRand();
    //    G4double zz = 5.*cm + fDetector->GetCatcherZ() + 1*um;
    //    G4ThreeVector pos(cos(phi)*rad, sin(phi)*rad, zz);
    //    fParticleGun->SetParticlePosition(pos);
    //        // set position
    //        //G4PhysicalVolumeStore* PVStore = G4PhysicalVolumeStore::GetInstance();
    //        //for (auto it = PVStore->begin(); it != PVStore->end(); ++it) {
    //        //    G4VPhysicalVolume* currentVolume = *it;
    //        //    G4String volumeName = currentVolume->GetName();
    //        //    if (volumeName.find("Catcher") != G4String::npos) {
    //        //        auto posDist = fGPS->GetCurrentSource()->GetPosDist();
    //        //        posDist->SetCentreCoords(G4ThreeVector(0.,0.,0.));
    //        //        posDist->SetPosDisType("Volume");
    //        //        posDist->SetPosDisShape("Para");
    //        //        posDist->SetParAlpha(0.*deg);
    //        //        posDist->SetParTheta(0.*deg);
    //        //        posDist->SetHalfX(0.25*m);
    //        //        posDist->SetHalfY(0.25*m);
    //        //        posDist->SetHalfZ(0.25*m);
    //        //        posDist->ConfineSourceToVolume(volumeName);
    //        //        break;
    //        //    }
    //        //}
    //    // set direction
    //    phi = G4UniformRand() * 2. * M_PI;
    //    G4ThreeVector mom;
    //    mom.setRThetaPhi(1., val[2], phi);
    //    fParticleGun->SetParticleMomentumDirection(mom.unit());
    //    

    //    // generate the vertex
    //    fParticleGun->GeneratePrimaryVertex(anEvent); // for first implementation
    //    //fGPS->GeneratePrimaryVertex(anEvent);
    //}
    //// protons incident on catcher
    //else {
    //    fGPS->GeneratePrimaryVertex(anEvent);
    //    G4double Ep = fGPS->GetParticleEnergy();

    //    G4PrimaryVertex* vertex = anEvent->GetPrimaryVertex();
    //    if (vertex && vertex->GetNumberOfParticle() > 0) {
    //        G4PrimaryParticle* primary = vertex->GetPrimary(0);
    //        G4ThreeVector momentumDir = primary->GetMomentumDirection();
    //        G4double theta = momentumDir.theta();
    //        if(momentumDir.x() < 0) theta*= -1;
    //        G4double energy = primary->GetKineticEnergy();
    //        //G4cout << "Direction: " << momentumDir << " Energy: " << energy/MeV << " MeV" << G4endl;
    //        analysis->FillH2(0, 180./M_PI*theta, energy, abs(1./sin(theta)));
    //        analysis->FillH2(1, cos(theta), energy);
    //    }
    //    analysis->FillH1(0, Ep);
    //}
}

void PrimaryGeneratorAction::SetSourceMode(SourceMode mode) {
    fSourceMode = mode; 
}


// particle gun implementation
void PrimaryGeneratorAction::GenerateCASTOR440Flux()
{
    G4int numCasks = fDetector->GetNumCASTOR440s();
    if (numCasks == 0) return;

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

void PrimaryGeneratorAction::GenerateCASTOR440FuelFlux()
{
    G4int numCasks = fDetector->GetNumCASTOR440s();
    if (fCaskNum < 0 || fCaskNum >= numCasks || fFuelNum < 0 || fFuelNum >= 84) {
        G4Exception("PrimaryGeneratorAction", "InvalidSelection", JustWarning, "Invalid cask or fuel element index.");
        return;
    }

    //// 1. Configure the Isotope to decay at rest
    //G4ParticleDefinition* ion = G4IonTable::GetIonTable()->GetIon(fIsotopeZ, fIsotopeA, 0);
    //if (!ion) {
    //    G4Exception("PrimaryGeneratorAction", "InvalidIon", JustWarning, "Could not find requested ion Z/A.");
    //    return;
    //}
    //fParticleGun->SetParticleDefinition(ion);
    //fParticleGun->SetParticleEnergy(0.*eV); // Decay at rest
    // Instead, we will use whatever particle gun parameters have been set

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


////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//void PrimaryGeneratorAction::SetProtons() {
//    fUseNeutronPhaseSpace = false;
//    G4cout << " ---> Setting incident proton beam" << G4endl;
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//void PrimaryGeneratorAction::SetNeutrons() {
//    fUseNeutronPhaseSpace = true;
//    G4cout << " ---> Setting neutrons from catcher" << G4endl;
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//
//void PrimaryGeneratorAction::SetNeutronPhaseSpace(std::shared_ptr<THnSparseD> hist) 
//{
//    if(hist) {
//        fhNeutronPhaseSpace = std::shared_ptr<THnSparseD>((THnSparseD*)hist->Clone());
//        gROOT->GetListOfCleanups()->Remove(fhNeutronPhaseSpace.get());
//    }
//}
//
////....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
