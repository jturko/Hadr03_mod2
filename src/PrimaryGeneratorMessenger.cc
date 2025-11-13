//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorMessenger.hh"

#include "PrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithoutParameter.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::PrimaryGeneratorMessenger(PrimaryGeneratorAction* gun)
:fPrimaryGeneratorAction(gun)
{
    // directory
    fDir = new G4UIdirectory("/LDRS/gun/");
    fDir->SetGuidance("primary generator commands");

    // set incident protons (with GeneralParticleSource)
    fSetProtonsCmd = new G4UIcmdWithoutParameter("/LDRS/gun/setProtons", this);
    fSetProtonsCmd->SetGuidance("set incident protons on catcher");
    fSetProtonsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
    
    // set incident neutron (from phase space file)
    fSetNeutronsCmd = new G4UIcmdWithoutParameter("/LDRS/gun/setNeutrons", this);
    fSetNeutronsCmd->SetGuidance("set neutrons emitted from catcher");
    fSetNeutronsCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
    delete fDir;
    delete fSetProtonsCmd;
    delete fSetNeutronsCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if(command == fSetProtonsCmd) {
        fPrimaryGeneratorAction->SetProtons();
    }

    if(command == fSetNeutronsCmd) {
        fPrimaryGeneratorAction->SetNeutrons();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


