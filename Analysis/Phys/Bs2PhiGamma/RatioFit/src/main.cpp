#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "TChain.h"
#include "TF1.h"
#include "TFile.h"
#include "TH1F.h"
#include "TLeaf.h"
#include "TRandom.h"
#include "TTree.h"

#include "FFAux.h"
#include "FFBinning.h"
#include "FFBlind.h"
#include "FFConfig.h"
#include "FFFitter.h"
#include "FFLoad.h"
#include "FFPdfs.h"

/**
    @mainpage Fit framework for binned lifetime measurements at LHCb
    @author Carlos Sánchez Mayordomo (casanche@cern.ch) (IFIC, Valencia)
*/

void analyzeMacro(); //for toys
int main_data();
int main_toysZhirui();
int main_toysGen();

int main(int argc, char** argv) {
    main_data();
    //main_toysZhirui();
    //main_toysGen();
}

//==============
//==== DATA ====
//==============
int main_data()
{
    //=======================
    //==== Configuration ====
    //=======================
    FFConfig::read("config");
    if( FFConfig::get<bool>("Verbose") ) {
        FFConfig::print();
    }
    FFPdfs pdfs;
    pdfs.initialize();
    gRandom->SetSeed(0);

    //=======================
    //==== Set functions ====
    //=======================
    double xmin = FFConfig::get<double>("xmin");
    double xmax = FFConfig::get<double>("xmax");
    TF1* f1 = new TF1("f1", pdfs.Bs2PhiGamma, xmin, xmax, 1);
    f1->SetParameter(0, FFConfig::get<double>("A")); //not needed
    TF1* f2 = new TF1("f2", pdfs.Bd2KstGamma, xmin, xmax, 0);

    //========================
    //==== Set histograms ====
    //========================
    std::vector<double> binwalls = FFBinning::manual();
    TH1F* h1 = new TH1F("h1", "h1", binwalls.size()-1, &binwalls[0]);
    TH1F* h2 = new TH1F("h2", "h2", binwalls.size()-1, &binwalls[0]);

    //Automatic
    h1 = (TH1F*)FFLoad::get("direct_loose_phi", binwalls).Clone(); //options: direct_loose_phi, direct_tight_phi, sw_loose_phi, sw_tight_phi
    h2 = (TH1F*)FFLoad::get("direct_loose_kst", binwalls).Clone(); //options: direct_loose_kst, direct_tight_kse, sw_loose_kst, sw_tight_kst

    //Manual
    /*//std::string file = "/media/carlos/My Passport/LHCb/tuples/data/VGlt/sWeight/sw_miriam/Bs_loose_two_weights.root";
    //std::string file = "/home/carlos/Escritorio/LHCb/Work/FF/tuples/splotted_data/Bs2PhiG_with_sweights_tight.root";
    std::string file = "/media/carlos/My Passport/LHCb/tuples/pablo/Jan2015/PhiGamma/sPlots/Bs2PhiG_with_sweights_all_data_Both_Trigger_LooseSelection_and_Fiducials.root";
    double tau, sWeight;
    if(FFAux::ExistFile(file)) {
        TChain* tree1 = new TChain("DecayTree");
        tree1->Add(file.c_str());
        tree1->SetBranchAddress("B_BMassConst_TAU", &tau);
        tree1->SetBranchStatus("*",0);
        tree1->SetBranchStatus("B_BMassConst_TAU",1);
        tree1->SetBranchStatus("sWeight",1);
        h1->Sumw2();
        for(int i = 0; i < tree1->GetEntries(); ++i) {
            tree1->GetEntry(i);
            sWeight = tree1->GetLeaf("sWeight")->GetValue(0);
            h1->Fill(tau*1000, sWeight);
        }
    }

    //file = "/media/carlos/My Passport/LHCb/tuples/data/VGlt/sWeight/sw_miriam/B0_loose_two_weights.root";
    //file = "/home/carlos/Escritorio/LHCb/Work/FF/tuples/splotted_data/Bd2KstG_with_sweights_tight.root";
    file = "/media/carlos/My Passport/LHCb/tuples/pablo/Jan2015/KstGamma/sPlots/Bd2KstG_with_sweights_all_data_Both_Trigger_LooseSelection_and_Fiducials.root";
    if(FFAux::ExistFile(file)) {
        TChain* tree2 = new TChain("DecayTree");
        tree2->Add(file.c_str());
        tree2->SetBranchAddress("B_BMassConst_TAU", &tau);
        tree2->SetBranchStatus("*",0);
        tree2->SetBranchStatus("B_BMassConst_TAU",1);
        tree2->SetBranchStatus("sWeight",1);
        h2->Sumw2();
        for(int i = 0; i < tree2->GetEntries(); ++i) {
            tree2->GetEntry(i);
            sWeight = tree2->GetLeaf("sWeight")->GetValue(0);
            h2->Fill(tau*1000, sWeight);
        }
    }*/

    //h1->Print();
    //h2->Print();

    FFFitter::initialize(h1, f1, h2, f2);

    //=============
    //==== Fit ====
    //=============
    //FFFitter::direct(h1, f1);
    FFFitter::ratio(h1, f1, h2, f2);
    //FFFitter::simultaneous(h1, f1, h2, f2);

    return 0;
}

//=======================
//==== ZHIRUI'S TOYS ====
//=======================
int main_toysZhirui()
{
    //=======================
    //==== Configuration ====
    //=======================
    FFConfig::read("config");
    if( FFConfig::get<bool>("Verbose") ) {
        FFConfig::print();
    }
    FFPdfs pdfs;
    pdfs.initialize();
    gRandom->SetSeed(0);

    //=======================
    //==== Set functions ====
    //=======================
    double xmin = FFConfig::get<double>("xmin");
    double xmax = FFConfig::get<double>("xmax");
    TF1* f1 = new TF1("f1", pdfs.Bs2PhiGamma, xmin, xmax, 1);
    f1->SetParameter(0, FFConfig::get<double>("A"));
    TF1* f2 = new TF1("f2", pdfs.Bd2KstGamma, xmin, xmax, 0);

    //===================
    //==== Read Toys ====
    //===================
    std::ofstream fout("toys.dat");
    fout.close();

    std::vector<double> binwalls = FFBinning::manual();
    TH1F* h1 = new TH1F("h1", "h1", binwalls.size()-1, &binwalls[0]);
    h1->Sumw2();
    TH1F* h2 = new TH1F("h2", "h2", binwalls.size()-1, &binwalls[0]);
    h2->Sumw2();
    FFFitter::initialize(h1, f1, h2, f2);
    TStopwatch sw;

    int Ntoys = 1000;
    for(int i = 0; i < Ntoys; ++i) {
        std::string dir = "/media/carlos/My Passport/LHCb/tuples/toys/adelta/";
        std::string file = dir + "data_Toy_RL_ADelta-0.0_" + FFAux::toString(i) + ".root";

        if( FFAux::ExistFile(file) ) {
            FFAux::ProgressBarTPS(i+1, Ntoys, 50, sw);

            std::ofstream fout("toys.dat", std::ios::app);
            fout << i;
            fout.close();

            //Read histograms
            double tau;
            //double sWeight;
            TChain* tree1 = new TChain("Bs2PhiGamma_Signal");
            tree1->Add(file.c_str());
            tree1->SetBranchAddress("B_TAU", &tau);
            //tree1->SetBranchAddress("SIGYield_sw", &sWeight);
            for(int i = 0; i < tree1->GetEntries(); ++i) {
                tree1->GetEntry(i);
                h1->Fill(tau);
            }

            TChain* tree2 = new TChain("Bd2KstGamma_Signal");
            tree2->Add(file.c_str());
            tree2->SetBranchAddress("B_TAU", &tau);
            //tree2->SetBranchAddress("SIGYield_sw", &sWeight);
            for(int i = 0; i < tree2->GetEntries(); ++i) {
                tree2->GetEntry(i);
                h2->Fill(tau);
            }

            //Fit
            FFFitter::ratio(h1, f1, h2, f2);

            tree1->Delete();
            tree2->Delete();
            h1->Reset();
            h2->Reset();
        }
    }

    analyzeMacro();

    return 0;
}


//====================
//==== GENERATION ====
//====================
int main_toysGen()
{
    //=======================
    //==== Configuration ====
    //=======================
    FFConfig::read("config");
    if( FFConfig::get<bool>("Verbose") ) {
        FFConfig::print();
    }
    FFPdfs pdfs;
    pdfs.initialize();
    gRandom->SetSeed(0);

    //=======================
    //==== Set functions ====
    //=======================
    double xmin = FFConfig::get<double>("xmin");
    double xmax = FFConfig::get<double>("xmax");
    TF1* f1 = new TF1("f1", pdfs.Bs2PhiGamma, xmin, xmax, 1);
    f1->SetParameter(0, FFConfig::get<double>("A"));
    TF1* f2 = new TF1("f2", pdfs.Bd2KstGamma, xmin, xmax, 0);

    //==================
    //==== Toy loop ====
    //==================
    std::ofstream fout("toys.dat");

    std::vector<double> binwalls = FFBinning::manual();
    TH1F* h1 = new TH1F("h1", "h1", binwalls.size()-1, &binwalls[0]);
    h1->Sumw2();
    TH1F* h2 = new TH1F("h2", "h2", binwalls.size()-1, &binwalls[0]);
    h2->Sumw2();
    FFFitter::initialize(h1, f1, h2, f2);
    TStopwatch sw;

    int Ntoys = FFConfig::get<int>("Ntoys");
    for(int i = 0; i < Ntoys; ++i) {
        FFAux::ProgressBarTPS(i+1, Ntoys, 50, sw);

        //output file
        std::ofstream fout;
        if(i == 0) {
            fout.open("toys.dat");
        } else {
            fout.open("toys.dat", std::ios::app);
        }
        fout << i;
        fout.close();

        //Cholesky
        double cov[] = {3.61E-6, -3.0894E-6, -3.0894E-6, 3.6E-5};
        double x[] = {0, 0};
        double y[] = {0.6627, 0.081};
        const int dim = 2;
        double* ch = FFAux::Cholesky(cov, dim);

        TRandom3 *random = new TRandom3(0);
        for(int i = 0; i < dim; ++i) {
            x[i] = random->Gaus(0, 1);
        }
        for(int i = 0; i < dim; ++i) {
            for(int j = 0; j < dim; ++j) {
                y[i] += ch[i*dim + j] * x[j];
            }
        }
        /*FFPdfs::Gs = y[0];
        FFPdfs::DGs = y[1];
        FFPdfs::Gd = random->Gaus(0.6579, 0.0017);*/
        delete random;
        delete ch;

        double xmin_norm = FFConfig::get<double>("xmin_norm");
        double xmax_norm = FFConfig::get<double>("xmax_norm");
        TF1* f1n = new TF1("f1", pdfs.Bs2PhiGamma, xmin_norm, xmax_norm, 1);
        f1n->SetParameter(0, FFConfig::get<double>("A"));
        TF1* f2n = new TF1("f2", pdfs.Bd2KstGamma, xmin_norm, xmax_norm, 0);

        int Ntoys_phi = 4000;
        int Ntoys_kst = 24000;
        for(int j = 0; j < Ntoys_phi; ++j) {
            double tau = f1n->GetRandom();
            h1->Fill(tau);
        }
        for(int j = 0; j < Ntoys_kst; ++j) {
            double tau = f2n->GetRandom();
            h2->Fill(tau);
        }

        //=============
        //==== Fit ====
        //=============
        /*FFPdfs::Gd = 0.6579;
        FFPdfs::Gs = 0.6627;
        FFPdfs::DGs = 0.081;*/
        //FFFitter::direct(h1, f1);
        FFFitter::ratio(h1, f1, h2, f2);

        h1->Reset();
        h2->Reset();
    }

    analyzeMacro();

    return 0;
}

void analyzeMacro() {
    std::ifstream fin("toys.dat");
    if(!fin.is_open()) {
        std::cout << "Error reading toy file" << std::endl;
    } else {
        TH1F* h_1 = new TH1F("h_1", "h_1", 50, -2, 2);
        TH1F* h_2 = new TH1F("h_2", "h_2", 50, 0.1, 0.65);

        int Ntoy;
        double v1, v2;
        fin >> Ntoy >> v1 >> v2;
        while(!fin.eof()) {
            h_1->Fill(v1);
            h_2->Fill(v2);
            fin >> Ntoy >> v1 >> v2;
        }

        std::cout << "FIRST " << h_1->GetMean() << " +- " << h_1->GetMeanError() << ", RMS " << h_1->GetRMS() << " +- " << h_1->GetRMSError() << std::endl;
        std::cout << "SECOND " << h_2->GetMean() << " +- " << h_2->GetMeanError() << ", RMS " << h_2->GetRMS() << " +- " << h_2->GetRMSError() << std::endl;

        FFAux::HistDraw(h_1, "pe", "toys.pdf");

        h_1->SetMarkerStyle(kFullCircle);
        h_1->Draw("pe");
    }
}
