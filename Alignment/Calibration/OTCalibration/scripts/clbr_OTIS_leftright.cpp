#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TF1.h>
#include <TF2.h>
#include <TStyle.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <math.h>
#include <boost/assign.hpp>
using namespace boost::assign;

std::vector<std::string> stationNames = list_of("T1")("T2")("T3");
std::vector<std::string> layerNames = list_of("X1")("U")("V")("X2");
std::vector<std::string> quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
std::vector<std::string> moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");

double walk_amp;
double walk_dy;
double walk_dprop;

TCanvas* createCanvas(const char* name, int w = 640, int h = 480)
{
  TCanvas* canvas = new TCanvas(name, name, 0, 0, w, h);
  canvas->SetWindowSize(w + (w - canvas->GetWw()), h + (h - canvas->GetWh()));
  return canvas;
}

void readT0s(const std::string& path, double t0s[3][4][4][9]);
void readOTIS_T0s(const std::string& path, double t0s[3][4][4][9][4]);

void writeT0s(double t0s[3][4][4][9])
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = quarterId + "@" + prefix + ".xml";

    std::ofstream file(fileName.c_str());
    if(file.fail())
    {
      printf("Can't open file: '%s'\n", fileName.c_str());
//      std::cout << "Can't open file: '" << fileName << "'" << std::endl;
      continue;
    }

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
    file << "\n";
    file << "<DDDB>\n";
    file << "<catalog name=\"" << prefix << quarterId << "\">\n";

    for(int m = 0; m < 9; m++)
    {
      std::string moduleId = quarterId + moduleNames[m];

      file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

      file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
      if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
      else      file << "     " << 2.6 << " " << 0/*(3.2 - 2.6 - 4.0 * 0.15)*/ << " " << 4.0 * 0.15 << "\n";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
      file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
      file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
      //file << "      " << 0 << "\n";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
      file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
      file << "    </paramVector>\n";

      file << "  </condition>\n";
    }

    file << "</catalog>\n";
    file << "</DDDB>\n";

    file.flush();
    file.close();
  }
}

void writeCondXMLs(double t0s[3][4][4][9][4])
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {
    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

    std::string fileName = quarterId + "@" + prefix + ".xml";

    std::ofstream file(fileName.c_str());
    if(file.fail())
    {
      printf("Can't open file: '%s'\n", fileName.c_str());
//      std::cout << "Can't open file: '" << fileName << "'" << std::endl;
      continue;
    }

    file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
    file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
    file << "\n";
    file << "<DDDB>\n";
    file << "<catalog name=\"" << prefix << quarterId << "\">\n";

    for(int m = 0; m < 9; m++)
    {
      std::string moduleId = quarterId + moduleNames[m];

      file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

      file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
      if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
      else      file << "     " << 2.6 << " " << 0/*(3.2 - 2.6 - 4.0 * 0.15)*/ << " " << 4.0 * 0.15 << "\n";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
      file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
      for(int b = 0; b<4;b++){
	  file << "      " <<  0.001 * (int)(1000.0 * t0s[s][l][q][m][b] + 0.5)<< "\n";
	}
      
      file << "    </paramVector>\n";

      file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
      file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
      file << "    </paramVector>\n";

      file << "  </condition>\n";
    }

    file << "</catalog>\n";
    file << "</DDDB>\n";

    file.flush();
    file.close();
  }
}



double fit(TFile* file, const char* name, int s, int l, int q, int m)
{
  char histName[256];
  sprintf(histName, "OTModuleClbrMon/%s/%s/%s/%s/%s", stationNames[s].c_str(), layerNames[l].c_str(), quarterNames[q].c_str(), moduleNames[m].c_str(), name);
  TH1D* hist = (TH1D*)file->Get(histName);

  if(hist == 0 || hist->GetEntries() < 100)
  {
	if(!(m==8 && (q == 0 || q == 2))) std::cout << "Not Enough statistics: " << histName << std::endl;
	return 0;
  }
  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();
  for(int i = 0; i < 5; i++)
  {
    hist->Fit("gaus", "QRLL", "", left, right);
    left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
    right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
  }

  //should not happen:
  if(abs(hist->GetFunction("gaus")->GetParameter(1))>10.0) {
	  std::cout << ((std::string) stationNames[s] + (std::string) layerNames[l] + (std::string) quarterNames[q] + (std::string) moduleNames[m] + (std::string) name + " : ").c_str()
	  << " fitted mean = " << hist->GetFunction("gaus")->GetParameter(1) 
	  << "; histo average = " << hist->GetMean()   << std::endl;
	  return hist->GetMean();
  }
  return hist->GetFunction("gaus")->GetParameter(1);
}

double GlobalT0(std::string fileName){

  TFile* file = new TFile(fileName.c_str());
  TCanvas* c=new TCanvas();
  gStyle->SetOptFit(111);

  std::string histName = "OTModuleClbrMon/" + stationNames[0] + "/driftTimeResidual";
  TH1D* hist = (TH1D*)file->Get(histName.c_str());
  histName = "OTModuleClbrMon/" + stationNames[1] + "/driftTimeResidual";
  hist->Add((TH1D*)file->Get(histName.c_str()));
  histName = "OTModuleClbrMon/" + stationNames[2] + "/driftTimeResidual";
  hist->Add((TH1D*)file->Get(histName.c_str()));
      
  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();
  for(int i = 0; i < 5; i++){
        hist->Fit("gaus", "QRLL", "", left, right);
        left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
        right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
   }
     
  hist->Draw();
  c->Print("globalT0.eps"); 
  std::cout << "Global t0 = " <<  hist->GetFunction("gaus")->GetParameter(1) << std::endl;
  std::cout << "Global resolution = " <<  hist->GetFunction("gaus")->GetParameter(2) << std::endl;
     
  return hist->GetFunction("gaus")->GetParameter(1);
}

int main()
{
  gStyle->SetOptStat("");
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.10);
  gStyle->SetPadLeftMargin(0.08);
  gStyle->SetTitleXOffset(1.05);
  gStyle->SetTitleYOffset(1.05);
  gStyle->SetTitleStyle(0);

  //read constants from new OTIS based DB (true) or old module based DB (false)
  bool readOTIS_DB = true;
  
  std::string fileName;
  //fileName = "/afs/cern.ch/user/p/phdargen/cmtuser/Brunel_v47r7/Calibration/OTCalibration/scripts/run154882/clbr_hists_run154882_oldSelection.root" ;
  fileName = "/afs/cern.ch/user/p/phdargen/cmtuser/Brunel_v47r7/Calibration/OTCalibration/scripts/run154882_oldSelection/clbr_hists_run154882_2it.root" ;
  std::cout << "Using file " << fileName.c_str() << std::endl;
  
  double globalT0= GlobalT0(fileName.c_str());;
  
  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));
  double OTIS_t0s[3][4][4][9][4]; memset(OTIS_t0s, 0, sizeof(OTIS_t0s));
  
  if(readOTIS_DB){
      std::cout << "Read OTIS T0s ..." << std::endl;
      readOTIS_T0s("./db_2015/", OTIS_t0s);
      for(int s = 0; s < 3; s++){
	for(int l = 0; l < 4; l++){
	  for(int q = 0; q < 4; q++){
	    for(int m = 8; m >= 0; m--){
	      for(int o=0; o<4; o++){
		  std::cout<< "s = "<<s<< " l = "<< l<< " q = "<< q << " m = "<< m << " o = " << o << " t0 = " << OTIS_t0s[s][l][q][m][o]<<std::endl;
	      }
	    }
	  }
	}
      }
  }
  else {
      std::cout<< "READ module t0s"<< std::endl;
      readT0s("./db_2013/", t0s);
      for(int s = 0; s < 3; s++){
	for(int l = 0; l < 4; l++){
	  for(int q = 0; q < 4; q++){
	    for(int m = 8; m >= 0; m--){
		std::cout<< "s = "<<s<< " l = "<< l<< " q = "<< q << " m = "<< m << "t0 = " << t0s[s][l][q][m]<<std::endl;
	    }
	  }
	}
      }
  }
  
  TFile* file = new TFile(fileName.c_str());

  std::cout << "Book hists ..." << std::endl;
  TH1D* hdt0 = new TH1D("hdt0", "", 100, -2, 2);
  TH1D* ht0 = new TH1D("ht0", "", 432, 0, 432);

  std::cout << "Loop ..." << std::endl;
  for(int s = 0; s < 3; s++) for(int l = 0; l < 4; l++) for(int q = 0; q < 4; q++)
  {

    for(int m = 8; m >= 0; m--)
    {
      std::string histName = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual";
      TH1D* hist = (TH1D*)file->Get(histName.c_str());
      if(hist == 0)continue;

      int modulen = m + 9 * (q + 4 * (l + 4 * s));
  
      double residual_0L =  fit(file, "driftTimeResidual_otis0_L", s, l, q, m);
      double residual_0R =  fit(file, "driftTimeResidual_otis0_R", s, l, q, m);
      double residual_1L =  fit(file, "driftTimeResidual_otis1_L", s, l, q, m);
      double residual_1R =  fit(file, "driftTimeResidual_otis1_R", s, l, q, m);
      double residual_2L =  fit(file, "driftTimeResidual_otis2_L", s, l, q, m);
      double residual_2R =  fit(file, "driftTimeResidual_otis2_R", s, l, q, m);
      double residual_3L =  fit(file, "driftTimeResidual_otis3_L", s, l, q, m);
      double residual_3R =  fit(file, "driftTimeResidual_otis3_R", s, l, q, m);
      
      double t0 = 0;
      if(!readOTIS_DB){
	OTIS_t0s[s][l][q][m][0] = t0s[s][l][q][m] ;
	OTIS_t0s[s][l][q][m][1] = t0s[s][l][q][m] ;
	OTIS_t0s[s][l][q][m][2] = t0s[s][l][q][m] ;
	OTIS_t0s[s][l][q][m][3] = t0s[s][l][q][m] ;
      }
      if(residual_0L != 0 && residual_0R !=0)OTIS_t0s[s][l][q][m][0] +=  0.5*(residual_0L+residual_0R) - globalT0 ;
      if(residual_1L != 0 && residual_1R !=0)OTIS_t0s[s][l][q][m][1] +=  0.5*(residual_1L+residual_1R) - globalT0;
      if(residual_2L != 0 && residual_2R !=0)OTIS_t0s[s][l][q][m][2] +=  0.5*(residual_2L+residual_2R) - globalT0;
      if(residual_3L != 0 && residual_3R !=0)OTIS_t0s[s][l][q][m][3] +=  0.5*(residual_3L+residual_3R) - globalT0;
	
      if(residual_0L != 0 && residual_0R !=0)t0 = 0.5*(residual_0L+residual_0R)- globalT0;
      if(residual_0L != 0 && residual_0R !=0)hdt0->Fill(0.5*(residual_0L+residual_0R)- globalT0);
      if(residual_1L != 0 && residual_1R !=0)hdt0->Fill(0.5*(residual_1L+residual_1R)- globalT0);
      if(residual_2L != 0 && residual_2R !=0)if(!(m==8 && (q == 0 || q == 2)))hdt0->Fill(0.5*(residual_2L+residual_2R)- globalT0);
      if(residual_3L != 0 && residual_3R !=0)if(!(m==8 && (q == 0 || q == 2)))hdt0->Fill(0.5*(residual_3L+residual_3R)- globalT0);

      ht0->SetBinContent(ht0->FindBin(modulen), t0);
      if(fabs(t0) > 1) std::cout << histName << " :: t0 = " << t0 << std::endl;
    }
  }

  writeCondXMLs(OTIS_t0s);

  TCanvas* canvas = createCanvas("", 1280, 480);
  gStyle->SetOptStat("");

  hdt0->Draw();
  canvas->SaveAs("hdt0.eps");
  
  ht0->GetYaxis()->SetRangeUser(-4, 4);
  ht0->SetMarkerStyle(20);
  ht0->SetMarkerSize(0.8);
  ht0->Draw("e1");
  canvas->SaveAs("ht0.eps");

  return 0;
}
