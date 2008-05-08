
#include "RichTrackResolutions.h"

namespace Rich
{

  // RICH CK resolution functions
  namespace TrackResolution
  {
    void doHist( TFile * file,
                 TCanvas * canvas,
                 const std::string hPath,
                 const std::string title,
                 const std::string xtitle,
                 const std::string fileName,
                 const double Xmin = 0,
                 const double Xmax = 0,
                 const double Ymin = 0,
                 const double Ymax = 0,
                 const bool applyMinMax = false )
    {
      TH1F * h = (TH1F*)file->Get( hPath.c_str() );
      if (h)
      {
        h->SetTitle(title.c_str());
        h->GetXaxis()->SetTitle(xtitle.c_str());
        if ( applyMinMax )
        {
          h->SetMaximum(Ymax);
          h->SetMinimum(Ymin);
          h->GetXaxis()->SetRangeUser(Xmin,Xmax);
        }
        h->Draw();
        Rich::Utils::systemSaveImage( canvas, fileName );
      } else { cerr << "Histogram '" << hPath << "' does not exist" << endl; }
    }

    void fitHist( TFile * file,
                  TCanvas * canvas,
                  const std::string hPath,
                  const std::string title,
                  const std::string xtitle,
                  const std::string fileName )
    {
      TH1F * h = (TH1F*)file->Get( hPath.c_str() );
      if (h)
      {
        TF1 * fitter = new TF1( "Fit", "gaus", -4, 4 );
        h->SetTitle(title.c_str());
        h->GetXaxis()->SetTitle(xtitle.c_str());
        h->Fit(fitter,"R");
        Rich::Utils::systemSaveImage( canvas, fileName );
        delete fitter;
      } else { cerr << "Histogram '" << hPath << "' does not exist" << endl; }
    }

    void doHists( TFile * file,
                  TCanvas * canvas,
                  const std::string & hPath,
                  const std::string & title,
                  const std::string & fileNameRoot,
                  const std::string & imageType,
                  const std::string & radiator )
    {

      canvas->SetLogy(1);

      doHist( file,canvas,
              hPath+"/dTrEntPtot",
              title,
              radiator+" Entry Point Momentum Resolution (Rec-MC) / MeV",
              fileNameRoot+"EntryPtotRes."+imageType );

      doHist( file,canvas,
              hPath+"/dTrExtPtot",
              title,
              radiator+" Exit Point Momentum Resolution (Rec-MC) / MeV",
              fileNameRoot+"ExitPtotRes."+imageType );

      doHist( file,canvas,
              hPath+"/dTrEntX",
              title,
              radiator+" Entry X Point Resolution (Rec-MC) / mm",
              fileNameRoot+"EntryXRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrEntY",
              title,
              radiator+" Entry Y Point Resolution (Rec-MC) / mm",
              fileNameRoot+"EntryYRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrEntZ",
              title,
              radiator+" Entry Z Point Resolution (Rec-MC) / mm",
              fileNameRoot+"EntryZRes."+imageType );

      doHist( file,canvas,
              hPath+"/dTrExtX",
              title,
              radiator+" Exit X Point Resolution (Rec-MC) / mm",
              fileNameRoot+"ExitXRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrExtY",
              title,
              radiator+" Exit Y Point Resolution (Rec-MC) / mm",
              fileNameRoot+"ExitYRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrExtZ",
              title,
              radiator+" Exit Z Point Resolution (Rec-MC) / mm",
              fileNameRoot+"ExitZRes."+imageType );

      doHist( file,canvas,
              hPath+"/dTrEntTX",
              title,
              radiator+" Entry X Slope Resolution (Rec-MC)",
              fileNameRoot+"EntryTXRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrEntTY",
              title,
              radiator+" Entry Y Slope Resolution (Rec-MC)",
              fileNameRoot+"EntryTYRes."+imageType );

      doHist( file,canvas,
              hPath+"/dTrExtTX",
              title,
              radiator+" Exit X Slope Resolution (Rec-MC)",
              fileNameRoot+"ExitTXRes."+imageType );
      doHist( file,canvas,
              hPath+"/dTrExtTY",
              title,
              radiator+" Exit Y Slope Resolution (Rec-MC)",
              fileNameRoot+"ExitTYRes."+imageType );

      canvas->SetLogy(0);
      // turn off histo data
      gStyle->SetOptStat(0);
      // turn on some fit data
      gStyle->SetOptFit(1011);

      fitHist( file,canvas,
               hPath+"/pullPEnt",
               title,
               radiator+" Entry Point Momentum Pull (Rec-MC)",
               fileNameRoot+"EntryPtotPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullPExt",
               title,
               radiator+" Exit Point Momentum Pull (Rec-MC)",
               fileNameRoot+"ExitPtotPull."+imageType );

      fitHist( file,canvas,
               hPath+"/pullXEnt",
               title,
               radiator+" Entry Point X Position Pull (Rec-MC)",
               fileNameRoot+"EntryXPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullYEnt",
               title,
               radiator+" Entry Point Y Position Pull (Rec-MC)",
               fileNameRoot+"EntryYPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullTXEnt",
               title,
               radiator+" Entry Point X Slope Pull (Rec-MC)",
               fileNameRoot+"EntryTXPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullTYEnt",
               title,
               radiator+" Entry Point Y Slope Pull (Rec-MC)",
               fileNameRoot+"EntryTYPull."+imageType );

      fitHist( file,canvas,
               hPath+"/pullXExt",
               title,
               radiator+" Exit Point X Position Pull (Rec-MC)",
               fileNameRoot+"ExitXPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullYExt",
               title,
               radiator+" Exit Point Y Position Pull (Rec-MC)",
               fileNameRoot+"ExitYPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullTXExt",
               title,
               radiator+" Exit Point X Slope Pull (Rec-MC)",
               fileNameRoot+"ExitTXPull."+imageType );
      fitHist( file,canvas,
               hPath+"/pullTYExt",
               title,
               radiator+" Exit Point Y Slope Pull (Rec-MC)",
               fileNameRoot+"ExitTYPull."+imageType );

      doHist( file,canvas,
              hPath+"/dPOvPVPVertex",
              title,
              radiator+" Vertex dP/P V P",
              fileNameRoot+"VertdPOPVP."+imageType,
              0, 100000, 0, 0.01, true );
      doHist( file,canvas,
              hPath+"/dPOvPVPEnt",
              title,
              radiator+" Rad. Entry dP/P V P",
              fileNameRoot+"EntrydPOPVP."+imageType,
              0, 100000, 0, 0.01, true );
      doHist( file,canvas,
              hPath+"/dPOvPVPExt",
              title,
              radiator+" Rad. Exit dP/P V P",
              fileNameRoot+"ExitdPOPVP."+imageType,
              0, 100000, 0, 0.01, true );


    }

    void trackRes( TFile * file,
                   const std::string & prodTag,
                   const std::string & nameTag,
                   const std::string & imageType,
                   const std::string & radiator )
    {
      if (!file) return;

      gStyle->SetOptStat("mr");

      // make a canvas to work with
      TCanvas * canvas = new TCanvas("TrackRes","TrackRes");

      // forward tracks
      {
        const std::string hPath("RICH/RiRecFrwdTkResMoni/"+radiator);
        const std::string title("Forward Tracks"+nameTag);
        const std::string fileNameRoot(prodTag+"/"+prodTag+"_ForwardTracks_"+radiator);
        doHists(file,canvas,hPath,title,fileNameRoot,imageType,radiator);
      }

      // match tracks
      {
        const std::string hPath("RICH/RiRecMatchTkResMoni/"+radiator);
        const std::string title("Match Tracks"+nameTag);
        const std::string fileNameRoot(prodTag+"/"+prodTag+"_MatchTracks_"+radiator);
        doHists(file,canvas,hPath,title,fileNameRoot,imageType,radiator);
      }

      // seed tracks
      {
        const std::string hPath("RICH/RiRecSeedTkResMoni/"+radiator);
        const std::string title("Seed Tracks"+nameTag);
        const std::string fileNameRoot(prodTag+"/"+prodTag+"_SeedTracks_"+radiator);
        doHists(file,canvas,hPath,title,fileNameRoot,imageType,radiator);
      }

      // VTT tracks
      {
        const std::string hPath("RICH/RiRecVTTTkResMoni/"+radiator);
        const std::string title("VeloTT Tracks"+nameTag);
        const std::string fileNameRoot(prodTag+"/"+prodTag+"_VeloTTTracks_"+radiator);
        doHists(file,canvas,hPath,title,fileNameRoot,imageType,radiator);
      }

      // Ks tracks
      {
        const std::string hPath("RICH/RiRecKsTkResMoni/"+radiator);
        const std::string title("Ks Tracks"+nameTag);
        const std::string fileNameRoot(prodTag+"/"+prodTag+"_KsTracks_"+radiator);
        doHists(file,canvas,hPath,title,fileNameRoot,imageType,radiator);
      }

    }

  }
}
