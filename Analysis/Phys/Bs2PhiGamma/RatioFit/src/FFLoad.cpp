#include "FFLoad.h"

#include <iostream>

/**
* Get from a histogram
*/
TH1F FFLoad::get(const std::string& command, const std::vector<double>& binwalls) {
    TH1F h;

    //Temp
    if(command == "hist1") {
        ;
    }

    //Temp
    if(command == "hist2") {
        ;
    }

    //Direct
    if(command == "direct_loose_phi") {
        h = FFAux::FromHist("data/PhiG_BG_SUBTRACTED_TAU_ADAPTIVE_Loose_RSB_scaledstats_numintegration.root",
                                 "VL_data_SIG_TAU_ADAPTIVE", "hphi");
        for(int i = 1; i < h.GetSize(); ++i) {
            h.SetBinContent(i, h.GetBinContent(i) * h.GetBinWidth(i));
            h.SetBinError(i, h.GetBinError(i) * h.GetBinWidth(i));
        }
    }
    else if(command == "direct_loose_kst") {
        h  = FFAux::FromHist("data/KstG_BG_SUBTRACTED_TAU_ADAPTIVE_Loose_RSB_scaledstats_numintegration.root",
                                  "VL_data_SIG_TAU_ADAPTIVE", "hkst");
        for(int i = 1; i < h.GetSize(); ++i) {
            h.SetBinContent(i, h.GetBinContent(i) * h.GetBinWidth(i));
            h.SetBinError(i, h.GetBinError(i) * h.GetBinWidth(i));
        }
    }
    else if(command == "direct_tight_phi") {
        h = FFAux::FromHist("data/PhiG_BG_SUBTRACTED_TAU_ADAPTIVE_Tight_RSB_scaledstats_numintegration.root",
                                 "VT_data_SIG_TAU_ADAPTIVE", "hphi");
        for(int i = 1; i < h.GetSize(); ++i) {
            h.SetBinContent(i, h.GetBinContent(i) * h.GetBinWidth(i));
            h.SetBinError(i, h.GetBinError(i) * h.GetBinWidth(i));
        }
    }
    else if(command == "direct_tight_kst") {
        h  = FFAux::FromHist("data/KstG_BG_SUBTRACTED_TAU_ADAPTIVE_Tight_RSB_scaledstats_numintegration.root",
                                  "VT_data_SIG_TAU_ADAPTIVE", "hkst");
        for(int i = 1; i < h.GetSize(); ++i) {
            h.SetBinContent(i, h.GetBinContent(i) * h.GetBinWidth(i));
            h.SetBinError(i, h.GetBinError(i) * h.GetBinWidth(i));
        }
    }

    //Weighted trees
    if(command == "sw_loose_phi") {
        std::string file =  "data/Bs2PhiG_with_sweights_loose.root";
        double tau, sWeight;
        if(FFAux::ExistFile(file)) {
            TChain* tree1 = new TChain("Bs2PhiGamma");
            tree1->Add(file.c_str());
            tree1->SetBranchAddress("B_BMassConst_TAU", &tau);
            tree1->SetBranchAddress("sWeight", &sWeight);
            tree1->SetBranchStatus("*",0);
            tree1->SetBranchStatus("B_BMassConst_TAU",1);
            tree1->SetBranchStatus("sWeight",1);
            h = TH1F("h", "h", binwalls.size() - 1, &binwalls[0]);
            h.Sumw2();
            for(int i = 0; i < tree1->GetEntries(); ++i) {
                tree1->GetEntry(i);
                sWeight = tree1->GetLeaf("sWeight")->GetValue(0);
                h.Fill(tau*1000, sWeight);
            }
        }
    }
    else if(command == "sw_loose_kst") {
        std::string file =  "data/Bd2KstG_with_sweights_loose.root";
        double tau, sWeight;
        if(FFAux::ExistFile(file)) {
            TChain* tree1 = new TChain("Bd2KstGamma");
            tree1->Add(file.c_str());
            tree1->SetBranchAddress("B_BMassConst_TAU", &tau);
            tree1->SetBranchAddress("sWeight", &sWeight);
            tree1->SetBranchStatus("*",0);
            tree1->SetBranchStatus("B_BMassConst_TAU",1);
            tree1->SetBranchStatus("sWeight",1);
            h = TH1F("h", "h", binwalls.size() - 1, &binwalls[0]);
            h.Sumw2();
            for(int i = 0; i < tree1->GetEntries(); ++i) {
                tree1->GetEntry(i);
                sWeight = tree1->GetLeaf("sWeight")->GetValue(0);
                h.Fill(tau*1000, sWeight);
            }
        }
    }
    else if(command == "sw_tight_phi") {
        std::string file =  "data/Bs2PhiG_with_sweights_tight.root";
        double tau, sWeight;
        if(FFAux::ExistFile(file)) {
            TChain* tree1 = new TChain("Bs2PhiGamma");
            tree1->Add(file.c_str());
            tree1->SetBranchAddress("B_BMassConst_TAU", &tau);
            tree1->SetBranchAddress("sWeight", &sWeight);
            tree1->SetBranchStatus("*",0);
            tree1->SetBranchStatus("B_BMassConst_TAU",1);
            tree1->SetBranchStatus("sWeight",1);
            h = TH1F("h", "h", binwalls.size() - 1, &binwalls[0]);
            h.Sumw2();
            for(int i = 0; i < tree1->GetEntries(); ++i) {
                tree1->GetEntry(i);
                sWeight = tree1->GetLeaf("sWeight")->GetValue(0);
                h.Fill(tau*1000, sWeight);
            }
        }
    }
    else if(command == "sw_tight_kst") {
        std::string file =  "data/Bd2KstG_with_sweights_tight.root";
        double tau, sWeight;
        if(FFAux::ExistFile(file)) {
            TChain* tree1 = new TChain("Bd2KstGamma");
            tree1->Add(file.c_str());
            tree1->SetBranchAddress("B_BMassConst_TAU", &tau);
            tree1->SetBranchAddress("sWeight", &sWeight);
            tree1->SetBranchStatus("*",0);
            tree1->SetBranchStatus("B_BMassConst_TAU",1);
            tree1->SetBranchStatus("sWeight",1);
            h = TH1F("h", "h", binwalls.size() - 1, &binwalls[0]);
            h.Sumw2();
            for(int i = 0; i < tree1->GetEntries(); ++i) {
                tree1->GetEntry(i);
                sWeight = tree1->GetLeaf("sWeight")->GetValue(0);
                h.Fill(tau*1000, sWeight);
            }
        }
    }

    return h;
}
