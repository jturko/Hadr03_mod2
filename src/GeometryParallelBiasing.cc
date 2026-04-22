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
    G4VPhysicalVolume* ghostWorld = GetWorld();
    G4LogicalVolume* ghostLogical = ghostWorld->GetLogicalVolume();
    G4IStore* iStore = G4IStore::GetInstance(GetName());

    if (!fDetector->GetUseBiasing()) {
        G4cout << " [Biasing] Geometry Importance Biasing is OFF. Analog tracking only." << G4endl;
        iStore->AddImportanceGeometryCell(1, *ghostWorld);
        return; 
    }
    
    // Define the total number of shells to be used in the imporance biasing
    G4int nShells = 20; 

    // 1. Set the background world importance to the MAXIMUM value (2^nShells).
    // This ensures tracks that escape the cask are not killed by Russian Roulette
    // in the air before they reach the detector.
    iStore->AddImportanceGeometryCell(pow(2,nShells), *ghostWorld);

    // 2. Elongate the shells to span the entire 25m world to prevent 
    // particles from escaping the top/bottom and bypassing the importance gradient.
    G4double rMin = 800.0 * mm;
    G4double rMax = 1330.0 * mm;
    G4double height = 24.0 * m; 
    G4double shellThickness = (rMax - rMin) / nShells;
    

    G4int numCasks = fDetector->GetNumCASTOR440s();
    for (G4int c = 0; c < numCasks; ++c) {
        G4ThreeVector pos = fDetector->GetCASTOR440Position(c);
        G4RotationMatrix* rot = fDetector->GetCASTOR440Rotation(c);
        
        // 3. Create an explicit Inner Core (Importance 1)
        // This covers the fuel region where primary particles are born.
        G4Tubs* coreSolid = new G4Tubs("BiasingCoreSolid", 0.*mm, rMin, height/2.0, 0.*deg, 360.*deg);
        G4LogicalVolume* coreLogical = new G4LogicalVolume(coreSolid, nullptr, "BiasingCoreLog");
        G4VPhysicalVolume* corePhys = new G4PVPlacement(rot, pos, coreLogical, "BiasingCorePhys", ghostLogical, false, c*100 + 99);
        
        iStore->AddImportanceGeometryCell(1, *corePhys, c*100 + 99);

        // 4. Create the Splitting Shells (Importance 2 to 1024)
        for (G4int i = 0; i < nShells; ++i) {
            G4double innerR = rMin + i * shellThickness;
            G4double outerR = innerR + shellThickness;

            G4String solidName = "BiasingShellSolid_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4Tubs* shellSolid = new G4Tubs(solidName, innerR, outerR, height/2.0, 0.*deg, 360.*deg);
            
            G4String logName = "BiasingShellLog_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4LogicalVolume* shellLogical = new G4LogicalVolume(shellSolid, nullptr, logName);
            
            G4String physName = "BiasingShellPhys_c" + std::to_string(c) + "_s" + std::to_string(i);
            G4VPhysicalVolume* shellPhys = new G4PVPlacement(rot, pos, shellLogical, 
                                                             physName, ghostLogical, false, c * 100 + i);
            
            G4double importance = std::pow(2.0, i + 1); 
            iStore->AddImportanceGeometryCell(importance, *shellPhys, c * 100 + i);
        }
    }
}
