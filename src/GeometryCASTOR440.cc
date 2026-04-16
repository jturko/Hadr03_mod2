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
    fCaskOuterRadius = 1330. * mm;
    fCaskInnerRadius = 800. * mm;
    fCaskHeight      = 4080. * mm;
    fCavityHeight    = 3500. * mm;
    fLidThickness    = 290. * mm;

    //fCastIronMatName = "G4_AIR";
    fCastIronMatName = "CastIron";
    
    fHeliumMatName   = "G4_He";
    fPEMatName       = "G4_POLYETHYLENE";
    fFuelMatName     = "UO2";

    //fSteelMatName    = "G4_AIR";
    fSteelMatName    = "G4_STAINLESS-STEEL";
}

GeometryCASTOR440::~GeometryCASTOR440() {}


// in the x-z plane:
// ^ (+x)
// > (+y)

//               68
//           62      67
//       55      61      66
//   47      54      60      65
//       46      53      59      64
//   38      45      52      58      63
//       37      44      51      57
//   29      36      43      50      56
//       28      35      42      49
//   20      27      34      41      48
//       19      26      33      40
//   12      18      25      32      39
//       11      17      24      31
//   05      10      16      23      30
//       04      09      15      22
//           03      08      14      21
//               02      07      13
//                   01      06
//                       00

G4int GeometryCASTOR440::Build()
{
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

    // 1. Cask Body (Main Mother Volume)
    G4Tubs* body_solid = new G4Tubs("CastorBody", 0.*mm, fCaskOuterRadius, fCaskHeight/2.0, 0.*deg, 360.*deg);
    fCASTORBodyLog = new G4LogicalVolume(body_solid, mat_CastIron, "CastorBodyLog");
    fCASTORBodyLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.3, 0.3, 0.3, alpha)));

    // 2. Cavity (Filled with Helium)
    G4Tubs* cavity_solid = new G4Tubs("Cavity", 0.*mm, fCaskInnerRadius, fCavityHeight/2.0, 0.*deg, 360.*deg);
    fCavityLog = new G4LogicalVolume(cavity_solid, mat_He, "CavityLog");
    fCavityLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.9, 0.9, 0.9, alpha)));
    
    // Shifted downwards to leave space for the massive steel lid at the top
    G4double cavityZ = -150. * mm;
    new G4PVPlacement(0, G4ThreeVector(0., 0., cavityZ), fCavityLog, "CavityPhys", fCASTORBodyLog, false, 0);

    // 3. Primary Lid
    G4Tubs* lid_solid = new G4Tubs("Lid", 0.*mm, fCaskInnerRadius, fLidThickness/2.0, 0.*deg, 360.*deg);
    fLidLog = new G4LogicalVolume(lid_solid, mat_Steel, "LidLog");
    fLidLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.6, 0.6, 0.7, alpha))); 

    G4double lidZ = cavityZ + fCavityHeight/2.0 + fLidThickness/2.0;
    new G4PVPlacement(0, G4ThreeVector(0., 0., lidZ), fLidLog, "LidPhys", fCASTORBodyLog, false, 0);

    // 4. Fuel Assemblies (84 VVER-440 hexagonal assemblies)
    G4double zPlanes[2] = { -fCavityHeight/2.0 + 100.*mm, fCavityHeight/2.0 - 100.*mm };
    G4double rInner[2] = { 0.*mm, 0.*mm };
    G4double rOuter[2] = { 70.*mm, 70.*mm }; // Approx 144 mm across flats
    G4Polyhedra* fuel_solid = new G4Polyhedra("FuelAssy", 0.*deg, 360.*deg, 6, 2, zPlanes, rInner, rOuter);
    fFuelAssemblyLog = new G4LogicalVolume(fuel_solid, mat_Fuel, "FuelAssyLog");
    fFuelAssemblyLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(1.0, 0.0, 0.0, alpha)));

    G4int nPlaced = 0;
    G4double pitch = 150. * mm; // Standard VVER-440 pitch
    for (int i = -4; i <= 4; ++i) {
        for (int j = -4; j <= 4; ++j) {
            G4double x = pitch * (i + j / 2.0);
            G4double y = pitch * j * std::sqrt(3.0) / 2.0;
            // Bound inside the basket and cap exactly at 84 assemblies
            if (std::sqrt(x*x + y*y) < 700. * mm) {
                if (nPlaced < 84) {
                    fFuelPositions.push_back(G4ThreeVector(x, y, 0.));
                    new G4PVPlacement(0, G4ThreeVector(x, y, 0.), fFuelAssemblyLog, "FuelPhys", fCavityLog, false, nPlaced);
                    nPlaced++;
                }
            }
        }
    }

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
        new G4PVPlacement(0, G4ThreeVector(x, y, cavityZ), fModeratorRodLog, "ModRodPhys", fCASTORBodyLog, false, i);
    }

    // 6. External Heat Dispersion Fins
    G4double finThickness = 10. * mm;
    G4double finOuterRadius = fCaskOuterRadius + 50. * mm;
    G4Tubs* fin_solid = new G4Tubs("Fin", fCaskOuterRadius, finOuterRadius, finThickness/2.0, 0.*deg, 360.*deg);
    fFinLog = new G4LogicalVolume(fin_solid, mat_CastIron, "FinLog");
    fFinLog->SetVisAttributes(new G4VisAttributes(true, G4Colour(0.2, 0.2, 0.2, alpha)));

    G4double startZ = -1500. * mm;
    G4double finSpacing = 50. * mm;
    for (int i = 0; i < 60; ++i) {
        G4ThreeVector finPos(0., 0., startZ + i * finSpacing);
        fCASTORAssembly->AddPlacedVolume(fFinLog, finPos, rotate);
    }

    // Lock the core body into the assembly coordinate frame
    move = G4ThreeVector(0., 0., 0.);
    fCASTORAssembly->AddPlacedVolume(fCASTORBodyLog, move, rotate);

    return 1;
}

void GeometryCASTOR440::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate, G4int copyNo) 
{
    G4bool surfCheck = true;
    fCASTORAssembly->MakeImprint(logic_world, move, rotate, copyNo, surfCheck);
}

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

G4ThreeVector GeometryCASTOR440::GetFuelPosition(G4int index) const {
    if (index >= 0 && index < fFuelPositions.size()) {
        return fFuelPositions[index];
    }
    return G4ThreeVector(0., 0., 0.);
}
