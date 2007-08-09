
#include "RichPIDPerformance.h"

namespace RICH
{
  namespace PIDPerformance
  {

    TH1F * loadHist( TFile * file,
                     const std::string & location )
    {
      TH1F * h = (TH1F*)file->Get( location.c_str() );
      if (!h) { cerr << "ERROR : Histogram '" << location << "' not found" << endl; }
      return h;
    }

    TH1F * heavyIDedKaons( TFile * file,
                           const std::string & location )
    {
      TH1F * h1 = loadHist( file, location+"/MC/144" );
      TH1F * h2 = loadHist( file, location+"/MC/145" );
      if ( !h1 || !h2 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone( "TMP1" );
      hout->Add(h2);
      return hout;
    }

    TH1F * lightIDedKaons( TFile * file,
                           const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/141");
      TH1F * h2 = loadHist ( file, location+"/MC/142");
      TH1F * h3 = loadHist ( file, location+"/MC/143");
      if ( !h1 || !h2 || !h3 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP2");
      hout->Add(h2);
      hout->Add(h3);
      return hout;
    }

    TH1F * lightIDedPions( TFile * file,
                           const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/131" );
      TH1F * h2 = loadHist ( file, location+"/MC/132" );
      TH1F * h3 = loadHist ( file, location+"/MC/133" );
      if ( !h1 || !h2 || !h3 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP3");
      hout->Add(h2);
      hout->Add(h3);
      return hout;
    }

    TH1F * heavyIDedPions( TFile * file,
                           const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/134" );
      TH1F * h2 = loadHist ( file, location+"/MC/135" );
      if ( !h1 || !h2 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP4");
      hout->Add(h2);
      return hout;
    }

    TH1F * allTruePions( TFile * file,
                         const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/131" );
      TH1F * h2 = loadHist ( file, location+"/MC/132" );
      TH1F * h3 = loadHist ( file, location+"/MC/133" );
      TH1F * h4 = loadHist ( file, location+"/MC/134" );
      TH1F * h5 = loadHist ( file, location+"/MC/135" );
      TH1F * h6 = loadHist ( file, location+"/MC/136" );
      if ( !h1 || !h2 || !h2 || !h3 || !h4 || !h5 || !h6 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP5");
      hout->Add(h2);
      hout->Add(h3);
      hout->Add(h4);
      hout->Add(h5);
      hout->Add(h6);
      return hout;
    }

    TH1F * allTrueKaons( TFile * file,
                         const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/141" );
      TH1F * h2 = loadHist ( file, location+"/MC/142" );
      TH1F * h3 = loadHist ( file, location+"/MC/143" );
      TH1F * h4 = loadHist ( file, location+"/MC/144" );
      TH1F * h5 = loadHist ( file, location+"/MC/145" );
      TH1F * h6 = loadHist ( file, location+"/MC/146" );
      if ( !h1 || !h2 || !h2 || !h3 || !h4 || !h5 || !h6 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP6");
      hout->Add(h2);
      hout->Add(h3);
      hout->Add(h4);
      hout->Add(h5);
      //hout->Add(h6);
      return hout;
    }

    TH1F * allTrueProtons( TFile * file,
                           const std::string & location )
    {
      TH1F * h1 = loadHist ( file, location+"/MC/151" );
      TH1F * h2 = loadHist ( file, location+"/MC/152" );
      TH1F * h3 = loadHist ( file, location+"/MC/153" );
      TH1F * h4 = loadHist ( file, location+"/MC/154" );
      TH1F * h5 = loadHist ( file, location+"/MC/155" );
      TH1F * h6 = loadHist ( file, location+"/MC/156" );
      if ( !h1 || !h2 || !h2 || !h3 || !h4 || !h5 || !h6 ) return 0;
      TH1F * hout = (TH1F*)h1->Clone("TMP7");
      hout->Add(h2);
      hout->Add(h3);
      hout->Add(h4);
      hout->Add(h5);
      hout->Add(h6);
      return hout;
    }

    TH1F * protonIDedProtons( TFile * file,
                              const std::string & location )
    {
      return loadHist ( file, location+"/MC/155" );
    }

    TH1F * kaonIDedKaons( TFile * file,
                          const std::string & location )
    {
      return loadHist ( file, location+"/MC/144" );
    }

    TH1F * pionIDedPions( TFile * file,
                          const std::string & location )
    {
      return loadHist ( file, location+"/MC/133" );
    }

    TH1F * kaonIDedPions( TFile * file,
                          const std::string & location )
    {
      return loadHist ( file, location+"/MC/134" );
    }

    TH1F * pionIDedKaons( TFile * file,
                          const std::string & location )
    {
      return loadHist ( file, location+"/MC/143" );
    }


    TH1F * protonIDedKaons( TFile * file,
                            const std::string & location )
    {
      return loadHist ( file, location+"/MC/145" );
    }

    std::string averageEff( TH1F * top, TH1F * bot )
    {
      if ( top && bot )
      {
        const double eff = ( bot->GetEntries()>0 ? top->GetEntries()/bot->GetEntries() : 0 );
        const double err = ( bot->GetEntries()>0 ? sqrt(eff*(1-eff)/bot->GetEntries()) : 0 );
        char buffer[50];
        sprintf (buffer, "%4.2f #pm %4.2f", 100*eff, 100*err );
        return std::string(buffer);
      } else { return ""; }
    }

    void lightIDPerf ( TFile * file,
                       const std::string & prodTag,
                       const std::string & location,
                       const std::string & titleEx,
                       const std::string & imageName,
                       const std::vector<std::string> & imageType )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      TCanvas * canvas = new TCanvas("LightID","LightID");

      // Light IDed pions
      TH1F * top = lightIDedPions( file,location );
      if (!top) { cerr << "ERROR : Failed to load lightIDedPions " << location << endl; }
      // all pions
      TH1F * bot = allTruePions( file,location );
      if (!bot) { cerr << "ERROR : Failed to load allTruePions " << location << endl; }
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Light ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->DrawCopy();

      delete top;
      delete bot;

      // Light MisIDed kaons
      top = lightIDedKaons( file,location );
      if (!top) { cerr << "ERROR : Failed to load lightIDedKaons " << location << endl; }
      // all kaons
      bot = allTrueKaons( file,location );
      if (!bot) { cerr << "ERROR : Failed to load allTrueKaons " << location << endl; }
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->DrawCopy("same");

      // Text labels
      TLatex * idLabel = new TLatex();
      idLabel->SetNDC();
      idLabel->SetTextSize(0.04);
      idLabel->SetTextColor(kRed);
      idLabel->DrawLatex( 0.35, 0.52, ("#pi #rightarrow e, #mu, #pi : "+eff+" %").c_str() );
      idLabel->SetTextColor(kBlue);
      idLabel->DrawLatex( 0.35, 0.47, ("K #rightarrow e, #mu, #pi : "+miseff+" %").c_str() );

      // save image file
      for ( std::vector<std::string>::const_iterator iType = imageType.begin();
            iType != imageType.end(); ++iType )
      {
        RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_lightID-"+imageName+"."+(*iType));
      }

      delete top;
      delete bot;

    }

    void heavyIDPerf ( TFile * file,
                       const std::string & prodTag,
                       const std::string & location,
                       const std::string & titleEx,
                       const std::string & imageName,
                       const std::vector<std::string> & imageType,
                       const bool superImpose = false,
                       const bool plotLabels  = true,
                       const bool saveImage = true,
                       const int markType = 20,
                       const double xEff  = 0.35,
                       const double yEff1 = 0.52,
                       const double yEff2 = 0.47,
                       const std::string extraLabel = "" )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      static TCanvas * canvas;
      if (!superImpose) canvas = new TCanvas("HeavyID","HeavyID");

      // Heavy IDed kaons
      TH1F * top = heavyIDedKaons( file,location );
      // all kaons
      TH1F * bot = allTrueKaons( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Heavy ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->SetMarkerStyle(markType);
      if (superImpose)
      {
        top->DrawCopy("same");
      }
      else
      {
        top->DrawCopy();
      }

      delete top;
      delete bot;

      // heavy misIDed pions
      top = heavyIDedPions( file,location );
      // all pions
      bot = allTruePions( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->SetMarkerStyle(markType);
      top->DrawCopy("same");

      // Text labels
      if ( plotLabels )
      {
        TLatex * idLabel = new TLatex();
        idLabel->SetNDC();
        idLabel->SetTextSize(0.04);
        idLabel->SetTextColor(kRed);
        idLabel->DrawLatex( xEff, yEff1, (extraLabel+"K #rightarrow K, Pr : "+eff+" %").c_str() );
        idLabel->SetTextColor(kBlue);
        idLabel->DrawLatex( xEff, yEff2, (extraLabel+"#pi #rightarrow K, Pr : "+miseff+" %").c_str() );
      }
      // save to file
      if ( saveImage )
      {
        for ( std::vector<std::string>::const_iterator iType = imageType.begin();
              iType != imageType.end(); ++iType )
        {
          RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_heavyID-"+imageName+"."+(*iType) );
        }
      }

      delete top;
      delete bot;

    }

    void electronIDPerf ( TFile * file,
                          const std::string & prodTag,
                          const std::string & location,
                          const std::string & titleEx,
                          const std::string & imageName,
                          const std::vector<std::string> & imageType,
                          const bool superImpose = false,
                          const bool plotLabels  = true,
                          const bool saveImage = true,
                          const int markType = 20,
                          const double xEff = 0.35,
                          const double yEff1 = 0.52,
                          const double yEff2 = 0.47,
                          const std::string extraLabel = "" )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      static TCanvas * canvas;
      if (!superImpose) canvas = new TCanvas("KaonID","KaonID");

      // Kaon IDed kaons
      TH1F * top = kaonIDedKaons( file,location );
      // all kaons
      TH1F * bot = allTrueKaons( file,location );

      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Kaon ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->SetMarkerStyle(markType);
      if (superImpose)
      {
        top->DrawCopy("same");
      }
      else
      {
        top->DrawCopy();
      }

      delete top;
      delete bot;

      // mis IDed pions
      top = kaonIDedPions( file,location );
      // all pions
      bot = allTruePions( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->SetMarkerStyle(markType);
      top->DrawCopy("same");

      // Text labels
      if ( plotLabels )
      {
        TLatex * idLabel = new TLatex();
        idLabel->SetNDC();
        idLabel->SetTextSize(0.04);
        idLabel->SetTextColor(kRed);
        idLabel->DrawLatex( xEff, yEff1, (extraLabel+"K #rightarrow K : "+eff+" %").c_str() );
        idLabel->SetTextColor(kBlue);
        idLabel->DrawLatex( xEff, yEff2, (extraLabel+"#pi #rightarrow K : "+miseff+" %").c_str() );
      }
      // save to file
      if ( saveImage )
      {
        for ( std::vector<std::string>::const_iterator iType = imageType.begin();
              iType != imageType.end(); ++iType )
        {
          RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_kaonID-"+imageName+"."+(*iType) );
        }
      }

      delete top;
      delete bot;

    }

    void kaonIDPerf ( TFile * file,
                      const std::string & prodTag,
                      const std::string & location,
                      const std::string & titleEx,
                      const std::string & imageName,
                      const std::vector<std::string> & imageType,
                      const bool superImpose = false,
                      const bool plotLabels  = true,
                      const bool saveImage = true,
                      const int markType = 20,
                      const double xEff = 0.35,
                      const double yEff1 = 0.52,
                      const double yEff2 = 0.47,
                      const std::string extraLabel = "" )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      static TCanvas * canvas;
      if (!superImpose) canvas = new TCanvas("KaonID","KaonID");

      // Kaon IDed kaons
      TH1F * top = kaonIDedKaons( file,location );
      // all kaons
      TH1F * bot = allTrueKaons( file,location );

      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Kaon ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->SetMarkerStyle(markType);
      if (superImpose)
      {
        top->DrawCopy("same");
      }
      else
      {
        top->DrawCopy();
      }

      delete top;
      delete bot;

      // mis IDed pions
      top = kaonIDedPions( file,location );
      // all pions
      bot = allTruePions( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->SetMarkerStyle(markType);
      top->DrawCopy("same");

      // Text labels
      if ( plotLabels )
      {
        TLatex * idLabel = new TLatex();
        idLabel->SetNDC();
        idLabel->SetTextSize(0.04);
        idLabel->SetTextColor(kRed);
        idLabel->DrawLatex( xEff, yEff1, (extraLabel+"K #rightarrow K : "+eff+" %").c_str() );
        idLabel->SetTextColor(kBlue);
        idLabel->DrawLatex( xEff, yEff2, (extraLabel+"#pi #rightarrow K : "+miseff+" %").c_str() );
      }
      // save to file
      if ( saveImage )
      {
        for ( std::vector<std::string>::const_iterator iType = imageType.begin();
              iType != imageType.end(); ++iType )
        {
          RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_kaonID-"+imageName+"."+(*iType) );
        }
      }

      delete top;
      delete bot;

    }

    void pionIDPerf ( TFile * file,
                      const std::string & prodTag,
                      const std::string & location,
                      const std::string & titleEx,
                      const std::string & imageName,
                      const std::vector<std::string> & imageType )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      TCanvas * canvas = new TCanvas("PionID","PionID");

      // pion IDed pions
      TH1F * top = pionIDedPions( file,location );
      // all pions
      TH1F * bot = allTruePions( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Pion ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->DrawCopy();

      delete top;
      delete bot;

      // Light MisIDed particles
      top = pionIDedKaons( file,location );
      // all pions
      bot = allTrueKaons( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->DrawCopy("same");

      // Text labels
      TLatex * idLabel = new TLatex();
      idLabel->SetNDC();
      idLabel->SetTextSize(0.04);
      idLabel->SetTextColor(kRed);
      idLabel->DrawLatex( 0.35, 0.52, ("#pi #rightarrow #pi : "+eff+" %").c_str() );
      idLabel->SetTextColor(kBlue);
      idLabel->DrawLatex( 0.35, 0.47, ("K #rightarrow #pi : "+miseff+" %").c_str() );

      // save image file
      for ( std::vector<std::string>::const_iterator iType = imageType.begin();
            iType != imageType.end(); ++iType )
      {
        RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_pionID-"+imageName+"."+(*iType) );
      }

      delete top;
      delete bot;

    }

    void protonIDPerf ( TFile * file,
                        const std::string & prodTag,
                        const std::string & location,
                        const std::string & titleEx,
                        const std::string & imageName,
                        const std::vector<std::string> & imageType )
    {
      if (!file) { cout << "Invalid file pointer" << endl; return; }

      // turn off histo data
      gStyle->SetOptStat(0);
      // turn off fit data
      gStyle->SetOptFit(0);

      // make a canvas to work with
      TCanvas * canvas = new TCanvas("ProtonID","ProtonID");

      // Kaon IDed kaons
      TH1F * top = protonIDedProtons( file,location );
      // all kaons
      TH1F * bot = allTrueProtons( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string eff = averageEff(top,bot);

      // ID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetMaximum(105);
      top->SetMinimum(0);
      top->SetLineColor(kRed);
      top->SetMarkerColor(kRed);
      top->SetTitle( ("Proton ID | "+titleEx).c_str() );
      top->GetXaxis()->SetTitle("Momentum / GeV/c");
      top->GetYaxis()->SetTitle("Efficiency / %");
      top->DrawCopy();

      delete top;
      delete bot;

      // proton Ided kaons
      top = protonIDedKaons( file,location );
      // all kaons
      bot = allTrueKaons( file,location );
      if ( !top || !bot )
      { cerr << "Error loading histos for " << file->GetName() << " " << location << endl; return; }

      // work out average efficiency
      const std::string miseff = averageEff(top,bot);

      // MisID efficiency
      top->Divide(top,bot,1,1,"B");
      top->Scale(100); // in percent
      top->SetLineColor(kBlue);
      top->SetMarkerColor(kBlue);
      top->DrawCopy("same");

      // Text labels
      TLatex * idLabel = new TLatex();
      idLabel->SetNDC();
      idLabel->SetTextSize(0.04);
      idLabel->SetTextColor(kRed);
      idLabel->DrawLatex( 0.35, 0.52, ("Pr #rightarrow Pr : "+eff+" %").c_str() );
      idLabel->SetTextColor(kBlue);
      idLabel->DrawLatex( 0.35, 0.47, ("K #rightarrow Pr : "+miseff+" %").c_str() );

      // save image file
      for ( std::vector<std::string>::const_iterator iType = imageType.begin();
            iType != imageType.end(); ++iType )
      {
        RichUtils::systemSaveImage( canvas, prodTag+"/"+prodTag+"_protonID-"+imageName+"."+(*iType) );
      }

      delete top;
      delete bot;

    }

    void allPIDMeasures( TFile * file,
                         const std::string & prodTag,
                         const std::string & location,
                         const std::string & titleEx,
                         const std::string & imageName,
                         const std::vector<std::string> & imageType )
    {
      heavyIDPerf  ( file, prodTag, location, titleEx, imageName, imageType );
      lightIDPerf  ( file, prodTag, location, titleEx, imageName, imageType );
      kaonIDPerf   ( file, prodTag, location, titleEx, imageName, imageType );
      pionIDPerf   ( file, prodTag, location, titleEx, imageName, imageType );
      protonIDPerf ( file, prodTag, location, titleEx, imageName, imageType );
    }

    void posNegHeavyIDCompare( TFile * file,
                               const std::string & prodTag,
                               const std::string & titleEx,
                               const std::string & imageName,
                               const std::vector<std::string> & imageType )
    {
      heavyIDPerf ( file, prodTag, "RICH/PIDQC/ALL/POSITIVE/2TO100", titleEx, imageName, imageType, false, true, true, 20, 0.3, 0.57, 0.47, "#bullet +ve " );
      heavyIDPerf ( file, prodTag, "RICH/PIDQC/ALL/NEGATIVE/2TO100", titleEx, imageName, imageType, true,  true, true, 24, 0.3, 0.52, 0.42, "#circ -ve " );
      kaonIDPerf  ( file, prodTag, "RICH/PIDQC/ALL/POSITIVE/2TO100", titleEx, imageName, imageType, false, true, true, 20, 0.3, 0.57, 0.47, "#bullet +ve " );
      kaonIDPerf  ( file, prodTag, "RICH/PIDQC/ALL/NEGATIVE/2TO100", titleEx, imageName, imageType, true,  true, true, 24, 0.3, 0.52, 0.42, "#circ -ve " );
    }

    void allTracks( TFile * pFile,
                    const std::string & prodTag,
                    const std::string & titletag,
                    const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/ALL/2TO100", "All Tracks"+titletag, "all_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/ALL/2TO10", "All Tracks"+titletag,  "all_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/ALL/10TO70", "All Tracks"+titletag, "all_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/ALL/70TO100", "All Tracks"+titletag, "all_70to100GeV", imageType );
      // pos/neg
      posNegHeavyIDCompare( pFile, prodTag, "All Tracks", "all_2to100GeV_posneg", imageType );
    }

    void forwardTracks( TFile * pFile,
                        const std::string & prodTag,
                        const std::string & titletag,
                        const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/FRWD/2TO100", "Forward Tracks"+titletag, "frwd_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/FRWD/2TO10", "Forward Tracks"+titletag,  "frwd_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/FRWD/10TO70", "Forward Tracks"+titletag, "frwd_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/FRWD/70TO100", "Forward Tracks"+titletag, "frwd_70to100GeV", imageType );
    }

    void matchTracks( TFile * pFile,
                      const std::string & prodTag,
                      const std::string & titletag,
                      const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/MATCH/2TO100", "Match Tracks"+titletag, "match_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/MATCH/2TO10", "Match Tracks"+titletag,  "match_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/MATCH/10TO70", "Match Tracks"+titletag, "match_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/MATCH/70TO100", "Match Tracks"+titletag, "match_70to100GeV", imageType );
    }

    void longTracks( TFile * pFile,
                     const std::string & prodTag,
                     const std::string & titletag,
                     const std::vector<std::string> & imageType )
    {

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1/2TO100", "Long Tracks"+titletag, "long_dll1_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1/2TO10", "Long Tracks"+titletag,  "long_dll1_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1/10TO70", "Long Tracks"+titletag, "long_dll1_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1/70TO100", "Long Tracks"+titletag, "long_dll1_70to100GeV", imageType );

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1PT05/2TO100", "Long Tracks"+titletag, "long_dll1pt05_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1PT05/2TO10", "Long Tracks"+titletag,  "long_dll1pt05_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1PT05/10TO70", "Long Tracks"+titletag, "long_dll1pt05_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL1PT05/70TO100", "Long Tracks"+titletag, "long_dll1pt05_70to100GeV", imageType );

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2/2TO100", "Long Tracks"+titletag, "long_dll2_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2/2TO10", "Long Tracks"+titletag,  "long_dll2_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2/10TO70", "Long Tracks"+titletag, "long_dll2_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2/70TO100", "Long Tracks"+titletag, "long_dll2_70to100GeV", imageType );

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2PT05/2TO100", "Long Tracks"+titletag, "long_dll2pt05_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2PT05/2TO10", "Long Tracks"+titletag,  "long_dll2pt05_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2PT05/10TO70", "Long Tracks"+titletag, "long_dll2pt05_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL2PT05/70TO100", "Long Tracks"+titletag, "long_dll2pt05_70to100GeV", imageType );

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL5/2TO100", "Long Tracks"+titletag, "long_dll5_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL5/2TO10", "Long Tracks"+titletag,  "long_dll5_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL5/10TO70", "Long Tracks"+titletag, "long_dll5_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGDLL5/70TO100", "Long Tracks"+titletag, "long_dll5_70to100GeV", imageType );

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONG/2TO100", "Long Tracks"+titletag, "long_2to100GeV", imageType );
      // 3 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONG/3TO100", "Long Tracks"+titletag, "long_3to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONG/2TO10", "Long Tracks"+titletag,  "long_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONG/10TO70", "Long Tracks"+titletag, "long_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONG/70TO100", "Long Tracks"+titletag, "long_70to100GeV", imageType );
   

      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGPT05/2TO100", "Long Tracks"+titletag, "long_pt05_2to100GeV", imageType );
      // 3 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGPT05/3TO100", "Long Tracks"+titletag, "long_pt05_3to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGPT05/2TO10", "Long Tracks"+titletag,  "long_pt05_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGPT05/10TO70", "Long Tracks"+titletag, "long_pt05_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/LONGPT05/70TO100", "Long Tracks"+titletag, "long_pt05_70to100GeV", imageType );
 

    }

    void seedTracks( TFile * pFile,
                     const std::string & prodTag,
                     const std::string & titletag,
                     const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/SEED/2TO100", "Seed Tracks"+titletag, "seed_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/SEED/2TO10", "Seed Tracks"+titletag,  "seed_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/SEED/10TO70", "Seed Tracks"+titletag, "seed_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/SEED/70TO100", "Seed Tracks"+titletag, "seed_70to100GeV", imageType );
    }

    void ksTracks( TFile * pFile,
                   const std::string & prodTag,
                   const std::string & titletag,
                   const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/KSTRACKS/2TO100", "Ks Tracks"+titletag, "kstracks_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/KSTRACKS/2TO10", "Ks Tracks"+titletag,  "kstracks_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/KSTRACKS/10TO70", "Ks Tracks"+titletag, "kstracks_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/KSTRACKS/70TO100", "Ks Tracks"+titletag, "kstracks_70to100GeV", imageType );
    }

    void vttTracks( TFile * pFile,
                    const std::string & prodTag,
                    const std::string & titletag,
                    const std::vector<std::string> & imageType )
    {
      // 2 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/VTT/2TO100", "VTT Tracks"+titletag, "vtt_2to100GeV", imageType );
      // 2 - 10 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/VTT/2TO10", "VTT Tracks"+titletag,  "vtt_2to10GeV",  imageType );
      // 10 - 70 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/VTT/10TO70", "VTT Tracks"+titletag, "vtt_10to70GeV", imageType );
      // 70 - 100 GeV
      allPIDMeasures( pFile, prodTag, "RICH/PIDQC/VTT/70TO100", "VTT Tracks"+titletag, "vtt_70to100GeV", imageType );
    }

    void allPlots( TFile * pFile,
                   const std::string & filetag,
                   const std::string & titletag,
                   const std::vector<std::string> & imageType )
    {
      gSystem->Exec( ("mkdir -p "+filetag).c_str() );
      // make the plots
      allTracks     ( pFile, filetag, titletag, imageType );
      longTracks    ( pFile, filetag, titletag, imageType );
      forwardTracks ( pFile, filetag, titletag, imageType );
      matchTracks   ( pFile, filetag, titletag, imageType );
      seedTracks    ( pFile, filetag, titletag, imageType );
      vttTracks     ( pFile, filetag, titletag, imageType );
      ksTracks      ( pFile, filetag, titletag, imageType );
    }

    // shortcut methods

    void compareRads( const std::string & tag,
                      const std::string & filetag,
                      const std::string & aerofile,
                      const std::string & c4f10file,
                      const std::string & cf4file,
                      const std::string & allfile,
                      const std::vector<std::string> & imageType )
    {
      // load files
      TFile * pFile1 = TFile::Open(allfile.c_str());
      TFile * pFile2 = TFile::Open(aerofile.c_str());
      TFile * pFile3 = TFile::Open(c4f10file.c_str());
      TFile * pFile4 = TFile::Open(cf4file.c_str());

      // make the png figure
      heavyIDPerf ( pFile2, tag, "RICH/PIDQC/FRWD/2TO100", "Radiator Contributions"+filetag, "frwd_radCompare", imageType, false, false, false, 28 );
      heavyIDPerf ( pFile3, tag, "RICH/PIDQC/FRWD/2TO100", "Radiator Contributions"+filetag, "frwd_radCompare", imageType, true,  false, false, 25 );
      heavyIDPerf ( pFile4, tag, "RICH/PIDQC/FRWD/2TO100", "Radiator Contributions"+filetag, "frwd_radCompare", imageType, true,  false, false, 26 );
      heavyIDPerf ( pFile1, tag, "RICH/PIDQC/FRWD/2TO100", "Radiator Contributions"+filetag, "frwd_radCompare", imageType, true,  false, true,  20 );

      // close files
      pFile1->Close();
      pFile2->Close();
      pFile3->Close();
      pFile4->Close();
    }

    /*
    // new tune -----------------------------------------------------------------------
    void brv27r3Performance( const std::string file =
    "rootFiles/allrads-newTune-10000000.root" )
    {
    const std::string tag = "brv27r3";
    gSystem->Exec( ("mkdir -p "+tag).c_str() );
    //allPlots( file, tag, " : Brunel v27r3" );
    compareRads( tag," : Brunel v27r3",
    "rootFiles/aero-newTune-10000000.root",
    "rootFiles/c4f10-newTune-10000000.root",
    "rootFiles/rich2-newTune-10000000.root",
    "rootFiles/allrads-newTune-10000000.root" );
    }
    void brv27r3PerformanceR1( const std::string file =
    "rootFiles/rich1-newTune-10000000.root" )
    {
    const std::string tag = "brv27r3_rich1";
    gSystem->Exec( ("mkdir -p "+tag).c_str() );
    allPlots( file, tag, " : Brunel v27r3 : RICH1" );
    }
    void brv27r3PerformanceR2( const std::string file =
    "rootFiles/rich2-newTune-10000000.root" )
    {
    const std::string tag = "brv27r3_rich2";
    gSystem->Exec( ("mkdir -p "+tag).c_str() );
    allPlots( file, tag, " : Brunel v27r3 : RICH2" );
    }
    void brv27r3PerformanceAero( const std::string file =
    "rootFiles/aero-newTune-10000000.root" )
    {
    const std::string tag = "brv27r3_aero";
    gSystem->Exec( ("mkdir -p "+tag).c_str() );
    allPlots( file, tag, " : Brunel v27r3 : RICH1 Aerogel" );
    }
    void brv27r3PerformanceC4F10( const std::string file =
    "rootFiles/c4f10-newTune-10000000.root" )
    {
    const std::string tag = "brv27r3_c4f10";
    gSystem->Exec( ("mkdir -p "+tag).c_str() );
    allPlots( file, tag, " : Brunel v27r3 : RICH1 C4F10" );
    }

    */

  }
}
