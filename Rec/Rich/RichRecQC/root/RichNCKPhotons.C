
#include "RichNCKPhotons.h"

namespace RICH
{
  // RICH CK photon yields
  namespace NCKPhotons
  {

    // fit limits
    std::map<std::string,double> & fitLimits()
    {
      static std::map<std::string,double> fitLimits;
      fitLimits["Aerogel"]  = 10;
      fitLimits["Rich1Gas"] = 30;
      fitLimits["Rich2Gas"] = 30;
      return fitLimits;
    }

    void doHist( TFile * file,
                 const std::string & hPath,
                 const std::string & title,
                 const std::string & prodTag,
                 const std::string & radiator,
                 const std::string & trackType,
                 const std::string & imageType )
    {
      TH1F * h = (TH1F*)file->Get(hPath.c_str());
      if (h)
      {
        TCanvas * canvas = new TCanvas("NPhotons","NPhotons");
        TF1 * fitter = new TF1("NPFit", "gaus", -fitLimits()[radiator],fitLimits()[radiator]);
        const std::string xName( trackType+" "+radiator+" # CK Photons / track (beta>0.99)" );
        h->GetXaxis()->SetTitle(xName.c_str());
        h->SetTitle(title.c_str());
        h->Fit(fitter,"R");
        const std::string fileName = prodTag+"/"+prodTag+"_"+trackType+"_"+radiator+"_nCKphotons."+imageType;
        RichUtils::systemSaveImage( canvas, fileName );
        delete fitter;
      } else { cerr << "Histogram '" << hPath << "' does not exist" << endl; }
    }

    // Creates images files for Aerogel CK resolution, for given root file
    void aerogel ( TFile * file,
                   const std::string & prodTag,
                   const std::string & nameTag,
                   const std::string & imageType )
    {
      const std::string radiator = "Aerogel";
      doHist(file,"RICH/RiCKResFrwd/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Forward",imageType);
      doHist(file,"RICH/RiCKResMatch/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Match",imageType);
      doHist(file,"RICH/RiCKResVeloTT/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"VeloTT",imageType);
      doHist(file,"RICH/RiCKResSeed/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Seed",imageType);
      doHist(file,"RICH/RiCKResKsTk/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"KsTrack",imageType);
    }

    // Creates images files for Aerogel CK resolution, for gicen root file
    void c4f10 ( TFile * file,
                 const std::string & prodTag,
                 const std::string & nameTag,
                 const std::string & imageType )
    {
      const std::string radiator = "Rich1Gas";
      doHist(file,"RICH/RiCKResFrwd/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Forward",imageType);
      doHist(file,"RICH/RiCKResMatch/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Match",imageType);
      doHist(file,"RICH/RiCKResVeloTT/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"VeloTT",imageType);
      doHist(file,"RICH/RiCKResSeed/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Seed",imageType);
      doHist(file,"RICH/RiCKResKsTk/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"KsTrack",imageType);
    }

    // Creates images files for Aerogel CK resolution, for gicen root file
    void cf4 ( TFile * file,
               const std::string & prodTag,
               const std::string & nameTag,
               const std::string & imageType )
    {
      const std::string radiator = "Rich2Gas";
      doHist(file,"RICH/RiCKResFrwd/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Forward",imageType);
      doHist(file,"RICH/RiCKResMatch/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Match",imageType);
      doHist(file,"RICH/RiCKResVeloTT/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"VeloTT",imageType);
      doHist(file,"RICH/RiCKResSeed/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"Seed",imageType);
      doHist(file,"RICH/RiCKResKsTk/"+radiator+"/nCKphots",radiator+nameTag,prodTag,radiator,"KsTrack",imageType);
    }

    // shortcut to producing all resolution figures
    void createAllFigures( TFile * file,
                           const std::string prodTag,
                           const std::string nameTag )
    {
      if (!file) return;

      // turn off histo data
      gStyle->SetOptStat(1);
      // turn on some fit data
      gStyle->SetOptFit(1011);

      // Create EPS files
      aerogel(file,prodTag,nameTag,"eps");
      c4f10(file,prodTag,nameTag,"eps");
      cf4(file,prodTag,nameTag,"eps");

      // create PNG files
      aerogel(file,prodTag,nameTag,"png");
      c4f10(file,prodTag,nameTag,"png");
      cf4(file,prodTag,nameTag,"png");

    }

  }
}
