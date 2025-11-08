
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "EventAction.hh"
#include "RunAction.hh"

#include "G4SDManager.hh"
#include "G4HCofThisEvent.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4UnitsTable.hh"

#include "G4RunManager.hh"
#include "G4Run.hh"

//#include "G4RootAnalysisManager.hh"
//#include "g4root.hh"

#include "G4AnalysisManager.hh"

#include "ProgressBar.hh"
#include "G4Threading.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAct)
    :fRunAction(runAct)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event* evt)
{
    //fEvtNb = evt->GetEventID();
    //fRunAction->GetProgBar()->Print(fEvtNb);
    
    uint64_t thisEventNumber = ProgressBar::gEvtNb.fetch_add(1, std::memory_order_relaxed) + 1;
    if(G4Threading::G4GetThreadId() == 0) {
        fRunAction->GetProgBar()->Print(thisEventNumber);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
