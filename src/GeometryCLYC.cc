// 1. Geant4 Headers First (Ensures G4RotationMatrix/G4ThreeVector are declared)
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4NistManager.hh"

#include "G4Tubs.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4GenericPolycone.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4SubtractionSolid.hh"
#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4AssemblyVolume.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4SystemOfUnits.hh"

// 2. Class Header after Geant4 Dependencies
#include "GeometryCLYC.hh"
#include <string>

GeometryCLYC::GeometryCLYC() :
    fCLYCAssembly(NULL),
    fCLYCCrystalLog(NULL),
    fAlumCasingLog(NULL),
    fPbCollimatorLog(NULL),
    fPEHDCollimatorLog(NULL),
    fPEPlugLog(NULL)
{
    fCLYCCrystalRadius = 25./2 * mm;
    fCLYCCrystalLength = 25. * mm;
    fAlumCasingThickness = 0.5 * mm;
    
    fPbCollimatorInnerRadius = 29./2. * mm;
    fPbCollimatorOuterRadius = 40./2. * mm;
    fPbCollimatorLength = 50. * mm;
    
    fPEHDCollimatorInnerRadius = 40./2. * mm;
    fPEHDCollimatorOuterRadius = 50./2. * mm;
    fPEHDCollimatorLength = 50. * mm;

    fPEPlugInnerRadius = 29./2. * mm;
    fPEPlugOuterRadius = 50./2. * mm;
    fPEPlugLength = 45. * mm;
    fPEPlugFaceThickness = 10. * mm; 

    fCLYCCrystalMatName = "CLYC";
    fAlumMatName = "G4_Al";
    fPbMatName = "G4_Pb";
    fPEHDMatName = "G4_POLYETHYLENE";

    G4double alpha = 1.0; 
    fCLYCCrystalColour = G4Colour(0.0, 1.0, 0.0, alpha); // Green
    fAlumColour        = G4Colour(0.5, 0.5, 0.5, alpha); // Grey
    fPbColour          = G4Colour(0.6, 0.4, 0.2, alpha); // Brown
    fPEHDColour        = G4Colour(0.0, 1.0, 1.0, alpha); // Cyan
    fPEPlugColour      = G4Colour(1.0, 1.0, 0.0, alpha); // Yellow

    //fCLYCCrystalColour = G4Colour::Green();
    //fAlumColour = G4Colour::Grey();
    //fPbColour = G4Colour::Brown();
    //fPEHDColour = G4Colour::Cyan();
    //fPEPlugColour = G4Colour::Yellow();
}

GeometryCLYC::~GeometryCLYC() {}



// CLYC crystal centered with HDPE collimator rear face
G4int GeometryCLYC::Build()
{
    BuildMaterials();
    fCLYCAssembly = new G4AssemblyVolume();
    
    G4double startPhi = 0.0*deg, endPhi = 360.0/2.*deg;
    G4ThreeVector move;
    G4RotationMatrix* rotate = NULL;
    G4bool onlyBuildCLYC = true;

    G4NistManager* manager = G4NistManager::Instance();
    G4Material* CLYC_material = manager->FindOrBuildMaterial(fCLYCCrystalMatName);
    G4Material* Alum_material = manager->FindOrBuildMaterial(fAlumMatName);
    G4Material* Pb_material = manager->FindOrBuildMaterial(fPbMatName);
    G4Material* PEHD_material = manager->FindOrBuildMaterial(fPEHDMatName);

    // 1. PE Plug
    // Front face sits exactly at Z=0, projecting backwards.
    std::vector<G4double> rPlug = { 0.*mm, fPEPlugOuterRadius, fPEPlugOuterRadius, fPEPlugInnerRadius, fPEPlugInnerRadius, 0.*mm };
    std::vector<G4double> zPlug = { 0.*mm, 0.*mm, -fPEPlugFaceThickness, -fPEPlugFaceThickness, -fPEPlugLength, -fPEPlugLength };

    G4GenericPolycone* PE_plug_solid = new G4GenericPolycone("PE_plug_solid", startPhi, endPhi, rPlug.size(), rPlug.data(), zPlug.data());
    fPEPlugLog = new G4LogicalVolume(PE_plug_solid, PEHD_material, "PEPlugLog", 0, 0, 0);
    fPEPlugLog->SetVisAttributes(new G4VisAttributes(true, fPEPlugColour));

    move = G4ThreeVector(0., 0., 0.);
    if(!onlyBuildCLYC)
        fCLYCAssembly->AddPlacedVolume(fPEPlugLog, move, rotate);

    // 2. Collimators (PEHD and Pb)
    // Front faces aligned with the back of the plug lip (Z = -fPEPlugFaceThickness).
    // Tubs are defined from their center, so we shift by half their length.
    G4double pehdCenterZ = -fPEPlugFaceThickness - (fPEHDCollimatorLength / 2.0);
    G4Tubs* PEHD_collimator_solid = new G4Tubs("PEHD_collimator_solid", fPEHDCollimatorInnerRadius, fPEHDCollimatorOuterRadius, fPEHDCollimatorLength/2., startPhi, endPhi);
    fPEHDCollimatorLog = new G4LogicalVolume(PEHD_collimator_solid, PEHD_material, "PEHDCollimatorLog", 0, 0, 0);
    fPEHDCollimatorLog->SetVisAttributes(new G4VisAttributes(true, fPEHDColour));

    move = G4ThreeVector(0., 0., pehdCenterZ);
    if(!onlyBuildCLYC)
        fCLYCAssembly->AddPlacedVolume(fPEHDCollimatorLog, move, rotate);

    G4double pbCenterZ = -fPEPlugFaceThickness - (fPbCollimatorLength / 2.0);
    G4Tubs* Pb_collimator_solid = new G4Tubs("Pb_collimator_solid", fPbCollimatorInnerRadius, fPbCollimatorOuterRadius, fPbCollimatorLength/2., startPhi, endPhi);
    fPbCollimatorLog = new G4LogicalVolume(Pb_collimator_solid, Pb_material, "PbCollimatorLog", 0, 0, 0);
    fPbCollimatorLog->SetVisAttributes(new G4VisAttributes(true, fPbColour));

    move = G4ThreeVector(0., 0., pbCenterZ);
    if(!onlyBuildCLYC)
        fCLYCAssembly->AddPlacedVolume(fPbCollimatorLog, move, rotate);

    // 3. CLYC Crystal and Aluminum Casing
    // Front face of the Aluminum Casing is flush with the rear face of the entire plug.
    G4double L = fCLYCCrystalLength;
    G4double R = fCLYCCrystalRadius;
    G4double t = fAlumCasingThickness;

    // The plug ends at -fPEPlugLength. We place the front of the casing exactly there.
    G4double crystalCenterZ = -fPEPlugLength - t - (L / 2.0);

    G4Tubs* CLYC_solid = new G4Tubs("CLYC_solid", 0.*mm, R, L/2.0, startPhi, endPhi);
    fCLYCCrystalLog = new G4LogicalVolume(CLYC_solid, CLYC_material, "CLYCCrystalLog", 0, 0, 0);
    fCLYCCrystalLog->SetVisAttributes(new G4VisAttributes(true, fCLYCCrystalColour));

    move = G4ThreeVector(0., 0., crystalCenterZ);
    fCLYCAssembly->AddPlacedVolume(fCLYCCrystalLog, move, rotate);

    // Aluminum casing overlaps the sides and FRONT face (+Z) of the crystal
    std::vector<G4double> rCasing = { 0.*mm, R + t, R + t, R, R, 0.*mm };
    std::vector<G4double> zCasing = { +(L/2. + t), +(L/2. + t), -L/2., -L/2., +L/2., +L/2. };

    G4GenericPolycone* Alum_solid = new G4GenericPolycone("Alum_solid", startPhi, endPhi, rCasing.size(), rCasing.data(), zCasing.data());
    fAlumCasingLog = new G4LogicalVolume(Alum_solid, Alum_material, "AlumCasingLog", 0, 0, 0);
    fAlumCasingLog->SetVisAttributes(new G4VisAttributes(true, fAlumColour));

    move = G4ThreeVector(0., 0., crystalCenterZ);
    if(!onlyBuildCLYC)
        fCLYCAssembly->AddPlacedVolume(fAlumCasingLog, move, rotate);

    return 1;
}

//G4int GeometryCLYC::Build()
//{
//    BuildMaterials();
//    fCLYCAssembly = new G4AssemblyVolume();
//    
//    G4double startPhi = 0.0*deg, endPhi = 360.0/2.*deg;
//    G4ThreeVector move;
//    G4RotationMatrix* rotate = NULL;
//
//    G4NistManager* manager = G4NistManager::Instance();
//    G4Material* CLYC_material = manager->FindOrBuildMaterial(fCLYCCrystalMatName);
//    G4Material* Alum_material = manager->FindOrBuildMaterial(fAlumMatName);
//    G4Material* Pb_material = manager->FindOrBuildMaterial(fPbMatName);
//    G4Material* PEHD_material = manager->FindOrBuildMaterial(fPEHDMatName);
//
//    // Assembly Alignment Logic:
//    // Crystal native center is 0. Collimators are mounted at +Length/2 relative to crystal.
//    // PE Plug originally mounted forward of the crystal. We want the flat face of the PE Plug 
//    // to sit precisely at Z=0 in the final assembly frame.
//    // Original Z position of plug face relative to crystal center: fPEPlugFaceThickness - fCLYCCrystalLength/2.0 + fPEHDCollimatorLength.
//    // Shift vector applied to crystal and collimators to pull them backwards behind Z=0.
//    G4double zShift = -(fPEPlugFaceThickness - fCLYCCrystalLength/2.0 + fPEHDCollimatorLength);
//    G4ThreeVector zShiftVec(0., 0., zShift);
//
//    // 1. CLYC Crystal
//    G4Tubs* CLYC_solid = new G4Tubs("CLYC_solid", 0.*mm, fCLYCCrystalRadius, fCLYCCrystalLength/2.0, startPhi, endPhi);
//    fCLYCCrystalLog = new G4LogicalVolume(CLYC_solid, CLYC_material, "CLYCCrystalLog", 0, 0, 0);
//    fCLYCCrystalLog->SetVisAttributes(new G4VisAttributes(true, fCLYCCrystalColour));
//    
//    move = G4ThreeVector(0., 0., -fCLYCCrystalLength/2.0) + zShiftVec;
//    fCLYCAssembly->AddPlacedVolume(fCLYCCrystalLog, move, rotate);
//
//    // 2. Aluminum Casing (Polycone wraps back and sides of crystal)
//    G4double L = fCLYCCrystalLength;
//    G4double R = fCLYCCrystalRadius;
//    G4double t = fAlumCasingThickness;
//    std::vector<G4double> rCasing = { 0.*mm, R + t, R + t, R, R, 0.*mm };
//    std::vector<G4double> zCasing = { +(L/2. + t), +(L/2. + t), -L/2., -L/2., +L/2., +L/2. };
//    
//    G4GenericPolycone* Alum_solid = new G4GenericPolycone("Alum_solid", startPhi, endPhi, rCasing.size(), rCasing.data(), zCasing.data());
//    fAlumCasingLog = new G4LogicalVolume(Alum_solid, Alum_material, "AlumCasingLog", 0, 0, 0);
//    fAlumCasingLog->SetVisAttributes(new G4VisAttributes(true, fAlumColour));
//
//    move = G4ThreeVector(0., 0., -fCLYCCrystalLength/2.0) + zShiftVec;
//    fCLYCAssembly->AddPlacedVolume(fAlumCasingLog, move, rotate);
//
//    // 3. Pb Collimator
//    G4Tubs* Pb_collimator_solid = new G4Tubs("Pb_collimator_solid", fPbCollimatorInnerRadius, fPbCollimatorOuterRadius, fPbCollimatorLength/2., startPhi, endPhi);
//    fPbCollimatorLog = new G4LogicalVolume(Pb_collimator_solid, Pb_material, "PbCollimatorLog", 0, 0, 0);
//    fPbCollimatorLog->SetVisAttributes(new G4VisAttributes(true, fPbColour));
//    
//    move = G4ThreeVector(0., 0., +fCLYCCrystalLength/2.) + zShiftVec;
//    fCLYCAssembly->AddPlacedVolume(fPbCollimatorLog, move, rotate);
//
//    // 4. PEHD Collimator
//    G4Tubs* PEHD_collimator_solid = new G4Tubs("PEHD_collimator_solid", fPEHDCollimatorInnerRadius, fPEHDCollimatorOuterRadius, fPEHDCollimatorLength/2., startPhi, endPhi);
//    fPEHDCollimatorLog = new G4LogicalVolume(PEHD_collimator_solid, PEHD_material, "PEHDCollimatorLog", 0, 0, 0);
//    fPEHDCollimatorLog->SetVisAttributes(new G4VisAttributes(true, fPEHDColour));
//    
//    move = G4ThreeVector(0., 0., +fCLYCCrystalLength/2.) + zShiftVec;
//    fCLYCAssembly->AddPlacedVolume(fPEHDCollimatorLog, move, rotate);
//
//    // 5. PE Plug
//    // Profile is constructed starting exactly at Z=0 and projecting into the -Z direction
//    std::vector<G4double> rPlug = { 0.*mm, fPEPlugOuterRadius, fPEPlugOuterRadius, fPEPlugInnerRadius, fPEPlugInnerRadius, 0.*mm };
//    std::vector<G4double> zPlug = { 0.*mm, 0.*mm, -fPEPlugFaceThickness, -fPEPlugFaceThickness, -fPEPlugLength, -fPEPlugLength };
//    
//    G4GenericPolycone* PE_plug_solid = new G4GenericPolycone("PE_plug_solid", startPhi, endPhi, rPlug.size(), rPlug.data(), zPlug.data());
//    fPEPlugLog = new G4LogicalVolume(PE_plug_solid, PEHD_material, "PEPlugLog", 0, 0, 0);
//    fPEPlugLog->SetVisAttributes(new G4VisAttributes(true, fPEPlugColour));
//    
//    // No translation needed; it sits exactly at assembly origin
//    move = G4ThreeVector(0., 0., 0.);
//    fCLYCAssembly->AddPlacedVolume(fPEPlugLog, move, rotate);
//
//    return 1;
//}

void GeometryCLYC::PlaceDetector(G4LogicalVolume* logic_world, G4ThreeVector move, G4RotationMatrix* rotate, G4int copyNo) 
{
    G4bool surfCheck = true;
    fCLYCAssembly->MakeImprint(logic_world, move, rotate, copyNo, surfCheck);
}

void GeometryCLYC::BuildMaterials() 
{
    G4NistManager* nist = G4NistManager::Instance();
    
    // Short-circuit if custom material already exists
    if (nist->FindOrBuildMaterial("CLYC") != nullptr) {
        return;
    }

    G4Material* mat_Pb = nist->FindOrBuildMaterial(fPbMatName);
    G4Material* mat_PE_HD = nist->FindOrBuildMaterial(fPEHDMatName);
    G4Material* mat_Al = nist->FindOrBuildMaterial(fAlumMatName);

    G4Isotope* iso_Li6 = new G4Isotope("Li6", 3, 6, 6.015 * g/mole);
    G4Isotope* iso_Li7 = new G4Isotope("Li7", 3, 7, 7.016 * g/mole);

    G4Element* el_Li_enr = new G4Element("Enriched Lithium", "Li", 2);
    el_Li_enr->AddIsotope(iso_Li6, 95.0 * perCent);
    el_Li_enr->AddIsotope(iso_Li7, 5.0 * perCent);

    G4Element* el_Cs = nist->FindOrBuildElement("Cs");
    G4Element* el_Y  = nist->FindOrBuildElement("Y");
    G4Element* el_Cl = nist->FindOrBuildElement("Cl");

    G4Material* mat_CLYC = new G4Material("CLYC", 3.31 * g/cm3, 4);
    mat_CLYC->AddElement(el_Cs,     2);
    mat_CLYC->AddElement(el_Li_enr, 1);
    mat_CLYC->AddElement(el_Y,      1);
    mat_CLYC->AddElement(el_Cl,     6);   
}
