#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Tubs.hh"
#include "G4Polyhedra.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4AssemblyVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"

#include "GeometryCASTOR440.hh"
#include <string>
#include <cmath>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GeometryCASTOR440::GeometryCASTOR440() :
    fCASTORAssembly(NULL),
    fCASTORBodyLog(NULL),
    fCavityLog(NULL),
    fLidLog(NULL),
    fFuelAssemblyLog(NULL),
    fModeratorRodLog(NULL),
    fFinLog(NULL)
{
    // Real-world dimensions for CASTOR 440/84
    fCaskHeight         = 4080.    * mm;
    fCaskInnerRadius    = 900.     * mm;
    fCaskOuterRadius    = 1270.    * mm;
    fFinTipRadius       = 1330.    * mm;
    fCavityHeight       = 3260.    * mm;
    fLidThickness       = 290.     * mm;
    fBottomThickness    = 390.     * mm;
    fActiveFuelLength   = 2420.    * mm;
    fTotalFuelLength    = 3217.    * mm;

    //fCastIronMatName = "G4_AIR";
    fCastIronMatName = "CastIron";
    
    fHeliumMatName   = "G4_He";
    fPEMatName       = "G4_POLYETHYLENE";
    fFuelMatName     = "UO2";

    //fSteelMatName    = "G4_AIR";
    fSteelMatName    = "G4_STAINLESS-STEEL";

    // initialize fuel assembly positions
    GenerateFuelPositions();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GeometryCASTOR440::~GeometryCASTOR440() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int GeometryCASTOR440::Build()
{
    G4cout << " -> Constructing GeometryCASTOR440..." << G4endl;
        
    G4bool surfCheck = true;

    BuildMaterials();
    fCASTORAssembly = new G4AssemblyVolume();
    
    G4NistManager* manager = G4NistManager::Instance();
    G4Material* mat_CastIron = manager->FindOrBuildMaterial(fCastIronMatName);
    G4Material* mat_He       = manager->FindOrBuildMaterial(fHeliumMatName);
    G4Material* mat_PE       = manager->FindOrBuildMaterial(fPEMatName);
    G4Material* mat_Fuel     = manager->FindOrBuildMaterial(fFuelMatName);
    G4Material* mat_Steel    = manager->FindOrBuildMaterial(fSteelMatName);

    G4ThreeVector move;
    G4RotationMatrix* rotate = nullptr;
    G4double alpha = 1; // Transparency for debugging internal structure

    // --- IRON CASK BODY ---
    // 1. Cask Body (Main Mother Volume)
    G4Tubs* body_solid = new G4Tubs("CastorBody", 0.*mm, fCaskOuterRadius, fCaskHeight/2.0, 0.*deg, 360.*deg);
    fCASTORBodyLog = new G4LogicalVolume(body_solid, mat_CastIron, "CastorBodyLog");
    fCASTORBodyLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.3, 0.3, 0.3, alpha)));


    // --- HELIUM CAVITY NEAR TOP LID --- 
    // 2. Cavity (Filled with Helium)
    G4Tubs* cavity_solid = new G4Tubs("Cavity", 0.*mm, fCaskInnerRadius, fCavityHeight/2.0, 0.*deg, 360.*deg);
    fCavityLog = new G4LogicalVolume(cavity_solid, mat_He, "CavityLog");
    fCavityLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.9, 0.9, 0.9, alpha)));
    // Shifted downwards to leave space for the massive steel lid at the top
    //G4double cavityZ = -150. * mm;
    G4double cavityZ = -fCaskHeight/2.0 + fBottomThickness + fCavityHeight/2.0;
    new G4PVPlacement(0, G4ThreeVector(0., 0., cavityZ), fCavityLog, "CavityPhys", fCASTORBodyLog, false, 0, surfCheck);


    // --- STAINLESS STEEL LID ---
    // 3. Primary Lid
    G4Tubs* lid_solid = new G4Tubs("Lid", 0.*mm, fCaskInnerRadius, fLidThickness/2.0, 0.*deg, 360.*deg);
    fLidLog = new G4LogicalVolume(lid_solid, mat_Steel, "LidLog");
    fLidLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.6, 0.6, 0.7, alpha))); 
    // lid bottom aligned with top of helium cavity
    G4double lidZ = cavityZ + fCavityHeight/2.0 + fLidThickness/2.0;
    new G4PVPlacement(0, G4ThreeVector(0., 0., lidZ), fLidLog, "LidPhys", fCASTORBodyLog, false, 0, surfCheck);

    // --- SECOND IMPLEMENTATION
    // VVER-440 fuel assembly: 144 mm across flats, ~145 mm basket pitch
    const G4double assyApothem = 72.  * mm;   // -> 144 mm flat-to-flat
    const G4double pitch       = 145. * mm;   // 1 mm flat-to-flat clearance
    G4double zPlanes[2] = { -fActiveFuelLength/2.0, +fActiveFuelLength/2.0 };
    //G4double zPlanes[2] = { -fCavityHeight/2.0 + 100.*mm,
    //                        +fCavityHeight/2.0 - 100.*mm };
    G4double rInner [2] = { 0., 0. };
    G4double rOuter [2] = { assyApothem, assyApothem };
    // create the assembly solid
    G4Polyhedra* fuel_solid = new G4Polyhedra(
        "FuelAssy",
        30.*deg, 360.*deg,    // <-- phiStart = 30 deg: flats face neighbors
        6, 2, zPlanes, rInner, rOuter);
    // logical volume
    fFuelAssemblyLog = new G4LogicalVolume(fuel_solid, mat_Fuel, "FuelAssyLog");
    fFuelAssemblyLog->SetVisAttributes(new G4VisAttributes(true,
                                       G4Colour(1.0, 0.0, 0.0, alpha)));
    // placement
    G4int nPlaced = 0;
    for(size_t i=0; i<fFuelPositions.size(); ++i) {
        G4cout << " -> Placing a fuel assembly at (x,y) = (" << fFuelPositions[i].x() / mm << ", " << fFuelPositions[i].y() / mm << ")" << G4endl;
        new G4PVPlacement(nullptr, fFuelPositions.at(i),
                          fFuelAssemblyLog, "FuelPhys",
                          fCavityLog, false, nPlaced,
                          surfCheck);
        ++nPlaced;
    }
        
    // --- MODERATOR RODS DRILLED INTO CASK WALL ---
    // 5. Moderator Rods (Polyethylene array drilled into the cast iron wall)
    G4double rodRadius = 30. * mm;
    G4double rodRingRadius = 1050. * mm;
    G4Tubs* rod_solid = new G4Tubs("ModRod", 0.*mm, rodRadius, fCavityHeight/2.0, 0.*deg, 360.*deg);
    fModeratorRodLog = new G4LogicalVolume(rod_solid, mat_PE, "ModRodLog");
    fModeratorRodLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.0, 1.0, 1.0, alpha)));

    for (int i = 0; i < 60; ++i) {
        G4double angle = i * (360. * deg / 60.);
        G4double x = rodRingRadius * std::cos(angle);
        G4double y = rodRingRadius * std::sin(angle);
        new G4PVPlacement(0, G4ThreeVector(x, y, cavityZ), fModeratorRodLog, "ModRodPhys", fCASTORBodyLog, false, i, surfCheck);
    }
    
    ////--- NEW IMPLEMENTATION ---
    // 6. External Heat Dispersion Fins (annular cast iron lamellae stacked along z)
    //    Inner radius  = fCaskOuterRadius (body / fin base) = 1270 mm
    //    Outer radius  = fFinTipRadius                       = 1330 mm
    //    Radial span   = 60 mm  (matches paper: 2660 mm tip - 2540 mm base = 120 mm => 60 mm /side)
    const G4double finThickness = 10. * mm;
    const G4double finSpacing   = 50. * mm;     // 10 mm fin + 40 mm air gap
    G4Tubs* fin_solid = new G4Tubs("Fin",
                                   fCaskOuterRadius,    // = 1270 mm  (was 1330)
                                   fFinTipRadius,       // = 1330 mm  (was +50 mm)
                                   finThickness/2.0,
                                   0.*deg, 360.*deg);
    fFinLog = new G4LogicalVolume(fin_solid, mat_CastIron, "FinLog");
    fFinLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.2, 0.2, 0.2, alpha)));
    // Span the body region that surrounds the cavity (avoid the lid stack and bottom plate).
    // Top of fin region: cavityZ + cavityHeight/2  (just below the lid)
    // Bottom of fin region: cavityZ - cavityHeight/2  (just above the bottom plate)
    const G4double finRegionTop    = cavityZ + fCavityHeight/2.0;
    const G4double finRegionBottom = cavityZ - fCavityHeight/2.0;
    const G4double finRegionLength = finRegionTop - finRegionBottom;
    
    const G4int nFins = static_cast<G4int>(std::floor(finRegionLength / finSpacing));
    const G4double startZ = finRegionBottom + 0.5 * finSpacing;     // centred in the region
    
    for (G4int i = 0; i < nFins; ++i) {
        G4ThreeVector finPos(0., 0., startZ + i * finSpacing);
        fCASTORAssembly->AddPlacedVolume(fFinLog, finPos, rotate);
    }

    // Lock the core body into the assembly coordinate frame
    move = G4ThreeVector(0., 0., 0.);
    fCASTORAssembly->AddPlacedVolume(fCASTORBodyLog, move, rotate);

    G4cout << " -> Finished constructing GeometryCASTOR440!" << G4endl;
    return 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GeometryCASTOR440::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate, G4int copyNo) 
{
    G4bool surfCheck = true;
    fCASTORAssembly->MakeImprint(logic_world, move, rotate, copyNo, surfCheck);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GeometryCASTOR440::BuildMaterials() 
{
    G4NistManager* nist = G4NistManager::Instance();
    if (nist->FindOrBuildMaterial("CastIron", false) != nullptr) return;

    G4Element* el_Fe = nist->FindOrBuildElement("Fe");
    G4Element* el_C  = nist->FindOrBuildElement("C");
    G4Material* mat_CastIron = new G4Material("CastIron", 7.3 * g/cm3, 2);
    mat_CastIron->AddElement(el_Fe, 96.0 * perCent);
    mat_CastIron->AddElement(el_C,   4.0 * perCent);

    G4Element* el_U = nist->FindOrBuildElement("U");
    G4Element* el_O = nist->FindOrBuildElement("O");
    G4Material* mat_UO2 = new G4Material("UO2", 10.5 * g/cm3, 2);
    mat_UO2->AddElement(el_U, 1);
    mat_UO2->AddElement(el_O, 2);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GeometryCASTOR440::GenerateFuelPositions() {
    G4double pitch = 147.0 * mm;
    G4double zOffset = 0. * mm;

    const G4double dx = pitch;
    const G4double dy = pitch * std::sqrt(3.0) / 2.0;
    const G4int    R  = 5;

    fFuelPositions.clear();
    fFuelPositions.reserve(84);

    // Build a full 5-ring hex lattice (91 candidate positions) in axial (q,r) coords.
    for (G4int q = -R; q <= R; ++q) {
        const G4int r1 = std::max(-R, -q - R);
        const G4int r2 = std::min( R, -q + R);
        for (G4int r = r1; r <= r2; ++r) {
            const G4double x = dx * (q + r / 2.0);
            const G4double y = dy * r;

            // --- Exclusion rules ---
            // (a) Center position (empty in CASTOR 440/84 basket).
            if (std::hypot(x, y) < 1e-6) continue;

            // (b) Six corner positions of the outermost ring.
            const G4double rmax = R * pitch;
            if (std::abs(std::hypot(x, y) - rmax) < 1e-3) {
                G4double ang = std::atan2(y, x) * 180.0 / CLHEP::pi;
                if (ang < 0) ang += 360.0;
                bool isCorner = false;
                for (G4double a = 0.0; a < 360.0; a += 60.0) {
                    if (std::abs(ang - a) < 1.0) { isCorner = true; break; }
                }
                if (isCorner) continue;
            }

            fFuelPositions.emplace_back(x, y, zOffset);
        }
    }
    G4cout << " -> Generated " << (int)fFuelPositions.size() << " fuel positions" << G4endl;
}

G4ThreeVector GeometryCASTOR440::GetFuelPosition(G4int index) const {
    if (index >= 0 && index < fFuelPositions.size()) {
        return fFuelPositions[index];
    }
    return G4ThreeVector(0., 0., 0.);
}
