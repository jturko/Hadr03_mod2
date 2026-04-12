#include "GeometryParallelBiasing.hh"
#include "DetectorConstruction.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Tubs.hh"
#include "G4IStore.hh" // Required to set the importance values

GeometryParallelBiasing::GeometryParallelBiasing(G4String worldName, DetectorConstruction* det) 
  : G4VUserParallelWorld(worldName), fDetector(det) {}

void GeometryParallelBiasing::Construct() {


    // The "ghost world" is an exact empty replica of the mass world boundary
    G4VPhysicalVolume* ghostWorld = GetWorld();
    G4LogicalVolume* ghostLogical = ghostWorld->GetLogicalVolume();

    // --- INITIALIZE IMPORTANCE STORE ---
    // The G4IStore is a singleton that holds the importance value mapping for the geometry.
    //G4IStore* iStore = G4IStore::GetInstance();
    G4IStore* iStore = G4IStore::GetInstance(GetName());

    // Set the baseline importance to 1. This applies to the empty world space
    // and the center of the cask (where the fuel is).
    iStore->AddImportanceGeometryCell(1, *ghostWorld);

    // --- CHECK BIASING TOGGLE ---
    // If the macro didn't turn biasing on, we return early. 
    // The parallel world will exist, but everything has an importance of 1,
    // so no splitting or Russian roulette will occur (analog tracking).
    if (!fDetector->GetUseBiasing()) {
        G4cout << " ---> Biasing turned on -- contine constructing GeometryParallelBiasing..." << G4endl;
        return; 
    }

    // --- DEFINE SHELL DIMENSIONS ---
    // We want the shells to start at the inner cavity wall and end at the outer surface.
    G4double rMin = 800.0 * mm;
    G4double rMax = 1330.0 * mm;
    G4double height = 4080.0 * mm; 
    
    // 10 shells provides a smooth splitting gradient. 
    // Too few shells = massive splitting at once (inefficient).
    // Too many shells = tracking overhead checking boundaries constantly.
    G4int nShells = 10; 
    G4double shellThickness = (rMax - rMin) / nShells;

    // --- LOOP OVER DYNAMIC CASTOR BARRELS ---
    // We fetch the dynamic placements from the mass geometry so the ghost shells
    // perfectly overlap the physical casks, regardless of macro configurations.
    G4int numCasks = fDetector->GetNumCASTOR440s();
    for (G4int c = 0; c < numCasks; ++c) {
        
        G4ThreeVector pos = fDetector->GetCASTOR440Position(c);
        G4RotationMatrix* rot = fDetector->GetCASTOR440Rotation(c);
        
        for (G4int i = 0; i < nShells; ++i) {
            G4double innerR = rMin + i * shellThickness;
            G4double outerR = innerR + shellThickness;

            // Create the cylindrical shell solid
            G4String solidName = "BiasingShellSolid_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4Tubs* shellSolid = new G4Tubs(solidName, innerR, outerR, height/2.0, 0.*deg, 360.*deg);
            
            // Create the logical volume (no material needed for parallel worlds)
            G4String logName = "BiasingShellLog_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4LogicalVolume* shellLogical = new G4LogicalVolume(shellSolid, nullptr, logName);
            
            // Place the ghost shell into the ghost world
            G4String physName = "BiasingShellPhys_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4VPhysicalVolume* shellPhys = new G4PVPlacement(rot, pos, shellLogical, 
                                                             physName, ghostLogical, false, c * 100 + i);
            
            // --- ASSIGN IMPORTANCE ---
            // As particles move outward (i increases), the importance doubles.
            // When importance doubles, Geant4 splits the track into 2 tracks,
            // each with half the statistical weight of the original.
            // i=0 -> Imp=2, i=1 -> Imp=4, ... i=9 -> Imp=1024
            G4double importance = std::pow(2.0, i + 1); 
            iStore->AddImportanceGeometryCell(importance, *shellPhys);
        }
    }
}
