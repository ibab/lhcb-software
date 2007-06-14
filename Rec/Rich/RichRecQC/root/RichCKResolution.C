
#include "RichCKResolution.h"

namespace RICH
{

  // RICH CK resolution functions
  namespace CKResolution
  {

    // random histo number
    unsigned int uniqueHnumber()
    {
      static unsigned int i(0);
      return ++i;
    }

    // fit limits
    std::map<std::string,double> & fitLimits()
    {
      static std::map<std::string,double> fitLimits;
      if ( fitLimits.empty() )
      {
        fitLimits["Aerogel"]  = 0.006;
        fitLimits["Rich1Gas"] = 0.0035;
        fitLimits["Rich2Gas"] = 0.0015;
      }
      return fitLimits;
    }

    // CK max
    std::map<std::string,double> & CKMax()
    {
      static std::map<std::string,double> ckMax;
      if ( ckMax.empty() )
      {
        ckMax["Aerogel"]  = 0.24;
        ckMax["Rich1Gas"] = 0.055;
        ckMax["Rich2Gas"] = 0.03;
      }
      return ckMax;
    }

    // CK min
    std::map<std::string,double> & CKMin()
    {
      static std::map<std::string,double> ckMin;
      if ( ckMin.empty() )
      {
        ckMin["Aerogel"]  = 0.2;
        ckMin["Rich1Gas"] = 0.025;
        ckMin["Rich2Gas"] = 0.018;
      }
      return ckMin;
    }

    // binned CK bin edges
    typedef std::pair<double,double> Bins;
    typedef std::vector<Bins> BinsList;
    typedef std::map<std::string, BinsList> BinnedCK;
    BinnedCK & binnedCKbins()
    {
      static BinnedCK ckBinned;
      if ( ckBinned.empty() )
      {
        BinsList & aero = ckBinned["Aerogel"];
        aero.push_back( Bins(0,0.13)     );
        aero.push_back( Bins(0.13,0.18)  );
        aero.push_back( Bins(0.18,0.23)  );
        aero.push_back( Bins(0.23,1)     );
        BinsList & g1 = ckBinned["Rich1Gas"];
        g1.push_back( Bins(0,0.0275)     );
        g1.push_back( Bins(0.0275,0.047) );
        g1.push_back( Bins(0.047,1)      );
        BinsList & g2 = ckBinned["Rich2Gas"];
        g2.push_back( Bins(0,0.018)     );
        g2.push_back( Bins(0.018,0.026) );
        g2.push_back( Bins(0.026,1)     );
      }
      return ckBinned;
    }

    void makeCKHist( TFile * file,
                     TCanvas * canvas,
                     const std::string & hPath,
                     const std::string & title,
                     const std::string & fileName,
                     const std::string & radiator )
    {
      TH1F * h = (TH1F*)file->Get(hPath.c_str());
      if (h)
      {
        TF1 * fitter = new TF1("CKFit", "gaus", -fitLimits()[radiator],fitLimits()[radiator]);
        const std::string xName( radiator+" CK theta resolution / mrad" );
        h->GetXaxis()->SetTitle(xName.c_str());
        h->SetTitle(title.c_str());
        h->Fit(fitter,"R");
        RichUtils::systemSaveImage( canvas, fileName );
        delete fitter;
      } else { cerr << "Histogram '" << hPath << "' does not exist" << endl; }
    }

    void fitCkResVtheta_bin( const std::string & radiator,
                             const double binMin,
                             const double binMax,
                             TTree * tree,
                             double & res,
                             double & reserr,
                             double & ckMean )
    {

      // fit limits
      const double lim = (fitLimits())[radiator];

      // fit function
      TF1 * fitFunc = new TF1("CKFit", "gaus", -lim, lim );

      // make selection
      TString cutString = "RecoCKtheta>";
      cutString += binMin;
      cutString += " && RecoCKtheta<";
      cutString += binMax;
      TCut ckSel( cutString );

      // Get mean p in this bin
      TString meanPhistName = "MeanCkt";
      meanPhistName += uniqueHnumber();
      TH1F * myPmean = new TH1F(meanPhistName,meanPhistName,100,binMin,binMax);
      tree->Draw("RecoCKtheta>>"+meanPhistName,ckSel);
      ckMean = myPmean->GetMean(1);
      cout << " -> Mean CK theta = " << ckMean << " Entries = " << myPmean->GetEntries() << endl;
      delete myPmean;

      // Fit for CK res
      TString fitHistName = "myHist";
      fitHistName += uniqueHnumber();
      TH1F * myHist = new TH1F(fitHistName,fitHistName,100,-2*lim,2*lim);
      tree->Draw("RecoCKtheta-ExpCKtheta>>"+fitHistName,ckSel);
      myHist->Fit(fitFunc,"R");
      res    = fitFunc->GetParameter(2);
      reserr = fitFunc->GetParError(2);
      cout << " -> Fitted res = " << res << " +- " << reserr << endl;

      delete fitFunc;

    }

    void fitCkResVtheta_Imp( TFile * file,
                             const std::string & prodTag,
                             const std::string & tupleName,
                             const std::string & radiator,
                             const std::string & trackType,
                             const std::string & tag,
                             const BinsList & bins )
    {
      if (!file) return;
      cout << "Making CK res V p plot for " << tupleName << endl;

      TTree * tree = (TTree*)file->Get(tupleName.c_str());
      if ( !tree )
      { cerr << "Failed to open TTree " << tupleName << endl; return; }

      std::vector<double> vx,vy,vxerr,vyerr;

      for ( BinsList::const_iterator iB = bins.begin(); iB != bins.end(); ++iB )
      {
        // bins
        const double binMin(iB->first), binMax(iB->second);
        cout << "Fitting CK bin " << binMin << " -> " << binMax << endl;

        // do the fit
        double res(0), reserr(0), ckMean(0);
        fitCkResVtheta_bin( radiator, binMin, binMax, tree, res, reserr, ckMean );

        // store results
        vx.push_back(ckMean);
        vxerr.push_back(0);
        vy.push_back(res);
        vyerr.push_back(reserr);

      }

      // create output options files and fill annoying TVectorD for graph

      gSystem->Exec( ("mkdir -p "+tag+"-Options").c_str() );
      {
        const std::string filename = tag+"-Options/CKRes_" + trackType + "_" + radiator + "interpTool.opts";
        gSystem->Exec( ("rm "+filename).c_str() );
        std::ofstream optsfile(filename.c_str(),std::ios::app);
        optsfile << "ToolSvc.Offline.RichCherenkovResolution." << trackType << radiator << " = {";
        std::vector<double>::const_iterator iX = vx.begin();
        std::vector<double>::const_iterator iY = vy.begin();
        for ( ; iX != vx.end() && iY != vy.end(); ++iX, ++iY )
        {
          if ( iX != vx.begin() ) optsfile << ",";
          optsfile << " (" << *iX << "," << *iY << ")";
        }
        optsfile << " };" << endl;
      }
      {
        const std::string filename = tag+"-Options/CKRes_" + trackType + "_" + radiator + "BinnedTool.opts";
        gSystem->Exec( ("rm "+filename).c_str() );
        std::ofstream optsfile(filename.c_str(),std::ios::app);
        optsfile << "ToolSvc.Offline.RichCherenkovResolution.N" << radiator << "ResBins = {";
        for ( BinsList::const_iterator iB = bins.begin(); iB != bins.end(); ++iB )
        {
          if ( iB != bins.begin() ) optsfile << ",";
          optsfile << " (" << iB->first << "," << iB->second << ")";
        }
        optsfile << " };" << endl;
        optsfile << "ToolSvc.Offline.RichCherenkovResolution." << trackType << radiator << "Res = {";
        std::vector<double>::const_iterator iY = vy.begin();
        for ( ; iY != vy.end(); ++iY )
        {
          if ( iY != vy.begin() ) optsfile << ",";
          optsfile << " " << *iY;
        }
        optsfile << " };" << endl;
      }

      // create output figures
      const std::string fileName = prodTag+"/"+trackType+"-"+radiator+"-CKresVtheta-"+tag;
      TGraphErrors * g = new TGraphErrors( vx.size(),
                                           &*vx.begin(), &*vy.begin(),
                                           &*vxerr.begin(), &*vyerr.begin() );
      g->SetMarkerStyle(21);
      g->SetTitle( (prodTag+" : "+trackType+" "+radiator+" : "+tag).c_str() );
      g->GetXaxis()->SetTitle("Cherenkov Theta / rad");
      g->GetYaxis()->SetTitle("Cherenkov Theta Resolution / rad");
      TCanvas * canvas = new TCanvas("c1","MOO",200,10,600,400);
      g->Draw("AL*");
      RichUtils::systemSaveImage( canvas, fileName+".eps" );
      RichUtils::systemSaveImage( canvas, fileName+".png" );

    }

    void fitCkResVtheta( TFile * file,
                         const std::string & prodTag,
                         const std::string & tupleName,
                         const std::string & radiator,
                         const std::string & trackType,
                         const unsigned int nBins )
    {
      // CK range
      const double ckMin = CKMin()[radiator];
      const double ckMax = CKMax()[radiator];
      const double ckRange = (ckMax-ckMin)/(double)nBins;

      // construct the bins
      BinsList bins;
      for ( unsigned int i = 0; i < nBins; ++i )
      {
        // make the limits
        const double binCen = ckMin + ( (double)i * (ckMax-ckMin)/(double)nBins );
        const double binMin = binCen - ckRange;
        const double binMax = binCen + ckRange;
        bins.push_back( Bins(binMin,binMax) );
      }

      // do the fitting
      fitCkResVtheta_Imp( file, prodTag, tupleName, radiator, trackType, "ConstBins", bins );

    }

    void binnedCKres( TFile * file,
                      const std::string & prodTag,
                      const std::string & tupleName,
                      const std::string & radiator,
                      const std::string & trackType )
    {
      const BinsList & bins = binnedCKbins()[radiator];
      fitCkResVtheta_Imp( file, prodTag, tupleName, radiator, trackType, "VaryBins", bins );
    }

    void ckResVtheta( TFile * file,
                      const std::string & prodTag,
                      const std::string & radiator )
    {
      if (!file) return;

      // turn off histo data
      //gStyle->SetOptStat(0);
      // turn on some fit data
      gStyle->SetOptFit(1011);

      const unsigned int nBins = 4;

      // interpolated stuff
      fitCkResVtheta(file,prodTag,"RiRecPhotAngFrwdTkMoni/"+radiator+"/ckResTuple",radiator,"Forward",nBins);
      fitCkResVtheta(file,prodTag,"RiRecPhotAngMatchTkMoni/"+radiator+"/ckResTuple",radiator,"Match",nBins);
      fitCkResVtheta(file,prodTag,"RiRecPhotAngVTTTkMoni/"+radiator+"/ckResTuple",radiator,"VeloTT",nBins);
      fitCkResVtheta(file,prodTag,"RiRecPhotAngSeedTkMoni/"+radiator+"/ckResTuple",radiator,"Seed",nBins);
      fitCkResVtheta(file,prodTag,"RiRecPhotAngKsTkMoni/"+radiator+"/ckResTuple",radiator,"KsTrack",nBins);

      // binned stuff
      binnedCKres(file,prodTag,"RiRecPhotAngFrwdTkMoni/"+radiator+"/ckResTuple",radiator,"Forward");
      binnedCKres(file,prodTag,"RiRecPhotAngMatchTkMoni/"+radiator+"/ckResTuple",radiator,"Match");
      binnedCKres(file,prodTag,"RiRecPhotAngVTTTkMoni/"+radiator+"/ckResTuple",radiator,"VeloTT");
      binnedCKres(file,prodTag,"RiRecPhotAngSeedTkMoni/"+radiator+"/ckResTuple",radiator,"Seed");
      binnedCKres(file,prodTag,"RiRecPhotAngKsTkMoni/"+radiator+"/ckResTuple",radiator,"KsTrack");

    }

    void ckRes( TFile * file,
                const std::string & prodTag,
                const std::string & nameTag,
                const std::string & imageType,
                const std::string & radiator )
    {
      if (!file) return;

      // turn off histo data
      //gStyle->SetOptStat(0);
      // turn on some fit data
      gStyle->SetOptFit(1011);

      // make a canvas to work with
      TCanvas * canvas = new TCanvas((radiator+" CKRes").c_str(),(radiator+" CKRes").c_str());

      //  CK res forward tracks
      {
        const std::string hPath("RICH/RiRecPhotAngFrwdTkMoni/"+radiator+"/ckDiffTrue");
        const std::string title("Forward Tracks"+nameTag);
        const std::string fileName(prodTag+"/"+prodTag+"_"+radiator+"-CKRes_FrwdTracks_rec-exp."+imageType);
        makeCKHist(file,canvas,hPath,title,fileName,radiator);
      }
      {
        //  CK res match tracks
        const std::string hPath("RICH/RiRecPhotAngMatchTkMoni/"+radiator+"/ckDiffTrue");
        const std::string title("Match Tracks"+nameTag);
        const std::string fileName(prodTag+"/"+prodTag+"_"+radiator+"-CKRes_MatchTracks_rec-exp."+imageType);
        makeCKHist(file,canvas,hPath,title,fileName,radiator);
      }

      {
        //  CK res seed tracks
        const std::string hPath("RICH/RiRecPhotAngSeedTkMoni/"+radiator+"/ckDiffTrue");
        const std::string title("Seed Tracks"+nameTag);
        const std::string fileName(prodTag+"/"+prodTag+"_"+radiator+"-CKRes_SeedTracks_rec-exp."+imageType);
        makeCKHist(file,canvas,hPath,title,fileName,radiator);
      }

      {
        // CK res VTT tracks
        const std::string hPath("RICH/RiRecPhotAngVTTTkMoni/"+radiator+"/ckDiffTrue");
        const std::string title("VeloTT Tracks"+nameTag);
        const std::string fileName(prodTag+"/"+prodTag+"_"+radiator+"-CKRes_VeloTTTracks_rec-exp."+imageType);
        makeCKHist(file,canvas,hPath,title,fileName,radiator);
      }

      {
        // CK res Ks tracks
        const std::string hPath("RICH/RiRecPhotAngKsTkMoni/"+radiator+"/ckDiffTrue");
        const std::string title("KsTrack Tracks"+nameTag);
        const std::string fileName(prodTag+"/"+prodTag+"_"+radiator+"-CKRes_KsTkTracks_rec-exp."+imageType);
        makeCKHist(file,canvas,hPath,title,fileName,radiator);
      }

    }

  }
}
