#include "TApplication.h"
#include "TCanvas.h"
#include "TPaveLabel.h"
#include "TStyle.h"
#include "TH1.h"
#include "TSystem.h"
// from Viewer
#include "DimInfoHistos.h"
#include <stdlib.h>
#include <iostream>
#include <string>
#ifdef WIN32
namespace win {
#include <windows.h>
  
  
}
# define mysleep win::Sleep
#else
# define mysleep usleep  // usleep - sleep some number of microseconds
#endif

// Save a list of histograms published by Gaucho/DimHistos to a root file
// Author: J. Helder Lopes, 27/07/2006

int main(int argc, char **argv)
{  
  if( argc < 3 ) // Remember there is always at least one argument: argv[0] = self-file name
  {
    std::cerr << "SaveHistos: Give list of histogram service names and a file name as parameters. Stopping." 
             << std::endl; 
    return 1;
  }
  
  std::string hSvcname;
  std::string hname;
  std::string filename=argv[argc-1];
  std::cerr << "SaveHistos: Saving histograms to file " << filename << std::endl;
  
  TFile *f=0;
  TH1* hist=0;
  int refreshtime=2; // Refresh time in SECONDS for the DIM time handler
  
  DimBrowser dbr;  
  
  for(int i=0;i!=argc-2;++i)
  { 
    hSvcname=argv[i+1];
    //std::cerr << "SaveHistos: Trying to retrieve histogram " <<  hSvcname << std::endl; 
    // Check if this service is available
    int nSvc = dbr.getServices( hSvcname.c_str() );
    // std::cerr << "SaveHistos: Dim services browser result: " << nSvc << std::endl; 
    if( nSvc == 0 ) {
      std::cerr << "SaveHistos: No Dim histogram service " << hSvcname << " available. Skipping." << std::endl;
      continue;
    }
    int len = hSvcname.length();
    hname=hSvcname.substr(3,len-3);
    hist=0;
    
    DimInfoHistos* hinfo = new DimInfoHistos(hSvcname,refreshtime);
    mysleep(refreshtime*1000000);
    int ntries  = 1;
    while(1)
    {
      hist = hinfo->getHisto();
      if( hist == NULL )
	    {
	      //std::cerr << "SaveHistos: Trying to retrieve histogram " <<  hname << " ntry: " 
              //    << ntries  << std::endl;
	      mysleep(refreshtime*1000);
	      if(ntries==10) {
          std::cerr << "SaveHistos: histogram " <<  hname << " not found after " << ntries 
                    << " atempts. Skipping"  << std::endl;
          break;
        }
	      continue;
	    }
      else
	    {
	      if( 0 == f )f = new TFile(filename.c_str(),"update");
	      std::cerr << "SaveHistos: Saving Histogram " << hname << std::endl;
	      hist->Write();
        break;
	    }
      ntries++;
    }
  }
  
  if(0 == f) return 1; 
  f->Close();
  return 0;
}
