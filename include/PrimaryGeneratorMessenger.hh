//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef PrimaryGeneratorMessenger_h
#define PrimaryGeneratorMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class PrimaryGeneratorAction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWith3Vector;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorMessenger: public G4UImessenger
{
    public:
        PrimaryGeneratorMessenger(PrimaryGeneratorAction*);
        virtual ~PrimaryGeneratorMessenger();

        void SetNewValue(G4UIcommand*, G4String);

    private:
        PrimaryGeneratorAction*     fPrimaryGeneratorAction = nullptr;

        G4UIdirectory*              fDir = nullptr;
        G4UIcmdWithoutParameter*    fSetProtonsCmd = nullptr; 
        G4UIcmdWithoutParameter*    fSetNeutronsCmd = nullptr; 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

