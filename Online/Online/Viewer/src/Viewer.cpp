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
# define mysleep usleep // usleep - sleep some number of microseconds
#endif

// Opens a root canvas to display a histogram published by Gaucho
// Modified by J. Helder Lopes, 27/07/2006


int main(int argc, char **argv)
{
  std::string hSvcname;
  std::string hname;
  int refreshtime=10; // Refresh time in SECONDS for the DIM time handler
  
  // The argument should be a histogram Service name published by an Online/Gaucho job. 
  if( argc < 2 ) std::cerr << "Usage: $0 histoSvcname\n" 
                          << "where histoSvcname is a histogram Service name published by an Online/Gaucho job."
                          << std::endl;  
  else           hSvcname=argv[1];
  
  // Check if this histo service is available
  DimBrowser dbr;  
  std::cerr << "ROOT viewer: Looking for hSvcname: " << hSvcname.c_str() << std::endl;
  //std::cerr << "ROOT viewer: Dim services browser result: " << dbr.getServices( hSvcname.c_str() ) << std::endl; 
  if( dbr.getServices( hSvcname.c_str() ) == 0 ) {
    std::cerr << "ROOT viewer: No service " << hSvcname << " available. Leaving." << std::endl;
    return 1;
  }
  int len = hSvcname.length();
  hname=hSvcname.substr(3,len-3);
  
  TApplication theApp("App", &argc, argv);
  std::cerr <<"ROOT viewer: Started Root TApplication"  << std::endl;
  TCanvas* c = new TCanvas("c", "Online/Gaucho Histogram viewer using DIM", 400, 400);
  TH1* hist=0;
  
  // Enter event loop, one can now interact with the objects in
  // the canvas. Select "Exit ROOT" from Canvas "File" menu to exit
  // the event loop and execute the next statements.
  
  gStyle->SetHistFillColor(2);
  gStyle->SetHistFillStyle(2);
  gStyle->SetHistLineColor(2);
  gStyle->SetHistLineWidth(3);
  
  //std::cerr << "ROOT appl: going into histogram loop" << std::endl;
  
  DimInfoHistos hinfo(hSvcname,refreshtime);
  //std::cerr << "ROOT appl: hSvc constructed...  "<< std::endl;
  
  int ntries=0;
  std::cerr << "ROOT viewer: getting histogram" <<std::endl;
  
  while (1) {
    
    hist = hinfo.getHisto();
    if (hist == NULL)
    {
      ntries++;
      std::cerr << "ROOT viewer: No histogram found after " 
               << ntries << " attempts" <<std::endl;
      if(ntries==10) return 2;
      mysleep(refreshtime*1000000); 
      continue;
    }    
    gSystem->ProcessEvents();
    //std::cerr << "ROOT viewer: redrawing histogram" <<std::endl;
		//hist->Draw("E");
		hist->Draw();
    c->Update();
    
    mysleep(refreshtime*1000000); 
  }
  
  
  // std::cerr << "ROOT viewer: No success getting histogram. Leaving." <<std::endl;
  //theApp.Terminate(1);
  // Here we don't return from the eventloop. "Exit ROOT" will quit the app.
  //theApp.Run();
  
  return 1;
}
