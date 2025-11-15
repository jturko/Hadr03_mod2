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

    std::map<Double_t, Int_t> particleMap;

    Double_t protonID = 2212;
    Double_t neutronID = 2112;
    Double_t gammaID = 22;
    Double_t electronID = 11;

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

    TFile* outfilehists = new TFile("histograms.root", "RECREATE");

    TH1D * hTimeN = new TH1D("hTimeN", "emission time of neutrons from catcher;#it{t} / ns;Counts",  1000, 0, 10);          hTimeN->SetLineColor(kBlack);
    TH1D * hTimeG = new TH1D("hTimeG", "emission time of gammas from catcher;#it{t} / ns;Counts",    1000, 0, 10);          hTimeG->SetLineColor(kRed);
    TH1D * hTimeE = new TH1D("hTimeE", "emission time of electrons from catcher;#it{t} / ns;Counts", 1000, 0, 10);          hTimeE->SetLineColor(kBlue);
    
    TH1D * hCosThetaN = new TH1D("hCosThetaN", "cos(#theta) distribution of neutrons from catcher;cos(#theta);Counts",  180,-1,1);         hCosThetaN->SetLineColor(kBlack);
    TH1D * hCosThetaG = new TH1D("hCosThetaG", "cos(#theta) distribution of gammas from catcher;cos(#theta);Counts",    180,-1,1);         hCosThetaG->SetLineColor(kRed);
    TH1D * hCosThetaE = new TH1D("hCosThetaE", "cos(#theta) distribution of electrons from catcher;cos(#theta);Counts", 180,-1,1);         hCosThetaE->SetLineColor(kBlue);
    
    TH1D * hAngDistN = new TH1D("hAngDistN", "angular distribution of neutrons from catcher;#theta / #circ;Counts",  180, 0, 180);         hAngDistN->SetLineColor(kBlack);
    TH1D * hAngDistG = new TH1D("hAngDistG", "angular distribution of gammas from catcher;#theta / #circ;Counts",    180, 0, 180);         hAngDistG->SetLineColor(kRed);
    TH1D * hAngDistE = new TH1D("hAngDistE", "angular distribution of electrons from catcher;#theta / #circ;Counts", 180, 0, 180);         hAngDistE->SetLineColor(kBlue);
    
    TH2D * hEkinCosThetaN = new TH2D("hEkinCosThetaN", "#it{E}_{kin} vs cos(#theta) distribution of neutrons from catcher;cos(#theta);#it{E}_{kin} / MeV;Counts",  180,-1,1, 2000, 0, 20); 
    TH2D * hEkinCosThetaG = new TH2D("hEkinCosThetaG", "#it{E}_{kin} vs cos(#theta) distribution of gammas from catcher;cos(#theta);#it{E}_{kin} / MeV;Counts",    180,-1,1, 2000, 0, 20); 
    TH2D * hEkinCosThetaE = new TH2D("hEkinCosThetaE", "#it{E}_{kin} vs cos(#theta) distribution of electrons from catcher;cos(#theta);#it{E}_{kin} / MeV;Counts", 180,-1,1, 2000, 0, 20); 
    
    TH2D * hTimeEkinN = new TH2D("hTimeEkinN", "emission time vs kinetic energy of neutrons from catcher;#it{E}_{kin} / MeV;#it{t} / ns;Counts", 2000, 0, 20, 1000, 0, 10);

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
    THnSparseD * hsparse2 = new THnSparseD("hsparse2", "hsparse2", nDim2, xBin2, xMin2, xMax2);

    for (Long64_t i = 0; i < nEntries_tree; ++i) {
        tree->GetEntry(i);
        // Process data for each entry
        
        particleMap[particle]++;

        TVector3 pos(x,y,z);
        TVector3 mom(px,py,pz);

        if(particle == neutronID) {
            hTimeN->Fill(t);
            hTimeEkinN->Fill(Ekin, t);
            hAngDistN->Fill(180./M_PI * mom.Theta(), abs(1./sin(mom.Theta())));
            hCosThetaN->Fill(cos(mom.Theta()));
            hEkinCosThetaN->Fill(cos(mom.Theta()), Ekin);

            double val[nDim] = { t, x, y, z, px, py, pz };
            hsparse->Fill(val);

            // only fill for theta < 45 deg
            if(mom.Theta() < M_PI/4.) {
                double val2[nDim] = { t, Ekin, mom.Theta() };
                hsparse2->Fill(val2);
            }
        }
        else if(particle == gammaID) {
            hTimeG->Fill(t);
            hAngDistG->Fill(180./M_PI * mom.Theta(), abs(1./sin(mom.Theta())));
            hCosThetaG->Fill(cos(mom.Theta()));
            hEkinCosThetaG->Fill(cos(mom.Theta()), Ekin);
        }
        else if(particle == electronID) {
            hTimeE->Fill(t);
            hAngDistE->Fill(180./M_PI * mom.Theta(), abs(1./sin(mom.Theta())));
            hCosThetaE->Fill(cos(mom.Theta()));
            hEkinCosThetaE->Fill(cos(mom.Theta()), Ekin);
        }
 
    }

    for(auto it : particleMap) {
        cout << "PID: " << Long64_t(it.first) << ", count: " << it.second << endl;
    }

    new TCanvas;
    hTimeN->Draw();
    hTimeG->Draw("same");
    hTimeE->Draw("same");
    gPad->SetLogy();
    
    new TCanvas;
    hAngDistN->Draw("hist");
    hAngDistG->Draw("hist same");
    hAngDistE->Draw("hist same");
    gPad->SetLogy();
    
    new TCanvas;
    hCosThetaN->Draw("hist");
    hCosThetaG->Draw("hist same");
    hCosThetaE->Draw("hist same");
    gPad->SetLogy();
    
    new TCanvas;
    hEkinCosThetaN->Draw("colz");
    new TCanvas;
    hEkinCosThetaG->Draw("colz");
    new TCanvas;
    hEkinCosThetaE->Draw("colz");

    //new TCanvas;
    //hTimeEkinN->Draw("colz");
    //// test sampling from THnSparseD
    //const int nSamples = 10000000;
    //TH2D * hTimeEkinN_sampled = new TH2D("hTimeEkinN_sampled", "sampled emission time vs kinetic energy of neutrons from catcher;#it{E}_{kin} / MeV;#it{t} / ns;Counts", 2000, 0, 20, 1000, 0, 10);
    //for(int i=0; i<nSamples; i++) {
    //    double val[nDim];
    //    hsparse->GetRandom(val);
    //    double Ek = (val[4]*val[4] + val[5]*val[5] + val[6]*val[6])/(2.*939);
    //    hTimeEkinN_sampled->Fill(Ek, val[0]);
    //}
    //new TCanvas;
    //hTimeEkinN_sampled->Draw("colz");

    TFile* outfile = new TFile("phase_space.root", "RECREATE");
    outfile->cd();
    hsparse->Write();
    hsparse2->Write();
    outfile->Close();

    outfilehists->Write();

    //file->Close();
}
