
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>

#include "TH1F.h"
#include "TFile.h"
#include "TAxis.h"

/// Simple ROOT macro for producing the theta distribution maps
/// needed by the Markov ring finder.
///
/// Usage is simply :-
///
/// > .L PrintHistoXY.C+
/// > Markov::PrintRich1GasCKTheta("rich.histos.root");
/// > Markov::PrintRich2GasCKTheta("rich.histos.root");

namespace Markov
{

  void PrintHisto( TH1F * histo, const std::string & filename )
  {
    TAxis * xAxis = histo->GetXaxis();

    // Bins
    Int_t nbins  = xAxis->GetNbins();
    Int_t firstx = xAxis->GetFirst();
    Int_t lastx  = xAxis->GetLast();
    std::cout << "Found " << nbins << " bins in X : "
              << firstx << "->" << lastx << std::endl;

    std::ofstream file(filename.c_str());
    if ( file.is_open() )
    {
      std::cout << "Printing (x,y) values to " << filename << std::endl;
      for ( Int_t binx = firstx; binx<=lastx; ++binx )
      {
        const Double_t x = xAxis->GetBinCenter(binx);
        const Double_t y = histo->GetBinContent(binx);
        file << std::setprecision(10) << "( " << x << " , " << y << " )" << std::endl;
      }

    }
    else
    {
      std::cerr << "FAILED to open output file " << filename << std::endl;
    }

  }

  TH1F * loadHist( TFile * file,
                   const std::string & location )
  {
    TH1F * h = (TH1F*)file->Get( location.c_str() );
    if (!h) { cerr << "WARNING : Histogram '" << location << "' not found" << endl; }
    return h;
  }

  void PrintRich2GasCKTheta( std::string & filename )
  {
    TFile * file = TFile::Open(filename.c_str());
    if ( file )
    {
      TH1F  * h    = loadHist( file, "RICH/RiRecPhotAngLongTkMoni/Rich2Gas/ckThetaTrue" );
      PrintHisto(h,"rich2_thetaDist.txt");
      file->Close();
    }
    else { std::cerr << "Failed to open " << filename << std::endl; }
  }

  void PrintRich1GasCKTheta( std::string & filename )
  {
    TFile * file = TFile::Open(filename.c_str());
    if ( file )
    {
      TH1F  * h    = loadHist( file, "RICH/RiRecPhotAngLongTkMoni/Rich1Gas/ckThetaTrue" );
      PrintHisto(h,"rich1_thetaDist.txt");
      file->Close();
    }
    else { std::cerr << "Failed to open " << filename << std::endl; }
  }

}
