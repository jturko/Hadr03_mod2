#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"

#include "THnSparse.h"
#include "TROOT.h"

class G4Event;
class DetectorConstruction;
class PrimaryGeneratorMessenger;

enum SourceMode {
    kGPS,
    kCASTOR440_surface,
    kCASTOR440_fuel,
    kCASTOR440_fuel_biased
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction(DetectorConstruction*);
    ~PrimaryGeneratorAction() override;

  public:
    void GeneratePrimaries(G4Event*) override;

    G4ParticleGun*           GetParticleGun() { return fParticleGun; }
    G4GeneralParticleSource* GetGPS()         { return fGPS; }

    void SetSourceMode(SourceMode mode);

    void SetCaskNum (G4int val) { fCaskNum  = val; }
    void SetFuelNum (G4int val) { fFuelNum  = val; }
    void SetIsotopeZ(G4int val) { fIsotopeZ = val; }
    void SetIsotopeA(G4int val) { fIsotopeA = val; }

    // Bounding sphere radius around the CLYC assembly used by the directional
    // bias. Exposed so it can be tied to GeometryCLYC dimensions later if you
    // want a tighter cone.
    void     SetCLYCBoundingRadius(G4double r) { fCLYCBoundingRadius = r; }
    G4double GetCLYCBoundingRadius() const     { return fCLYCBoundingRadius; }

  private:
    G4ParticleGun*             fParticleGun = nullptr;
    G4GeneralParticleSource*   fGPS         = nullptr;
    DetectorConstruction*      fDetector    = nullptr;
    PrimaryGeneratorMessenger* fPrimaryGeneratorMessenger = nullptr;

    SourceMode fSourceMode;

    // ----- Source mode implementations -----
    void GenerateCASTOR440Flux();                                     // surface flux
    void GenerateCASTOR440FuelFlux();                                 // unbiased fuel flux
    void GenerateCASTOR440FuelFlux_GeometricCLYCbias(G4Event* event); // biased fuel flux

    // ----- Shared primitives used by both biased and unbiased modes -----
    // Returns a uniformly sampled global position inside (fCaskNum, fFuelNum).
    // Throws via G4Exception if the configured indices are invalid.
    G4ThreeVector GenerateFuelVertexPosition();
    // Sets a globally isotropic momentum direction on fParticleGun.
    void          SetIsotropicDirection();
    // Sets a directional-bias momentum direction (cone subtending the CLYC
    // bounding sphere) and returns the statistical weight to apply to the
    // resulting primary vertex.
    G4double      SetBiasedDirectionTowardsCLYC(const G4ThreeVector& vertexPos);

    // Source / fuel selection
    G4int    fCaskNum  = 0;
    G4int    fFuelNum  = 0;
    G4int    fIsotopeZ = 27; // default Co-60
    G4int    fIsotopeA = 60;

    // Default bounding sphere radius used by the geometric bias
    G4double fCLYCBoundingRadius = 150.0 * CLHEP::mm;
};

#endif

