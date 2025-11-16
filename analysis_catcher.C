#include <TFile.h>
#include <TTree.h>
#include <iostream>

#include <libgen.h>
#include <filesystem>

std::string getParticleName(long pid, bool for_root=false);

void analysis_catcher(const char* inputFileName = "hadr03.root") {
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
    
    // particle IDs
    Double_t protonID = 2212;
    Double_t neutronID = 2112;
    Double_t gammaID = 22;
    Double_t electronID = 11;

    // maps from particle ID to counts / histograms
    std::map<Double_t, Int_t> particleCountMap;
    std::map<Double_t, TH2D*> particleHistEkinCosThetaMap;
    std::map<Double_t, TH1D*> particleHistTimeMap;

    // binning
    const int nbinEkin = 1000;
    double lbinEkin = 0., hbinEkin = 10.;

    const int nbinCosTheta = 360;
    double lbinCosTheta = -1., hbinCosTheta = 1.;
    
    const int nbinTime = 10000;
    double lbinTime = 0., hbinTime = 100.;

    // tree branches
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

    // histogram output file
    std::string histFileName = std::filesystem::path(inputFileName).stem().string() + "_histograms.root";
    TFile* outfilehists = new TFile(histFileName.c_str(), "RECREATE");

    // sparse histogram with neutron phase-space
    const int nDim = 7; // time,    x,    y,    z,    px,    py,   pz
    double xMin[nDim] = {     0,  -50,  -50,   49,  -300,  -300, -300 };  
    double xMax[nDim] = {    10,   50,   50,   53,   300,   300,  300 };  
    int    xBin[nDim] = {  1000, 1000, 1000, 1000,  1000,  1000, 1000 };
    //int    xBin[nDim] = {   100,  100,  100,  100,   100,   100,  100 };
    THnSparseD * hsparse = new THnSparseD("hsparse", "hsparse", nDim, xBin, xMin, xMax);
    
    // 2nd sparse histogram with limited neutron phase-space (time, ekin, theta), only in forward directions
    const int nDim2 = 3; // time,  Ekin, theta
    double xMin2[nDim] = {     0,  0,    0    };
    double xMax2[nDim] = {    10,  10,   M_PI };
    int    xBin2[nDim] = {   100,  100,  90   };
    //int    xBin2[nDim] = {  1000,  1000, 900   };
    THnSparseD * hsparse2 = new THnSparseD("hsparse2", "hsparse2", nDim2, xBin2, xMin2, xMax2);

    for (Long64_t i = 0; i < nEntries_tree; ++i) {
        if(i % 1000000 == 0)
            cout << " ---> entry " << i << " / " << nEntries_tree << " processed ... " << endl;

        tree->GetEntry(i);
        
        // check if ID found
        if(particleCountMap[particle] == 0) {
            // if not, initialize histogram
            particleHistEkinCosThetaMap[particle] = new TH2D(Form("hEkinCosTheta_%s", getParticleName(particle).c_str()), 
                                                             Form("hEkinCosTheta_%s;cos(#theta);#it{E} / MeV;Counts", getParticleName(particle).c_str()), 
                                                             nbinCosTheta,  lbinCosTheta,   hbinCosTheta,
                                                             nbinEkin,      lbinEkin,       hbinEkin);
            particleHistTimeMap        [particle] = new TH1D(Form("hTime_%s", getParticleName(particle).c_str()), 
                                                             Form("hTime_%s;#it{t} / ns;Counts", getParticleName(particle).c_str()), 
                                                             nbinTime,      lbinTime,       hbinTime);
        }

        // setup vectors
        TVector3 pos(x,y,z);
        TVector3 mom(px,py,pz);
        
        // fill maps
        particleCountMap[particle]++;
        particleHistEkinCosThetaMap[particle]->Fill(cos(mom.Theta()), Ekin);
        particleHistTimeMap[particle]->Fill(t);

        if(particle == neutronID) {
            // 7D neutron phase space
            double val[nDim] = { t, x, y, z, px, py, pz };
            hsparse->Fill(val);

            // 3D neutron phase space (only for particle emitted w. limited theta)
            //if(mom.Theta() < M_PI/4.) {
            if(mom.Theta() < M_PI/8.) {
            //if(1) {
                double val2[nDim] = { t, Ekin, mom.Theta() };
                hsparse2->Fill(val2);
            }
        }
    }

    // canvases
    TCanvas* cEkin = new TCanvas;
    TCanvas* cCosTheta = new TCanvas;
    TCanvas* cTime = new TCanvas;
    
    // color counter
    int counter = 0;

    // print particle IDs
    for(auto it : particleCountMap) {
        TString opts = (counter == 0) ? "hist e1" : "hist e1 same";

        cout << "PID: " << Long64_t(it.first) << ", name: " << getParticleName(it.first) << ", count: " << it.second << endl;
        if(it.second < 1000) 
            continue;

        cEkin->cd();
        auto h = particleHistEkinCosThetaMap[it.first]->ProjectionY();
        h->Scale(1./1e9);
        h->SetTitle(getParticleName(it.first, true).c_str());
        h->SetLineColor(counter+1);
        h->SetMarkerColor(counter+1);
        h->Draw(opts);
        
        cCosTheta->cd();
        h = particleHistEkinCosThetaMap[it.first]->ProjectionX();
        h->Scale(1./1e9);
        h->SetTitle(getParticleName(it.first, true).c_str());
        h->SetLineColor(counter+1);
        h->SetMarkerColor(counter+1);
        h->Draw(opts);
        
        cTime->cd();
        h = particleHistTimeMap[it.first];
        h->Scale(1./1e9);
        h->SetTitle(getParticleName(it.first, true).c_str());
        h->SetLineColor(counter+1);
        h->SetMarkerColor(counter+1);
        h->Draw(opts);

        counter++;
    }

    // particle count hist
    const int nparticles = particleCountMap.size();
    TH1F* hParticle = new TH1F("hParticle","hParticle",nparticles,0,nparticles);
    counter = 0;
    for(auto it : particleCountMap) {
        hParticle->SetBinContent(counter+1, it.second/1e9);
        hParticle->GetXaxis()->SetBinLabel(counter+1, getParticleName(it.first,true).c_str());
        counter++;
    } 
    new TCanvas;
    hParticle->Draw();

    // save phase space
    std::string phaseFileName = std::filesystem::path(inputFileName).stem().string() + "_phase.root";
    TFile* outfile = new TFile(phaseFileName.c_str(), "RECREATE");
    outfile->cd();
    hsparse->Write();
    hsparse2->Write();
    outfile->Close();

    // write histograms file
    outfilehists->Write();
}




#include <string>
#include <unordered_map>
#include <cmath>

std::string getParticleName(long pid, bool for_root) {
    // Map for common particles (you can extend this as needed)
    static const std::unordered_map<long, std::string> commonParticles = {
        {11, "electron"},
        {-11, "positron"},
        {12, "nu_e"},
        {-12, "anti_nu_e"},
        {13, "mu-"},
        {-13, "mu+"},
        {14, "nu_mu"},
        {-14, "anti_nu_mu"},
        {15, "tau-"},
        {-15, "tau+"},
        {16, "nu_tau"},
        {-16, "anti_nu_tau"},
        {22, "gamma"},
        {111, "pi0"},
        {211, "pi+"},
        {-211, "pi-"},
        {130, "K_L0"},
        {310, "K_S0"},
        {311, "K0"},
        {321, "K+"},
        {-321, "K-"},
        {2112, "neutron"},
        {2212, "proton"},
        {-2212, "anti_proton"},
        {3122, "Lambda"},
        {3222, "Sigma+"},
        {3212, "Sigma0"},
        {3112, "Sigma-"},
        {3322, "Xi0"},
        {3312, "Xi-"},
        {3334, "Omega-"}
    };

    // Check if it's a common particle
    auto it = commonParticles.find(pid);
    if (it != commonParticles.end()) {
        return it->second;
    }

    // Handle ions (PDG numbering scheme for nuclei: 10LZZZAAAI)
    // For ions: ±10LZZZAAAI, where L=0, ZZZ=Z, AAA=A, I=isomer level
    if (std::abs(pid) > 1000000000) {
        long absPid = std::abs(pid);

        // Extract Z and A from PDG code: ±10LZZZAAAI
        int Z = (absPid / 10000) % 1000;
        int A = (absPid / 10) % 1000;

        // Get element symbol from Z
        static const std::string elements[] = {
            "n", "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
            "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
            "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
            "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr",
            "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
            "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",
            "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
            "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg",
            "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
            "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm",
            "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds",
            "Rg", "Cn", "Nh", "Fl", "Mc", "Lv", "Ts", "Og"
        };

        std::string symbol;
        if (Z < static_cast<int>(sizeof(elements)/sizeof(elements[0]))) {
            symbol = elements[Z];
        } else {
            symbol = "UnknownElement";
        }

        if(for_root)
            return "^{" + std::to_string(A) + "}" + symbol;
        else
            return symbol + "" + std::to_string(A);
    }

    // Unknown particle
    return "unknown_" + std::to_string(pid);
}

