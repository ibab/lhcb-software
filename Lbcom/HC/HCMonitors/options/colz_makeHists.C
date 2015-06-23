#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH2F.h"
#include "TString.h"
#include "TCanvas.h"
#include <iostream>
#include <map>
#include <TStyle.h>
#include <TRandom3.h>
using namespace std;

int limit=10000;
//Maximum adc_value witch is used
int nbins=80;
int nbins1d=200;
// Number of bins in the histogramms (*100 for 1D histogramms)//

TRandom3 *Random = new TRandom3();
//randomize
int randomize(int adc){

    if (adc<128.5){
        return adc;
    }else if(adc<255.5){
        //int a =Random->Uniform(0,2);
        //cout<<a<<endl;
        return adc+int(Random->Uniform(0,4));
    }else if (adc<511.5){
        return adc+ int(Random->Uniform(0,8));
    }else{
        return adc+ int(Random->Uniform(0,16));
    }
    return 0;
}

int getChannelNum(TString side, int station, int counter) {
    TString key = Form("%s%i%i",side.Data(),station,counter);
    map<TString,int> mapping;
    mapping["B00"]=47; mapping["B01"]=46; mapping["B02"]=45; mapping["B03"]=44;
    mapping["B10"]=23; mapping["B11"]=22; mapping["B12"]=21; mapping["B13"]=20;
    mapping["B20"]=11; mapping["B21"]=10; mapping["B22"]=9;  mapping["B23"]=8;


    //Run 154041
    mapping["F10"]=43; mapping["F11"]=42; mapping["F12"]=41; mapping["F13"]=40; 
    mapping["F20"]=31; mapping["F21"]=30; mapping["F22"]=29; mapping["F23"]=28;


    //Run 153759
    //mapping["F10"]=47; mapping["F11"]=46; mapping["F12"]=45; mapping["F13"]=36;
    //mapping["F20"]=27; mapping["F21"]=26; mapping["F22"]=25; mapping["F23"]=24;
    return mapping[key];
}

//EventMaximum per station
int getmaxium_4(int counter_0, int counter_1, int counter_2, int counter_3) {
    return max(max(counter_0,counter_1),max(counter_2,counter_3));
}

void makePlots(TString filename,TString runNumber) {


    gStyle->SetNdivisions(510,"x");
    gStyle->SetNdivisions(510,"z");
    gStyle->SetLabelSize(0.06,"xyz");
    gStyle->SetLabelOffset(0.015,"y");
    //gStyle->SetLabelOffset(0.,"z");
    gStyle->SetTitleSize(0.06,"xyz");
    gStyle->SetTitleOffset(1.00,"x");
    gStyle->SetTitleOffset(1,"y");

    gStyle->SetTitleOffset(-0.15,"z");



    gStyle->SetOptStat(0);  
    gStyle->SetNdivisions(510,"y");


    gStyle->SetPaperSize(20,26);
    gStyle->SetPadTopMargin(0.05);
    gStyle->SetPadRightMargin(0.2); // increase for colz plots
    gStyle->SetPadBottomMargin(0.16);
    gStyle->SetPadLeftMargin(0.14);


    /*

       Int_t lhcbFont        = 132;  // Old LHCb style: 62;
    // Line thickness
    Double_t lhcbWidth    = 2.00; // Old LHCb style: 3.00;
    // Text size
    Double_t lhcbTSize    = 0.06;
    // use plain black on white colors
    //gROOT->SetStyle("Plain"); 
    TStyle *lhcbStyle= new TStyle("lhcbStyle","LHCb plots style");


    lhcbStyle->SetTextFont(lhcbFont);
    lhcbStyle->SetTextSize(lhcbTSize);
    lhcbStyle->SetLabelFont(lhcbFont,"x");
    lhcbStyle->SetLabelFont(lhcbFont,"y");
    lhcbStyle->SetLabelFont(lhcbFont,"z");
    lhcbStyle->SetLabelSize(lhcbTSize,"x");
    lhcbStyle->SetLabelSize(lhcbTSize,"y");
    lhcbStyle->SetLabelSize(lhcbTSize,"z");
    lhcbStyle->SetTitleFont(lhcbFont);
    lhcbStyle->SetTitleFont(lhcbFont,"x");
    lhcbStyle->SetTitleFont(lhcbFont,"y");
    lhcbStyle->SetTitleFont(lhcbFont,"z");
    lhcbStyle->SetTitleSize(1.2*lhcbTSize,"x");
    lhcbStyle->SetTitleSize(1.2*lhcbTSize,"y");
    lhcbStyle->SetTitleSize(1.2*lhcbTSize,"z");

    lhcbStyle->SetMarkerStyle(20);
    lhcbStyle->SetMarkerSize(1.0);

    // label offsets
    lhcbStyle->SetLabelOffset(0.010,"X");
    lhcbStyle->SetLabelOffset(0.010,"Y");


    //////LHCb style (Black and white)
    //gROOT->ProcessLine(".L lhcbStyle.C");
    //lhcbStyle();
    */
    cout << filename << endl;
    TFile *f= TFile::Open(filename);
    TTree *t= (TTree*)f->Get("HCDigitTuple/HCDigitNtuple");
    // Set branch addresses
    Float_t adc_f[64]; t->SetBranchAddress("adc_F",adc_f);
    Float_t adc_b[64]; t->SetBranchAddress("adc_B",adc_b);

    ////////////////////////////// 
    // Prepare histograms
    //////////////////////////////

    //1D distribution in all quadrants
    map<TString, TH1F*> h1_distribution_;
    for (int iside=0; iside<2; iside++) {
        TString side = (iside==0 ? "B" : "F");

        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            for (int x=0; x<4; x++) {
                h1_distribution_[Form("%i_%i_%i",iside,station,x)] = new TH1F(Form("h1_distribution_%i_%i_%i",iside,station,x),"",nbins1d ,-0.5,1023.5);


                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->SetTitle(Form("%s%i_%i ADC counts",side.Data(),station,x)); 
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->CenterTitle(); 



                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->SetTitleOffset(1); 
            }
        }
    }







    // Correlation plots
    map<TString, TH2F*> h2_corrsWithinStation;
    for (int iside=0; iside<2; iside++) {
        TString side = (iside==0 ? "B" : "F");

        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            for (int x=0; x<3; x++) {
                for (int y=x+1; y<4; y++) {
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)] = new TH2F(Form("h2_corrsWithinStation_%i_%i_x=%i_y=%i",iside,station,x,y),"",nbins ,-0.5,1023.5,nbins ,-0.5,1023.5);


                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->SetTitle(Form("Correlation_%s%i_between_%i_%i",side.Data(),station,x,y)); 
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetXaxis()->SetTitle(Form("%s%i_%i ADC counts",side.Data(),station,x)); 
                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetXaxis()->SetTitleSize(0.05); 
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetXaxis()->CenterTitle(); 
                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetXaxis()->SetTitleOffset(2);

                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetYaxis()->SetTitle(Form("%s%i_%i ADC counts",side.Data(),station,y)); 
                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetYaxis()->SetTitleSize(0.01);
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetYaxis()->CenterTitle(); 
                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetYaxis()->SetTitleOffset(2); 




                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetZaxis()->SetTitle(Form("ADC counts")); 
                    //h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->GetZaxis()->CenterTitle(); 

                }
            }
        }
    }
    // Maxima per station
    map<TString, TH1F*> h1_123DistsWhen0Saturated;
    for (int iside=0; iside<2; iside++) {
        TString side = (iside==0 ? "B" : "F");

        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            for (int x=1; x<4; x++) {




                h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)] = new TH1F(Form("h1_123DistsWhen0Saturated_%i_%i_x=%i",iside,station,x),"",nbins1d ,-0.5,1023.5);


                h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->SetTitle(Form("%s%i_%i ADC counts",side.Data(),station,x)); 
                h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->CenterTitle(); 



                h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)]->GetXaxis()->SetTitleOffset(1); 

            }
        }
    }

    map<TString, TH1F*> h1_Maximum;
    for (int iside=0; iside<2; iside++) {

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            h1_Maximum[Form("%i_%i",iside,station)] = new TH1F(Form("h1_Maximum_%i_%i",iside,station),"",nbins1d ,-0.5,1023.5);


            h1_Maximum[Form("%i_%i",iside,station)]->GetXaxis()->SetTitle(Form("%s%i ADC counts",side.Data(),station)); 
            //h1_Maximum[Form("%i_%i",iside,station)]->SetTitle(Form("Maximum_of_Station_%s%i",side.Data(),station)); 
            h1_Maximum[Form("%i_%i",iside,station)]->GetXaxis()->CenterTitle(); 
            h1_Maximum[Form("%i_%i",iside,station)]->GetXaxis()->SetTitleOffset(1);


        }
    }


    /////////////////////////
    //Station Correlation all counters
    /////////////////////////
    map<TString, TH2F*> h2_corrs_inbetween_Stations_all;
    for (int iside=0; iside<2; iside++) {

        TString side = (iside==0 ? "B" : "F");

        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)] = new TH2F(Form("h2_corrs_inbetween_Stations_%i_sta1=%i_sta2=%i_all",iside,station,station_2),"",nbins ,-0.5,4*1024-0.5,nbins ,-0.5,4*1024-0.5);



                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->GetXaxis()->SetTitle(Form("%s%i ADC counts",side.Data(),station)); 
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->GetXaxis()->CenterTitle(); 

                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->GetYaxis()->SetTitle(Form("%s%i ADC counts",side.Data(),station_2)); 
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->GetYaxis()->CenterTitle(); 


            }   
        } 
    }

    /////////////////////////
    //Station Correlation
    /////////////////////////
    map<TString, TH2F*> h2_corrs_inbetween_Stations;
    for (int iside=0; iside<2; iside++) {

        TString side = (iside==0 ? "B" : "F");

        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)] = new TH2F(Form("h2_corrs_inbetween_Stations_%i_sta1=%i_sta2=%i",iside,station,station_2),"",nbins ,-0.5,1023.5,nbins ,-0.5,1023.5);



                //h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->SetTitle(Form("Correlation_between_%s%i_%s%i",side.Data(),station,side.Data(),station_2)); 
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetXaxis()->SetTitle(Form("%s%i ADC counts",side.Data(),station)); 
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetXaxis()->CenterTitle(); 
                //h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetXaxis()->SetTitleOffset(2);

                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetYaxis()->SetTitle(Form("%s%i ADC counts",side.Data(),station_2)); 
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetYaxis()->CenterTitle(); 
                //h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetYaxis()->SetTitleOffset(2); 

                //h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetZaxis()->SetTitle(Form("ADC counts")); 
                //h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->GetZaxis()->CenterTitle(); 


            }   
        } 
    }

    /////////////////////////
    //Station Correlation per counter
    /////////////////////////
    map<TString, TH2F*> h2_corrs_inbetween_Stations_counter;

    map<TString, TH2F*> h2_corrs_inbetween_Stations_diffrent_counters;
    for (int x=0; x<4; x++) {
        for (int iside=0; iside<2; iside++) {

            TString side = (iside==0 ? "B" : "F");

            for (int station=(iside==0 ? 0 : 1); station<2; station++) {
                for (int station_2=station+1; station_2<3; station_2++) {
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)] = new TH2F(Form("h2_corrs_inbetween_Stations_counter_%i__%i_sta1=%i_sta2=%i",x,iside,station,station_2),"",nbins ,-0.5,1023.5,nbins ,-0.5,1023.5);



                    //h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->SetTitle(Form("Correlation_between_%s%i_%s%i in counter_%i",side.Data(),station,side.Data(),station_2,x)); 
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetXaxis()->SetTitle(Form("%s%i_Counter_%i ADC counts",side.Data(),station,x)); 
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetXaxis()->CenterTitle(); 
                    //h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetXaxis()->SetTitleOffset(2);

                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetYaxis()->SetTitle(Form("%s%i_Counter_%i ADC counts",side.Data(),station_2,x)); 
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetYaxis()->CenterTitle(); 
                    //h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetYaxis()->SetTitleOffset(2); 
                    //h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetZaxis()->SetTitle(Form("ADC counts")); 
                    //h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->GetZaxis()->CenterTitle(); 

                    if (x!=0 )
                    {
                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]= new TH2F(Form("h2_corrs_inbetween_Stations_counters_%iand%i__%i_sta1=%i_sta2=%i",0,x,iside,station,station_2),"",nbins ,-0.5,1023.5,nbins ,-0.5,1023.5);

                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetXaxis()->SetTitle(Form("%s%i_Counter_%i ADC counts",side.Data(),station,0)); 
                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetXaxis()->CenterTitle(); 

                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetYaxis()->SetTitle(Form("%s%i_Counter_%i ADC counts",side.Data(),station_2,x)); 
                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetYaxis()->CenterTitle(); 

                        //h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetZaxis()->SetTitle(Form("ADC counts")); 
                        //h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->GetZaxis()->CenterTitle(); 

                    }

                }   
            } 
        }
    }

    /////////////////////////
    //Side Correlation
    /////////////////////////

    TH2F *h2_corrs_inbetween_Sides = new TH2F("h2_corrs_inbetween_Sides","",nbins,-0.5,1023.5,nbins,-0.5,1023.5);


    //h2_corrs_inbetween_Sides->SetTitle("Correlation between B and F"); 
    h2_corrs_inbetween_Sides->GetXaxis()->SetTitle("B ADC counts"); 
    h2_corrs_inbetween_Sides->GetXaxis()->CenterTitle(); 
    //h2_corrs_inbetween_Sides->GetXaxis()->SetTitleOffset(2);

    h2_corrs_inbetween_Sides->GetYaxis()->SetTitle("F ADC counts"); 
    h2_corrs_inbetween_Sides->GetYaxis()->CenterTitle(); 
    //h2_corrs_inbetween_Sides->GetYaxis()->SetTitleOffset(2); 


    //h2_corrs_inbetween_Sides->GetZaxis()->SetTitle(Form("ADC counts")); 
    //h2_corrs_inbetween_Sides->GetZaxis()->CenterTitle(); 


    /////////////////////////
    //Side Correlation per counter
    /////////////////////////

    map<TString, TH2F*> h2_corrs_inbetween_Sides_counter;

    for (int x=0; x<4; x++) {

        h2_corrs_inbetween_Sides_counter[Form("No%i",x)] =new TH2F(Form("h2_corrs_inbetween_Sides_counter_%i",x),"",nbins,-0.5,1023.5,nbins,-0.5,1023.5);


        //h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->SetTitle(Form("Correlation between B and F in counter: %i",x)); 
        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetXaxis()->SetTitle("B ADC counts"); 
        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetXaxis()->CenterTitle(); 
        //h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetXaxis()->SetTitleOffset(2);

        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetYaxis()->SetTitle("F ADC counts"); 
        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetYaxis()->CenterTitle(); 
        //h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetYaxis()->SetTitleOffset(2); 
        //h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetZaxis()->SetTitle(Form("ADC counts")); 
        //h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->GetZaxis()->CenterTitle(); 


    }



    //////////////////////////////
    // Load tree
    //////////////////////////////
    int entries=t->GetEntries();
    double counter=0;
    for (int i = 0; i<entries; i++) {
        t->GetEntry(i);
        if(double(i)/double(entries)>counter){
            cout<<endl<<counter*100<<"\% filled";

            counter+=0.05;
        }
        // Fill distribution per counter
        for (int iside=0; iside<2; iside+=1) {
            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                for (int x=0; x<4; x++) {
                    if( side=="B" ){ 
                        if (adc_b[getChannelNum(side,station,x)]<limit){
                            h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Fill(randomize(adc_b[getChannelNum(side,station,x)]));
                        }
                    }
                    else
                        if (adc_f[getChannelNum(side,station,x)]<limit){

                            h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Fill(randomize(adc_f[getChannelNum(side,station,x)]));
                        }
                }
            }
        }



        // Fill correlations within station
        for (int iside=0; iside<2; iside+=1) {
            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                for (int x=0; x<3; x++) {
                    for (int y=x+1; y<4; y++) {
                        if( side=="B" ){ 
                            if (adc_b[getChannelNum(side,station,x)]+ adc_b[getChannelNum(side,station,y)]<2*limit){
                                h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Fill(randomize(adc_b[getChannelNum(side,station,x)]),randomize( adc_b[getChannelNum(side,station,y)]));
                            }
                        }
                        else
                            if (adc_f[getChannelNum(side,station,x)]+ adc_f[getChannelNum(side,station,y)]<2*limit){

                                h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Fill( randomize(adc_f[getChannelNum(side,station,x)]), randomize(adc_f[getChannelNum(side,station,y)]));
                            }
                    }
                }
            }
        }

        // Fill 1,2,3 distributions when 0 is saturated
        for (int iside=0; iside<2; iside+=1) {
            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                for (int x=1; x<4; x++) {
                    if( side=="B" ) {
                        if (adc_b[getChannelNum(side,station,0)]>1000) h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)]->Fill(randomize( adc_b[getChannelNum(side,station,x)]) );
                    }
                    else {
                        if (adc_f[getChannelNum(side,station,0)]>1000) h1_123DistsWhen0Saturated[Form("%i_%i_%i",iside,station,x)]->Fill( randomize(adc_f[getChannelNum(side,station,x)] ));
                    }
                }
            }
        }



        // Fill Maximum per counter

        for (int iside=0; iside<2; iside+=1) {
            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                if( side=="B" ) {
                    h1_Maximum[Form("%i_%i",iside,station)]->Fill( randomize(getmaxium_4(adc_b[getChannelNum(side,station,0)], adc_b[getChannelNum(side,station,1)], adc_b[getChannelNum(side,station,2)], adc_b[getChannelNum(side,station,3)] )));
                } 
                else {
                    h1_Maximum[Form("%i_%i",iside,station)]->Fill(randomize(getmaxium_4(adc_f[getChannelNum(side,station,0)], adc_f[getChannelNum(side,station,1)], adc_f[getChannelNum(side,station,2)], adc_f[getChannelNum(side,station,3)] )));
                }
            }
        }

        // Fill correlations between stations

        for (int iside=0; iside<2; iside++) {

            float all_sta_1;
            float all_sta_2;

            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<2; station++) {

                for (int station_2=station+1; station_2<3; station_2++) {
                    if( side=="B" ) {
                        all_sta_1=adc_b[getChannelNum(side,station,0)]+ adc_b[getChannelNum(side,station,1)]+ adc_b[getChannelNum(side,station,2)]+ adc_b[getChannelNum(side,station,3)];

                        all_sta_2=adc_b[getChannelNum(side,station_2,0)]+ adc_b[getChannelNum(side,station_2,1)]+ adc_b[getChannelNum(side,station_2,2)]+ adc_b[getChannelNum(side,station_2,3)];



                    }
                    else {
                        all_sta_1=adc_f[getChannelNum(side,station,0)]+ adc_f[getChannelNum(side,station,1)]+ adc_f[getChannelNum(side,station,2)]+ adc_f[getChannelNum(side,station,3)];

                        all_sta_2=adc_f[getChannelNum(side,station_2,0)]+ adc_f[getChannelNum(side,station_2,1)]+ adc_f[getChannelNum(side,station_2,2)]+ adc_f[getChannelNum(side,station_2,3)];

                    }
                    if (all_sta_1+all_sta_2<2*limit )
                    {

                        h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Fill(randomize(all_sta_1),randomize(all_sta_2));
                    }
                }   
            } 
        }



        // Fill correlations between stations

        for (int iside=0; iside<2; iside++) {
            float max_sta_1;
            float max_sta_2;

            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<2; station++) {

                for (int station_2=station+1; station_2<3; station_2++) {
                    if( side=="B" ) {
                        max_sta_1=getmaxium_4(adc_b[getChannelNum(side,station,0)], adc_b[getChannelNum(side,station,1)], adc_b[getChannelNum(side,station,2)], adc_b[getChannelNum(side,station,3)] );

                        max_sta_2=getmaxium_4(adc_b[getChannelNum(side,station_2,0)], adc_b[getChannelNum(side,station_2,1)], adc_b[getChannelNum(side,station_2,2)], adc_b[getChannelNum(side,station_2,3)] );



                    }
                    else {
                        max_sta_1=getmaxium_4(adc_f[getChannelNum(side,station,0)], adc_f[getChannelNum(side,station,1)], adc_f[getChannelNum(side,station,2)], adc_f[getChannelNum(side,station,3)] );

                        max_sta_2=getmaxium_4(adc_f[getChannelNum(side,station_2,0)], adc_f[getChannelNum(side,station_2,1)], adc_f[getChannelNum(side,station_2,2)], adc_f[getChannelNum(side,station_2,3)] );

                    }
                    if (max_sta_1+max_sta_2<2*limit )
                    {

                        h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Fill(randomize(max_sta_1),randomize(max_sta_2));
                    }
                }   
            } 
        }

        // Fill correlations between stations per counter

        for (int x=0 ;x<4 ;x++ )
        {

            for (int iside=0; iside<2; iside++) {
                float count_sta_1;
                float count_sta_2;

                TString side = (iside==0 ? "B" : "F");
                for (int station=(iside==0 ? 0 : 1); station<2; station++) {

                    for (int station_2=station+1; station_2<3; station_2++) {


                        if( side=="B" ) {
                            count_sta_1=adc_b[getChannelNum(side,station,x)];

                            count_sta_2=adc_b[getChannelNum(side,station_2,x)];



                        }
                        else {
                            count_sta_1=adc_f[getChannelNum(side,station,x)];

                            count_sta_2=adc_f[getChannelNum(side,station_2,x)];

                        }
                        if (count_sta_1+count_sta_2<2*limit )
                        {

                            h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Fill(randomize(count_sta_1),randomize(count_sta_2));
                        }


                        if (x!=0 )
                        {
                            if( side=="B" ) {
                                count_sta_1=adc_b[getChannelNum(side,station,0)];
                            }
                            else {
                                count_sta_1=adc_f[getChannelNum(side,station,0)];
                            }

                            if (count_sta_1+count_sta_2<2*limit )
                            {

                                h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->Fill(randomize(count_sta_1),randomize(count_sta_2));
                            }



                        }

                    }   
                } 
            }
        }



        // Fill correlations between sides

        float max_b=0;
        float max_f=0;
        for (int iside=0; iside<2; iside+=1) {
            TString side = (iside==0 ? "B" : "F");

            for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                for (int x=0; x<3; x++) {
                    if (side=="B") {

                        max_b=max(adc_b[getChannelNum(side,station,x)],max_b);
                    } 
                    else {
                        max_f=max(adc_f[getChannelNum(side,station,x)],max_f);

                    }

                }
            }
        }
        if (max_b+max_f<limit)
        {

            h2_corrs_inbetween_Sides->Fill(randomize(max_b),randomize(max_f));
        }



        // Fill correlations between sides per counter


        for (int x =0 ;x<4 ; x++)
        {

            float count_b=0;
            float count_f=0;
            for (int iside=0; iside<2; iside+=1) {
                TString side = (iside==0 ? "B" : "F");

                for (int station=(iside==0 ? 0 : 1); station<3; station++) {
                    if (side=="B") {

                        count_b=max(adc_b[getChannelNum(side,station,x)],count_b);                    } 
                    else {
                        count_f=max(adc_f[getChannelNum(side,station,x)],count_f);                    } 

                }

            }


            if (count_b+count_f<limit)
            {

                h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->Fill(randomize(count_b),randomize(count_f));
            }

        }

    }//End Looping over Events


    cout<<endl;




    //////////////////////////////
    // Save histograms
    //////////////////////////////

    // Distribution per counters
    TFile* f_distribution_counter = new TFile("hists/Distribution_counters.root","recreate");
    for (map<TString,TH1F*>::iterator it=h1_distribution_.begin(); it!=h1_distribution_.end(); it++) {
        (it->second)->Write();
    }
    f_distribution_counter->Close();

    // Correlations within stations
    TFile* f_corrsWithinStation = new TFile("hists/CorrsWithinStation.root","recreate");
    for (map<TString,TH2F*>::iterator it=h2_corrsWithinStation.begin(); it!=h2_corrsWithinStation.end(); it++) {
        (it->second)->Write();
    }
    f_corrsWithinStation->Close();
    // Counters when 0 is saturated
    TFile* f_123DistsWhen0Saturated = new TFile("hists/123DistsWhen0Saturated.root","recreate");
    for (map<TString,TH1F*>::iterator it=h1_123DistsWhen0Saturated.begin(); it!=h1_123DistsWhen0Saturated.end(); it++) {
        (it->second)->Write();
    }
    f_123DistsWhen0Saturated->Close();
    // Maximum Counter per Station
    TFile* f_Maximum_per_station = new TFile("hists/Maximum_per_station.root","recreate");
    for (map<TString,TH1F*>::iterator it=h1_Maximum.begin(); it!=h1_Maximum.end(); it++) {
        (it->second)->Write();
    }
    f_Maximum_per_station->Close();


    /////////////////////////
    //Correlation between stations all still missing
    /////////////////////////

    // Correlations between stations
    TFile* f_corrs_inbetween_Stations = new TFile("hists/Corrs_inbetween_Stations.root","recreate");
    for (map<TString,TH2F*>::iterator it=h2_corrs_inbetween_Stations.begin(); it!=h2_corrs_inbetween_Stations.end(); it++) {
        (it->second)->Write();
    }
    f_corrs_inbetween_Stations->Close();

    // Correlations between stations per counter
    map<TString, TFile*> f_corrs_inbetween_Stations_counter;

    for (int x=0 ;x<4 ;x++ )
    {

        f_corrs_inbetween_Stations_counter[Form("_%i",x)] = new TFile(Form("hists/Corrs_inbetween_Stations_counter_%i.root",x),"recreate");
        for (map<TString,TH2F*>::iterator it=h2_corrs_inbetween_Stations_counter.begin(); it!=h2_corrs_inbetween_Stations_counter.end(); it++) {
            (it->second)->Write();
        }
        f_corrs_inbetween_Stations_counter[Form("_%i",x)]->Close();
    }


    // Correlations between sides
    TFile* f_corrs_inbetween_Sides = new TFile("hists/Corrs_inbetween_Sides.root","recreate");
    h2_corrs_inbetween_Sides->Write();
    f_corrs_inbetween_Sides->Close();



    // Correlations between sides per counter
    map<TString, TFile*> f_corrs_inbetween_Sides_counter;

    for (int x=0 ;x<4 ;x++ )
    {

        f_corrs_inbetween_Sides_counter[Form("_%i",x)] = new TFile(Form("hists/Corrs_inbetween_Sides_counter_%i.root",x),"recreate");
        for (map<TString,TH2F*>::iterator it=h2_corrs_inbetween_Sides_counter.begin(); it!=h2_corrs_inbetween_Sides_counter.end(); it++) {
            (it->second)->Write();
        }
        f_corrs_inbetween_Sides_counter[Form("_%i",x)]->Close();
    }












    //////////////////////////////
    // Print histograms
    //////////////////////////////


    /////////////////////////
    //Distribution per counter
    /////////////////////////

    TCanvas *c0 = new TCanvas("c0","",400,250);

    //gPad->SetBottomMargin(0.15);

    for (int iside=0; iside<2; iside+=1) {
        TString side = (iside==0 ? "B" : "F");
        cout <<"Side: "<< side << endl;
        int i=1;
        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            c0->Clear();
            c0->Divide(2,2);

            cout << "Station: "<<station<<endl;
            cout << "Normal Plots"<<endl;
            i=1;
            for (int x=0; x<4; x++) {
                cout <<"Counter: "<< x  << endl;
                c0->cd(i++);
                gPad->SetBottomMargin(0.15);
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");
            }

            c0->SaveAs(Form("figs/Run%s/Distribution%s%i.pdf(",runNumber.Data(),side.Data(),station),"pdf");

            cout<<endl<<endl<< "log-Plots"<<endl;
            c0->Clear();
            c0->Divide(2,2);
            i=1;
            for (int x=0; x<4; x++) {
                cout <<"Counter: "<< x  << endl;
                c0->cd(i++);

                gPad->SetBottomMargin(0.15);
                gPad->SetLogy();
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");

            }

            c0->SaveAs(Form("figs/Run%s/Distribution%s%i.pdf",runNumber.Data(),side.Data(),station),"pdf");


            cout<<endl<<endl<< "Normal plus log Plots"<<endl;


            c0->Clear();
            c0->Divide(1,2);
            for (int x=0; x<4; x++) {

                i=1;

                cout <<"Counter: "<< x  << endl;

                c0->cd(i++);
                gPad->SetBottomMargin(0.15);
                gPad->SetLogy(0);  
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");

                c0->cd(i++);
                gPad->SetBottomMargin(0.15);

                gPad->SetLogy(1); 
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");
                c0->SaveAs(Form("figs/Run%s/Distribution%s%i.pdf",runNumber.Data(),side.Data(),station),"pdf");


            }

            c0->Clear();
            c0->SaveAs(Form("figs/Run%s/Distribution%s%i.pdf]",runNumber.Data(),side.Data(),station),"pdf");


            //pdf



            c0->Clear();
            for (int x=0; x<4; x++) {


                /*leg = new TLegend(0.1,0.7,0.48,0.9);

                  leg->SetHeader(Form("ADC-distribution of %s%i_%i",side.Data(),station,x));

                //leg->AddEntry(h1_distribution_[Form("%i_%i_%i",iside,station,x)],"Histogram filled with adc-","f");

                leg->Draw();
                */
                i=1;

                cout <<"Counter: "<< x << endl;

                c0->cd(i++);
                gPad->SetBottomMargin(0.15);
                gPad->SetLogy(0);  
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");

                c0->SaveAs(Form("figs/Run%s/Counterdistribution/Distribution%s%i_%i.pdf",runNumber.Data(),side.Data(),station,x));

                gPad->SetLogy(1); 
                h1_distribution_[Form("%i_%i_%i",iside,station,x)]->Draw("");


                c0->SaveAs(Form("figs/Run%s/Counterdistribution/Distribution%s%i_%i_log.pdf",runNumber.Data(),side.Data(),station,x));

            }

            cout << "____________________________________________________________________________"<<endl;

        }
    }







    /////////////////////////
    //Between counters
    /////////////////////////

    TCanvas *c = new TCanvas("c","",400,250);


    cout <<endl<<endl<<endl<<endl<< "Correlation between counters"<<endl;
    for (int iside=0; iside<2; iside+=1) {
        TString side = (iside==0 ? "B" : "F");
        cout <<"Side: "<< side << endl;
        int i=1;
        for (int station=(iside==0 ? 0 : 1); station<3; station++) {
            c->Clear();
            c->Divide(2,3);

            cout << "Station: "<<station<<endl;
            cout << "Normal Plots"<<endl;
            i=1;
            for (int x=0; x<3; x++) {
                for (int y=x+1; y<4; y++) {
                    cout <<"Counters: "<< x << ", " << y << endl;
                    c->cd(i++);
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");
                }
            }

            c->SaveAs(Form("figs/Run%s/CorrsWithinStation_%s%i.pdf(",runNumber.Data(),side.Data(),station),"pdf");

            cout<<endl<<endl<< "log-Plots"<<endl;
            c->Clear();
            c->Divide(2,3);
            i=1;
            for (int x=0; x<3; x++) {
                for (int y=x+1; y<4; y++) {
                    cout <<"Counters: "<< x << ", " << y << endl;
                    c->cd(i++);

                    gPad->SetLogz();
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");
                }
            }

            c->SaveAs(Form("figs/Run%s/CorrsWithinStation_%s%i.pdf",runNumber.Data(),side.Data(),station),"pdf");


            cout<<endl<<endl<< "Normal plus log Plots"<<endl;


            c->Clear();
            c->Divide(1,2);
            for (int x=0; x<3; x++) {

                for (int y=x+1; y<4; y++) {
                    i=1;

                    cout <<"Counters: "<< x << ", " << y << endl;

                    c->cd(i++);
                    gPad->SetLogz(0);  
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");

                    c->cd(i++);

                    gPad->SetLogz(1); 
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");
                    c->SaveAs(Form("figs/Run%s/CorrsWithinStation_%s%i.pdf",runNumber.Data(),side.Data(),station),"pdf");

                }
            }

            c->Clear();
            c->SaveAs(Form("figs/Run%s/CorrsWithinStation_%s%i.pdf]",runNumber.Data(),side.Data(),station),"pdf");


            //pdf

            c->Clear();
            for (int x=0; x<3; x++) {

                for (int y=x+1; y<4; y++) {
                    i=1;

                    cout <<"Counters: "<< x << ", " << y << endl;

                    c->cd(i++);
                    gPad->SetLogz(0);  
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");

                    c->SaveAs(Form("figs/Run%s/Between_Counters/CorrsWithinStation_%s%i__%i_%i.pdf",runNumber.Data(),side.Data(),station,x,y));

                    gPad->SetLogz(1); 
                    h2_corrsWithinStation[Form("%i_%i_%i_%i",iside,station,x,y)]->Draw("COLZ");


                    c->SaveAs(Form("figs/Run%s/Between_Counters/CorrsWithinStation_%s%i__%i_%i_log.pdf",runNumber.Data(),side.Data(),station,x,y));
                }
            }

            cout << "____________________________________________________________________________"<<endl;

        }
    }





    /////////////////////////
    //Maximum per Station
    /////////////////////////
    cout <<endl<<endl<<endl<<endl<< "Maximum per Stations"<<endl;

    TCanvas *c2 = new TCanvas("c2","",400,250); //c->Divide(3,3);

    gPad->SetBottomMargin(0.15);
    for (int iside=0; iside<2; iside+=1) {
        int i=1;
        TString side = (iside==0 ? "B" : "F");

        cout <<"Side: "<< side << endl;

        for (int station=(iside==0 ? 0 : 1); station<3; station++) {

            i=1;

            c2->Clear();
            c2->Divide(1,2);
            c2->cd(i++);
            gPad->SetBottomMargin(0.15);
            h1_Maximum[Form("%i_%i",iside,station)]->Draw("C");

            c2->cd(i++);
            gPad->SetBottomMargin(0.15);
            gPad->SetLogy();
            h1_Maximum[Form("%i_%i",iside,station)]->Draw("C");
            c2->SaveAs(Form("figs/Run%s/Maximum_%s%i.pdf(",runNumber.Data(),side.Data(),station),"pdf");

            c2->Clear();


            gPad->SetBottomMargin(0.15);

            gPad->SetLogy(0);

            h1_Maximum[Form("%i_%i",iside,station)]->Draw("C");
            c2->SaveAs(Form("figs/Run%s/Maximum_%s%i.pdf",runNumber.Data(),side.Data(),station),"pdf");


            c2->Clear();
            c2->SetLogy();
            h1_Maximum[Form("%i_%i",iside,station)]->Draw("C");
            c2->SaveAs(Form("figs/Run%s/Maximum_%s%i.pdf)",runNumber.Data(),side.Data(),station),"pdf");


            cout << "____________________________________________________________________________"<<endl;

        }

    }




    /////////////////////////
    //Between Stations all counters
    /////////////////////////


    cout <<endl<<endl<<endl<<endl<< "Correlation between Stations all Counters"<<endl;
    TCanvas *c3a = new TCanvas("c3a","",400,250); //c->Divide(3,3);





    int i=1;

    c3a->Divide(2,2);

    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        cout <<endl<<endl<<endl<<endl<<"Side: "<< side << endl;


        cout <<"Side: "<< side << endl;

        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3a->cd(i++);
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");
            }
        }
    }

    c3a->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_all.pdf(",runNumber.Data()),"pdf");


    c3a->Clear();
    c3a->Divide(2,2);
    i=1;
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3a->cd(i++);
                gPad->SetLogz();
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");
            }
        }
    }

    c3a->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_all.pdf",runNumber.Data()),"pdf");


    c3a->Clear();
    c3a->Divide(1,2);
    i=1;
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {

                i=1;

                c3a->cd(i++);
                gPad->SetLogz(0);  
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3a->cd(i++);
                gPad->SetLogz();
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3a->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_all.pdf",runNumber.Data()),"pdf");

            }
        }
    }
    c3a->Clear();
    c3a->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_all.pdf]",runNumber.Data()),"pdf");
    //pdf
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3a->Clear();
                gPad->SetLogz(0);  
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3a->SaveAs(Form("figs/Run%s/Between_stations_all/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i.pdf",runNumber.Data(),iside,station,station_2));

                c3a->Clear();
                gPad->SetLogz();
                h2_corrs_inbetween_Stations_all[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3a->SaveAs(Form("figs/Run%s/Between_stations_all/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i_log.pdf",runNumber.Data(),iside,station,station_2));

            }
        }
    }


    cout << "____________________________________________________________________________"<<endl;






    /////////////////////////
    //Between Stations
    /////////////////////////


    cout <<endl<<endl<<endl<<endl<< "Correlation between Stations"<<endl;
    TCanvas *c3 = new TCanvas("c3","",400,250); //c->Divide(3,3);





    i=1;

    c3->Divide(2,2);

    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        cout <<endl<<endl<<endl<<endl<<"Side: "<< side << endl;


        cout <<"Side: "<< side << endl;

        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3->cd(i++);
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");
            }
        }
    }

    c3->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations.pdf(",runNumber.Data()),"pdf");


    c3->Clear();
    c3->Divide(2,2);
    i=1;
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3->cd(i++);
                gPad->SetLogz();
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");
            }
        }
    }

    c3->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations.pdf",runNumber.Data()),"pdf");


    c3->Clear();
    c3->Divide(1,2);
    i=1;
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {

                i=1;

                c3->cd(i++);
                gPad->SetLogz(0);  
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3->cd(i++);
                gPad->SetLogz();
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations.pdf",runNumber.Data()),"pdf");

            }
        }
    }
    c3->Clear();
    c3->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations.pdf]",runNumber.Data()),"pdf");
    //pdf
    for (int iside=0; iside<2; iside++) { 

        TString side = (iside==0 ? "B" : "F");
        for (int station=(iside==0 ? 0 : 1); station<2; station++) {
            for (int station_2=station+1; station_2<3; station_2++) {


                c3->Clear();
                gPad->SetLogz(0);  
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3->SaveAs(Form("figs/Run%s/Between_stations/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i.pdf",runNumber.Data(),iside,station,station_2));

                c3->Clear();
                gPad->SetLogz();
                h2_corrs_inbetween_Stations[Form("%i_%i_%i",iside,station,station_2)]->Draw("COLZ");

                c3->SaveAs(Form("figs/Run%s/Between_stations/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i_log.pdf",runNumber.Data(),iside,station,station_2));

            }
        }
    }


    cout << "____________________________________________________________________________"<<endl;







    /////////////////////////
    //Between Stations per counter
    /////////////////////////


    cout <<endl<<endl<<endl<<endl<< "Correlation between Stations_counter"<<endl;
    TCanvas *c3_b = new TCanvas("c3_b","",400,250); //c->Divide(3,3);




    for (int x=0 ;x<4 ;x++ )
    {

        cout <<"Counter: "<<x<<endl;

        int i=1;

        c3_b->Clear();
        c3_b->Divide(2,2);
        for (int iside=0; iside<2; iside++) { 

            TString side = (iside==0 ? "B" : "F");
            cout <<endl<<endl<<endl<<endl<<"Side: "<< side << endl;


            cout <<"Side: "<< side << endl;


            for (int station=(iside==0 ? 0 : 1); station<2; station++) {
                for (int station_2=station+1; station_2<3; station_2++) {


                    c3_b->cd(i++);
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");
                }
            }
        }

        c3_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_Counter_%i.pdf(",runNumber.Data(),x),"pdf");


        c3_b->Clear();
        c3_b->Divide(2,2);
        i=1;
        for (int iside=0; iside<2; iside++) { 

            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<2; station++) {
                for (int station_2=station+1; station_2<3; station_2++) {


                    c3_b->cd(i++);
                    gPad->SetLogz();
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");
                }
            }
        }

        c3_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_Counter_%i.pdf",runNumber.Data(),x),"pdf");


        c3_b->Clear();
        c3_b->Divide(1,2);
        i=1;
        for (int iside=0; iside<2; iside++) { 

            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<2; station++) {
                for (int station_2=station+1; station_2<3; station_2++) {

                    i=1;

                    c3_b->cd(i++);
                    gPad->SetLogz(0);  
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");

                    c3_b->cd(i++);
                    gPad->SetLogz();
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");

                    c3_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_Counter_%i.pdf",runNumber.Data(),x),"pdf");

                }
            }
        }
        c3_b->Clear();
        c3_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Stations_Counter_%i.pdf]",runNumber.Data(),x),"pdf");

        //pdf
        for (int iside=0; iside<2; iside++) { 

            TString side = (iside==0 ? "B" : "F");
            for (int station=(iside==0 ? 0 : 1); station<2; station++) {
                for (int station_2=station+1; station_2<3; station_2++) {


                    c3_b->Clear();
                    gPad->SetLogz(0);  
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");

                    c3_b->SaveAs(Form("figs/Run%s/Between_station_per_counter/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i_Counter_%i.pdf",runNumber.Data(),iside,station,station_2,x));
                    c3_b->Clear();
                    gPad->SetLogz();
                    h2_corrs_inbetween_Stations_counter[Form("No%i__%i_%i_%i",x,iside,station,station_2)]->Draw("COLZ");

                    c3_b->SaveAs(Form("figs/Run%s/Between_station_per_counter/Corrs_inbetween_Stations_%i_sta_%i_sta2_%i_Counter_%i_log.pdf",runNumber.Data(),iside,station,station_2,x));




                    c3_b->Clear();
                    gPad->SetLogz(0);  
                    if(x!=0){
                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->Draw("COLZ");

                        c3_b->SaveAs(Form("figs/Run%s/Between_station_per_counter_bonus/Corrs_inbetween_Stations_%i_sta_%i_counter_0_sta2_%i_Counter_%i.pdf",runNumber.Data(),iside,station,station_2,x));
                        c3_b->Clear();
                        gPad->SetLogz();

                        h2_corrs_inbetween_Stations_diffrent_counters[Form("Counters%iand%i__%i_%i_%i",0,x,iside,station,station_2)]->Draw("COLZ");
                        c3_b->SaveAs(Form("figs/Run%s/Between_station_per_counter_bonus/Corrs_inbetween_Stations_%i_sta_%i_counter_0_sta2_%i_Counter_%i_log.pdf",runNumber.Data(),iside,station,station_2,x));
                    }
                }
            }
        }

    }

    cout << "____________________________________________________________________________"<<endl;



    /////////////////////////
    // Between Sides
    /////////////////////////



    cout <<endl<<endl<<endl<<endl<< "Correlation between Sides"<<endl;


    TCanvas *c4 = new TCanvas("c4","",400,250);
    i=1;
    c4->Divide(1,2);
    c4->cd(i++);
    h2_corrs_inbetween_Sides->Draw("COLZ");
    c4->cd(i++);
    gPad->SetLogz();
    h2_corrs_inbetween_Sides->Draw("COLZ");
    c4->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides.pdf(",runNumber.Data()),"pdf");


    c4->Clear();
    gPad->SetLogy(0);
    h2_corrs_inbetween_Sides->Draw("COLZ");

    c4->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides.pdf",runNumber.Data()),"pdf");

    c4->SaveAs(Form("figs/Run%s/Between_sides/Corrs_inbetween_Sides.pdf",runNumber.Data()));


    c4->Clear();
    c4->SetLogz();

    h2_corrs_inbetween_Sides->Draw("COLZ");


    c4->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides.pdf)",runNumber.Data()),"pdf");

    c4->SaveAs(Form("figs/Run%s/Between_sides/Corrs_inbetween_Sides_log.pdf",runNumber.Data()));




    cout << "____________________________________________________________________________"<<endl;






    /////////////////////////
    // Between Sides per counter
    /////////////////////////



    cout <<endl<<endl<<endl<<endl<< "Correlation between Sides per conter"<<endl;


    TCanvas *c4_b = new TCanvas("c4_b","",400,250);

    for (int x=0 ;x<4 ;x++ )
    {
        cout<<endl<<endl<<x<<endl<<endl;

        i=1;
        c4_b->Clear();
        c4_b->Divide(1,2);
        c4_b->cd(i++);


        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->Draw("COLZ");
        c4_b->cd(i++);
        gPad->SetLogz();

        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->Draw("COLZ");
        c4_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides_Counter_%i.pdf(",runNumber.Data(),x),"pdf");


        c4_b->Clear();
        gPad->SetLogy(0);
        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->Draw("COLZ");

        c4_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides_Counter_%i.pdf",runNumber.Data(),x),"pdf");


        c4_b->Clear();
        c4_b->SetLogz();

        h2_corrs_inbetween_Sides_counter[Form("No%i",x)]->Draw("COLZ");


        c4_b->SaveAs(Form("figs/Run%s/Corrs_inbetween_Sides_Counter_%i.pdf)",runNumber.Data(),x),"pdf");





        cout << "____________________________________________________________________________"<<endl;

    }





}

int main() {


    //First 
    //TString runNumber = "154041";

    //second
    //TString runNumber = "153759";
    
    //154051
    TString runNumber = "154051";
    
    
    
    makePlots(Form("HC_Digits_run%s.root",runNumber.Data()),runNumber.Data());
    return 0;
}

