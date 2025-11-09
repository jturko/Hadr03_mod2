#include <TFile.h>
#include <TTree.h>
#include <iostream>

void analysis_script(const char* inputFileName = "hadr03.root") {
    TFile* file = TFile::Open(inputFileName);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << inputFileName << std::endl;
        return;
    }

    TTree* tree = (TTree*)file->Get("tree");
    if (!tree) {
        std::cerr << "Error: Could not retrieve TTree 'tree'" << std::endl;
        return;
    }

    Double_t neutronID = 2112;
    Double_t gammaID = 22;

    Double_t particle;
    Double_t Ekin;
    Double_t t;
    Double_t x;
    Double_t y;
    Double_t z;
    Double_t px;
    Double_t py;
    Double_t pz;
    Long64_t nEntries_tree = tree->GetEntries();
    std::cout << "Processing TTree: tree with " << nEntries_tree << " entries." << std::endl;

    // Set branch addresses
    tree->SetBranchAddress("particle", &particle);
    tree->SetBranchAddress("Ekin", &Ekin);
    tree->SetBranchAddress("t", &t);
    tree->SetBranchAddress("x", &x);
    tree->SetBranchAddress("y", &y);
    tree->SetBranchAddress("z", &z);
    tree->SetBranchAddress("px", &px);
    tree->SetBranchAddress("py", &py);
    tree->SetBranchAddress("pz", &pz);

    TH1D * hTimeN = new TH1D("hTimeN", "emission time of neutrons from catcher;#it{t} / ns;Counts", 1000, 0, 10);
    TH1D * hTimeG = new TH1D("hTimeG", "emission time of gammas from catcher;#it{t} / ns;Counts", 1000, 0, 10);         hTimeG->SetLineColor(kRed);
    
    TH2D * hTimeEkinN = new TH2D("hTimeEkinN", "emission time vs kinetic energy of neutrons from catcher;#it{E}_{kin} / MeV;#it{t} / ns;Counts", 2000, 0, 20, 1000, 0, 10);

    // sparse histogram with neutron phase-space
    const int nDim = 7; // time,    x,    y,    z,    px,    py,   pz
    double xMin[nDim] = {     0,  -50,  -50,   -5,  -300,  -300, -300 };  
    double xMax[nDim] = {    10,   50,   50,    5,   300,   300,  300 };  
    int    xBin[nDim] = {   100,  100,  100,  100,   100,   100,  100 };
    THnSparseD * hsparse = new THnSparseD("hsparse", "hsparse", nDim, xBin, xMin, xMax);

    for (Long64_t i = 0; i < nEntries_tree; ++i) {
        tree->GetEntry(i);
        // Process data for each entry
        
        if(particle == neutronID) {
            hTimeN->Fill(t);
            hTimeEkinN->Fill(Ekin, t);
            
            double val[nDim] = { t, x, y, z, px, py, pz };
            hsparse->Fill(val);
        }
        else if(particle == gammaID) {
            hTimeG->Fill(t);
        }
 
    }

    new TCanvas;
    hTimeN->Draw();
    hTimeG->Draw("same");
    
    new TCanvas;
    hTimeEkinN->Draw("colz");

    // test sampling from THnSparseD
    const int nSamples = 1000000;
    TH2D * hTimeEkinN_sampled = new TH2D("hTimeEkinN_sampled", "sampled emission time vs kinetic energy of neutrons from catcher;#it{E}_{kin} / MeV;#it{t} / ns;Counts", 2000, 0, 20, 1000, 0, 10);
    for(int i=0; i<nSamples; i++) {
        double val[nDim];
        hsparse->GetRandom(val);
        double Ek = (val[4]*val[4] + val[5]*val[5] + val[6]*val[6])/(2.*939);
        hTimeEkinN_sampled->Fill(Ek, val[0]);
    }
    new TCanvas;
    hTimeEkinN_sampled->Draw("colz");

    //file->Close();
}
