#include <TFile.h>
#include <TTree.h>
#include <iostream>

#include "root_utils.C"

void analysis_image(const char* inputFileName = "tmp_sum.root") {
    gStyle->SetPalette(kGreyScale);

    TFile* file = TFile::Open(inputFileName);
    if (!file || file->IsZombie()) {
        std::cerr << "Error: Could not open file " << inputFileName << std::endl;
        return;
    }
    TTree* hits = (TTree*)file->Get("hits");
    if (!hits) {
        std::cerr << "Error: Could not retrieve TTree 'hits'" << std::endl;
        return;
    }

    Double_t particle;
    Double_t Edep;
    Double_t t;
    Double_t x;
    Double_t y;
    Double_t z;
    Long64_t nEntries_hits = hits->GetEntries();
    std::cout << "Processing TTree: hits with " << nEntries_hits << " entries." << std::endl;

    // Set branch addresses
    hits->SetBranchAddress("particle", &particle);
    hits->SetBranchAddress("Edep", &Edep);
    hits->SetBranchAddress("t", &t);
    hits->SetBranchAddress("x", &x);
    hits->SetBranchAddress("y", &y);
    hits->SetBranchAddress("z", &z);

    // binning
    const int nbin_image = 100;
    double lbin_image = -50, hbin_image = 50;
    
    const int nbin_TOF = 700;
    double lbin_TOF = 0, hbin_TOF = 700;

    // cuts
    const int npoint_cut = 5;
    const int n_cut = 2;
    TCutG* cuts[n_cut];
    vector<double> pixels[n_cut];
    std::string names[n_cut] = { "samp", "bkgd" };
    double averages[n_cut];
    double std_devs[n_cut];
    int colours[n_cut] = { 2, 4 };
    // sample
    double point_cut_samp_x[npoint_cut] = { 5, 15, 15, 5,  5 };
    double point_cut_samp_y[npoint_cut] = { 5, 5,  15, 15, 5 };
    TCutG * cut_samp = new TCutG("cut_samp", npoint_cut, point_cut_samp_x, point_cut_samp_y);
    cut_samp->SetLineColor(kRed);
    cut_samp->SetLineWidth(2);
    cuts[0] = cut_samp;
    // bkgd
    double point_cut_bkgd_x[npoint_cut] = { -15, -5, -5, -15, -15 };
    double point_cut_bkgd_y[npoint_cut] = {   5,  5, -5, -5,    5 };
    TCutG * cut_bkgd = new TCutG("cut_bkgd", npoint_cut, point_cut_bkgd_x, point_cut_bkgd_y);
    cut_bkgd->SetLineColor(kBlue);
    cut_bkgd->SetLineWidth(2);
    cuts[1] = cut_bkgd;
    
    // histograms
    TH2D* hImage        = new TH2D("hImage", "hImage", nbin_image, lbin_image, hbin_image, nbin_image, lbin_image, hbin_image);

    TH1D* hTOF_all = new TH1D("hTOF_all", "hTOF_all", nbin_TOF, lbin_TOF, hbin_TOF);        hTOF_all->SetLineColor(kBlack);
    TH1D* hTOF_cut[n_cut];
    for(int i_cut=0; i_cut<n_cut; i_cut++) {
        hTOF_cut[i_cut] = new TH1D(Form("hTOF_cut_%s", names[i_cut].c_str()), Form("hTOF_cut_%s", names[i_cut].c_str()), nbin_TOF, lbin_TOF, hbin_TOF);
        hTOF_cut[i_cut]->SetLineColor(colours[i_cut]);
    }

    // loop over hits TTree
    for (Long64_t i = 0; i < nEntries_hits; ++i) {
        hits->GetEntry(i);
        
        // image
        hImage->Fill(x,y);

        // all
        hTOF_all->Fill(t);

        // cut
        for(int i_cut=0; i_cut<n_cut; i_cut++) {
            if(cuts[i_cut]->IsInside(x,y)) {
                hTOF_cut[i_cut]->Fill(t);
            }
        }
    }

    // loop over image pixels
    for(int ix = 0; ix < nbin_image+1; ix++) {
        for(int iy = 0; iy < nbin_image+1; iy++) {
            double xx = hImage->GetXaxis()->GetBinCenter(ix);
            double yy = hImage->GetYaxis()->GetBinCenter(iy);
            
            for(int i_cut=0; i_cut<n_cut; i_cut++) {
                if(cuts[i_cut]->IsInside(xx,yy)) {
                    pixels[i_cut].push_back(hImage->GetBinContent(ix,iy));
                }
            }
        }
    }

    // calculate values
    for(int i_cut=0; i_cut<n_cut; i_cut++) {
        cout << names[i_cut] << flush;
        
        // average
        averages[i_cut] = 0;
        for(auto it : pixels[i_cut]) {
            averages[i_cut] += it;
        }
        averages[i_cut] /= double(pixels[i_cut].size());
        cout << ",\t avg: " << averages[i_cut] << flush;

        // std dev
        std_devs[i_cut] = 0;
        for(auto it : pixels[i_cut]) {
            std_devs[i_cut] += (it - averages[i_cut]) * (it -averages[i_cut]);
        }
        std_devs[i_cut] /= double(pixels[i_cut].size());
        std_devs[i_cut] = sqrt(std_devs[i_cut]);
        cout << ",\t std_dev: " << std_devs[i_cut] << flush;

        // SNR = average / std_dev
        cout << ",\t SNR: " << averages[i_cut] / std_devs[i_cut] << endl;
    }

    // CNR = (average[samp] - average[bkgd]) / std_dev[bkgd]
    double CNR = abs(averages[1] - averages[0]) / std_devs[1];
    cout << "CNR: " << CNR << endl;
    
    new TCanvas;
    hImage->Draw("colz");
    cut_samp->Draw("same");
    cut_bkgd->Draw("same");

    TCanvas* cTOF = new TCanvas;
    hTOF_all->DrawNormalized();
    for(int i_cut=0; i_cut<n_cut; i_cut++)
        hTOF_cut[i_cut]->DrawNormalized("same");
    gPad->SetLogy();
    //DrawToFAxis(TCanvas *pC, Double_t FlightPath, Double_t y_axis, Double_t ToF_max, Double_t Ekin_max);
    DrawToFAxis(         cTOF,                2.648,           0.03,              650,               10.);
}
