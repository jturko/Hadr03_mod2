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
    fDir = new G4UIdirectory("/dcs-monitor/gun/");
    fDir->SetGuidance("primary generator commands");

    // set SourceMode enum
    fSourceModeCmd = new G4UIcmdWithAString("/dcs-monitor/gun/sourceMode", this);
    fSourceModeCmd->SetGuidance("Select the source distribution mode.");
    fSourceModeCmd->SetParameterName("mode", false);
    fSourceModeCmd->SetCandidates("GPS CASTOR440_surface CASTOR440_fuel CASTOR440_fuel_biased");
    fSourceModeCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

    fCaskNumCmd = new G4UIcmdWithAnInteger("/dcs-monitor/gun/caskNum", this);
    fCaskNumCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
    fFuelNumCmd = new G4UIcmdWithAnInteger("/dcs-monitor/gun/fuelNum", this);
    fFuelNumCmd->AvailableForStates(G4State_PreInit, G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorMessenger::~PrimaryGeneratorMessenger()
{
    delete fDir;
    
    delete fSourceModeCmd;

    delete fCaskNumCmd;
    delete fFuelNumCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{
    if (newValue == "GPS") {
            G4cout << " --> Setting source mode to the G4GeneralParticleSource (kGPS)" << G4endl;
            fPrimaryGeneratorAction->SetSourceMode(kGPS);
    }
    
    if (newValue == "CASTOR440_surface") {
            G4cout << " --> Setting source mode to the CASTOR 440/84 surface flux (kCASTOR440_surface)" << G4endl;
            fPrimaryGeneratorAction->SetSourceMode(kCASTOR440_surface);
    }

    if (command == fCaskNumCmd) fPrimaryGeneratorAction->SetCaskNum(fCaskNumCmd->GetNewIntValue(newValue));
    if (command == fFuelNumCmd) fPrimaryGeneratorAction->SetFuelNum(fFuelNumCmd->GetNewIntValue(newValue));
    if (newValue == "CASTOR440_fuel") {
        G4cout << " --> Setting source mode to CASTOR 440/84 fuel element volume flux (kCASTOR440_fuel)" << G4endl;
        fPrimaryGeneratorAction->SetSourceMode(kCASTOR440_fuel);
    }
    if (newValue == "CASTOR440_fuel_biased") {
        G4cout << " --> Setting source mode to CASTOR 440/84 fuel element volume flux with directional bias (kCASTOR440_fuel_biased)" << G4endl;
        fPrimaryGeneratorAction->SetSourceMode(kCASTOR440_fuel_biased);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


