// $Id: $
// Include files

#include <TProfile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TString.h>
#include <TPaveStats.h>
#include <TFile.h>
#include <TKey.h>
#include <TEllipse.h>
#include <TH1.h>
#include <TCanvas.h>
#include <TPad.h>
#include <TStyle.h>
#include <TImage.h>

#include "OnlineHistDB/OnlineHistogram.h"
#include "OMAlib/OMAlib.h"
#include "Gaucho/DimInfoMonObject.h"
#include "Gaucho/MonProfile.h"
#include "Gaucho/MonH1D.h"
#include "Gaucho/MonH2D.h"

// local
#include "DisplayHistogram.h"
#include "presenter.h"
#include "HistogramIdentifier.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DisplayHistogram
//
// 2011-02-09 : Olivier Callot
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
DisplayHistogram::DisplayHistogram( OnlineHistogram* hist ) :
  m_onlineHist( hist ),
  m_rootHistogram( NULL ),
  m_offsetHistogram( NULL),
  m_referenceHist( NULL ),
  m_hostingPad( NULL ),
  m_histogramImage( NULL ),
  m_prettyPalette( NULL )
{

}
//=============================================================================
// Destructor
//=============================================================================
DisplayHistogram::~DisplayHistogram() {
  if ( NULL != m_histogramImage ) delete m_histogramImage;
  m_histogramImage = NULL;
  if ( NULL != m_prettyPalette ) delete m_prettyPalette;
  m_prettyPalette = NULL;
}

//=========================================================================
//  
//=========================================================================
std::string DisplayHistogram::rootName ( ) {
  return m_onlineHist->rootName(); 
}

//=========================================================================
//  
//=========================================================================
void DisplayHistogram::deleteRootHist ( ) {
  if (NULL != m_rootHistogram ) delete m_rootHistogram;
  m_rootHistogram = NULL;
}
//=========================================================================
//  Set or clear the reference histogram
//=========================================================================
void DisplayHistogram::setReferenceHistogram( TH1* ref ) {
  //== Clean the reference if not visible.
  if ( NULL == ref ) {
    if ( m_referenceHist ) {
      delete m_referenceHist;
      m_referenceHist = NULL;
    }
    return;
  }
  if ( 0 != m_referenceHist ) return;

  if ( 0 == m_rootHistogram ) return;
  if ( 1 != m_rootHistogram->GetDimension() ) return;

  m_referenceHist = ref;
  // standard plot style
  m_referenceHist->SetLineStyle(2);
  m_referenceHist->SetLineColor(2); // red
  // normalization
  normalizeReference();
  //== Set options
  std::string refdopt="SAME";
  std::string sopt;
  if ( hasOption("REFDRAWOPTS", &sopt) ) refdopt += sopt;
  m_referenceHist->Draw(refdopt.c_str());
  m_referenceHist->SetStats(0);
  std::cout << "  ... superimpose reference..." << std::endl;

  //== Profile plots?
  TProfile* p    = dynamic_cast<TProfile*>(m_rootHistogram);
  TProfile* pref = dynamic_cast<TProfile*>(m_referenceHist);
  if (p && pref) {
    // spread display option is not propagated to online TProfile (while works in history mode):
    // use option for reference also for the online histogram
    p->SetErrorOption( pref->GetErrorOption() );
  }
}
//=========================================================================
//  Normalize the reference histogram
//=========================================================================
void DisplayHistogram::normalizeReference ( ) {
  if ( m_referenceHist ) {
    double normFactor = m_rootHistogram->GetNormFactor();

    // if GetNormFactor() > 0.1 , histogram is drawn normalized, just use the same normalization
    if ( normFactor < 0.1 ) {
      std::string refOption = pres::s_Area;
      std::string sopt;
      if ( hasOption("REF", &sopt) ) refOption = sopt;
      if (pres::s_Entries == refOption) {
        normFactor = m_rootHistogram->GetEntries();
        m_referenceHist->SetNormFactor(normFactor);
      } else if (pres::s_Area == refOption) {
        normFactor = m_rootHistogram->Integral();
      }
    }
    m_referenceHist->SetNormFactor(normFactor);
  }
}

//=========================================================================
//  Load the histogram using monObject (2010 version)
//=========================================================================
void DisplayHistogram::loadFromMonObject ( std::string& location, bool update ) {
  std::string dimService = m_onlineHist->dimServiceName();
  HistogramIdentifier hid( dimService );
  dimService.erase( 0, dimService.find( "_" ) ); //== erase to first underscore
  dimService.erase( 0, dimService.find( "/" ) ); //== erase to first slash after
  dimService = hid.histogramType() + "/" + location + dimService;
  std::cout << " .. DIM service : " << dimService << std::endl;

  DimInfoMonObject myDimObj( dimService.c_str(), 5, "Presenter");

  if ( myDimObj.createMonObject() ) {
    TString histoRootName;
    TString histoRootTitle;

    if (s_pfixMonH1D == hid.histogramType() ) {
      MonH1D* monTH1D = static_cast<MonH1D*>( myDimObj.monObject() );
      if (NULL != monTH1D) {
        histoRootName = TString(Form("%s__instance__%i", monTH1D->histName().c_str(), m_onlineHist->instance()) );
        histoRootTitle = TString(Form("%s", monTH1D->histTitle().c_str()));
        monTH1D->createObject(histoRootName.Data());
        monTH1D->hist()->SetTitle(histoRootTitle);
        if ( update ) {
          m_rootHistogram->Reset();
          m_rootHistogram->Add( monTH1D->hist() );
          prepareForDisplay();
          if ( NULL != m_hostingPad ) m_hostingPad->Modified();
        } else {
          m_rootHistogram = (TH1*)monTH1D->hist()->Clone();
        }
      }
    } else if ( s_pfixMonH2D == hid.histogramType() ) {
      MonH2D* monTH2D = static_cast<MonH2D*>( myDimObj.monObject() );
      if (NULL != monTH2D) {
        histoRootName = TString(Form("%s__instance__%i", monTH2D->histName().c_str(), m_onlineHist->instance()) );
        histoRootTitle = TString(Form("%s", monTH2D->histTitle().c_str()) );
        monTH2D->createObject(histoRootName.Data());
        monTH2D->hist()->SetTitle(histoRootTitle);
        if ( update ) {
          m_rootHistogram->Reset();
          m_rootHistogram->Add( monTH2D->hist() );
          prepareForDisplay();
          if ( NULL != m_hostingPad ) m_hostingPad->Modified();
        } else {
          m_rootHistogram = (TH1*)monTH2D->hist()->Clone();
        }
      }
    } else if (s_pfixMonProfile == hid.histogramType() ){
      MonProfile* monProfile = static_cast<MonProfile*>(myDimObj.monObject());
      if (NULL != monProfile) {
        histoRootName = TString(Form("%s__instance__%i", monProfile->profileName().c_str(), m_onlineHist->instance()) );
        histoRootTitle = TString(Form("%s", monProfile->profileTitle().c_str()) );
        monProfile->createObject(histoRootName.Data());
        monProfile->profile()->SetTitle(histoRootTitle);
        if ( update ) {
          m_rootHistogram->Reset();
          m_rootHistogram->Add( monProfile->profile() );
          prepareForDisplay();
          if ( NULL != m_hostingPad ) m_hostingPad->Modified();
        } else {
          m_rootHistogram = (TH1*)monProfile->profile()->Clone();
        }
      }
    } else {
      std::cout << "MonObject not included in the Presenter: " << hid.histogramType() << std::endl;
      m_rootHistogram =  0;
    }
    if ( 0 != m_rootHistogram ) {
      histoRootName = TString(Form("%s;%i", m_onlineHist->identifier().c_str(), m_onlineHist->instance()) );
      m_rootHistogram->AddDirectory(kFALSE);
      m_rootHistogram->SetName(histoRootName);
      setDisplayOptions();  // Pass the DB flags to the root histogram
    }
    delete myDimObj.monObject();
  } else {
    std::cout << "Failed to load " << dimService << std::endl;
    createDummyHisto();
  }
}
//=========================================================================
//  Creata a dummy histogram...
//=========================================================================
void DisplayHistogram::createDummyHisto( ) {
  if ( NULL != m_rootHistogram ) delete m_rootHistogram;
  std::string dummyTitle = "ERROR: missing source for " + m_onlineHist->identifier();
  m_rootHistogram = new TH1F( m_onlineHist->identifier().c_str(), dummyTitle.c_str(), 1, 0., 1.);
  m_rootHistogram->SetBit(kNoContextMenu);
  m_rootHistogram->AddDirectory(kFALSE);
  m_rootHistogram->SetStats( false ) ;
}
//=========================================================================
//  Draw the histogram
//=========================================================================
void DisplayHistogram::draw( TCanvas * editorCanvas , double xlow , double ylow ,
                             double xup , double yup , OMAlib* analysisLib,
                             bool fastHitmapDraw ,
                             TPad* overlayOnPad ){
  TPad* pad = overlayOnPad;
  m_isOverlap = (NULL != overlayOnPad);

  editorCanvas->cd();

  if ( !m_isOverlap ) {
    pad = new TPad( m_onlineHist->identifier().c_str(),
                    TString(""),
                    fabs(xlow), fabs(ylow), fabs(xup), fabs(yup));
    float fopt = 0.;
    if ( hasOption( "MARGIN_TOP"    , &fopt ) ) pad->SetTopMargin( fopt ) ;
    if ( hasOption( "MARGIN_BOTTOM" , &fopt ) ) pad->SetBottomMargin( fopt ) ;
    if ( hasOption( "MARGIN_LEFT"   , &fopt ) ) pad->SetLeftMargin( fopt ) ;
    if ( hasOption( "MARGIN_RIGHT"  , &fopt ) ) pad->SetRightMargin( fopt ) ;
    pad->SetBit(kNoContextMenu);
    pad->SetFillColor( 10 );
    pad->Draw(); 
  }
  m_hostingPad = pad;
  
  pad->cd();
  
  if ( 0 != m_rootHistogram ) {
    if (TH2D::Class() == m_rootHistogram->IsA() && fastHitmapDraw ) {
      if (NULL == m_histogramImage) {
        m_histogramImage = TImage::Create();
        m_histogramImage->SetConstRatio(false);
      }
      if ( NULL == m_prettyPalette ) {
        m_prettyPalette = new TImagePalette(1,0);
      }
      
      m_histogramImage->SetImage( (const Double_t *)((TH2D*)m_rootHistogram)->GetArray(),
                                  ((TH2D*)m_rootHistogram)->GetNbinsX() + 2,
                                  ((TH2D*)m_rootHistogram)->GetNbinsY() + 2,
                                  m_prettyPalette ); // gWebImagePalette
      m_histogramImage->Draw();
    } else {
      std::string opt =  m_isOverlap ? "SAME" : "";
      std::cout << "  option for drawing " << opt << std::endl;
      m_rootHistogram->Draw(opt.c_str());
    }
  }
  setDrawingOptions( pad );

  // fit if requested
  fit( analysisLib );

  if ( !m_isOverlap ) pad->SetName( m_onlineHist->identifier().c_str() );

  std::string sopt("");
  //boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);
  if ( /*rootLock && */ 0 != m_onlineHist ) {
    if ( hasOption("DRAWPATTERN", &sopt) && false == m_isOverlap) {

      std::string drawPatternFile = analysisLib->refRoot() + "/" + m_onlineHist->task() + "/" + sopt;

      TFile rootFile( drawPatternFile.c_str() );

      if (rootFile.IsZombie()) {
        std::cout << "Error opening Root file" << std::endl;
      } else {

        std::cout << "Draw pattern " << drawPatternFile << std::endl;

        TIter next1( rootFile.GetListOfKeys() ) ;
        TKey* key;
        TPad* drawPattern = NULL;
        while ( ( key = (TKey*) next1() ) ) {
          if ( key -> ReadObj( ) -> InheritsFrom( TCanvas::Class() ) ) drawPattern = (TCanvas*) key -> ReadObj() ;
        }
        drawPattern -> SetPad( TMath::Abs( xlow ) , TMath::Abs( ylow ) ,
                               TMath::Abs( xup  ) , TMath::Abs( yup  ) ) ;
        drawPattern -> SetName(  m_onlineHist->identifier().c_str() ) ;

        drawPattern->SetFillColor( 10 );
        
        TPad *padsav = (TPad*)gPad;
        TObject *obj;
        
        dynamic_cast< TAttLine* >( drawPattern ) -> Copy( (TAttLine&) *pad ) ;
        dynamic_cast< TAttFill* >( drawPattern ) -> Copy( (TAttFill&) *pad ) ;
        dynamic_cast< TAttPad*  >( drawPattern ) -> Copy( (TAttPad& ) *pad ) ;
        
        TIter next( drawPattern -> GetListOfPrimitives() ) ;
        while ( ( obj=next() ) ) {
          pad->cd();
          if ( TBox::Class()     == obj->IsA() ||
               TLine::Class()    == obj->IsA() ||
               TText::Class()    == obj->IsA() ||
               TEllipse::Class() == obj->IsA() )
            obj->Draw();
        }
        if ( padsav ) padsav->cd();
      }
      rootFile.Close();
    }
  }
}

//=========================================================================
//
//=========================================================================
void DisplayHistogram::setDrawingOptions( TPad* pad ) {
  if ( ( 0 == m_onlineHist ) || 
       ( 0 == m_rootHistogram ) ) return ;
  if ( m_isOverlap ) return;
  
  int iopt = 0;
  float fopt = 0.0;
  std::string sopt("");

  // TPaveStats is obtained after a pad->Draw(), but note that changing OptStat
  // doesn't resize the Pave.. thus it's better to set the global stat options also
  // before drawing
  /*
    if (m_historyTrendPlotMode) { // special settings for trend mode
    m_rootHistogram->SetDrawOption("E1");
    m_rootHistogram->SetStats(0);
    TPaveStats* stats =
    (TPaveStats*) m_rootHistogram->GetListOfFunctions()->FindObject("stats");

    if (stats) stats->Delete();
    pad->SetLogx(0);
    pad->SetLogy(0);
    fopt=.16f;
    pad->SetBottomMargin( (Float_t) fopt);
    bool histByRun=false;
    if( 0 != m_presenterInfo )
    histByRun = m_presenterInfo -> globalHistoryByRun() ;

    if ( histByRun ) {
    m_rootHistogram->GetXaxis()->SetTitle("run");
    fopt=1.2f;
    m_rootHistogram->GetXaxis()->SetTitleOffset( (Float_t) fopt);
    }
    else {
    m_rootHistogram->GetXaxis()->SetTitle("time");
    fopt=1.6f;
    m_rootHistogram->GetXaxis()->SetTitleOffset( (Float_t) fopt);
    }
    std::string ylab="Average";
    if (hasOption("LABEL_X", &sopt))
    ylab = ylab + " (" + sopt + ")";

    m_rootHistogram->SetYTitle (ylab.data());
    }
    else { // normal case
  */
  int statOpt = 0;
  if( false == hasOption("STATS", &statOpt)) statOpt = pres::s_defStatOptions;
  if (0 != statOpt) {
    int statStyle=0;
    if (hasOption("STATTRANSP", &iopt)) if ( iopt > 0 ) statStyle=1001;
    gStyle->SetStatStyle(statStyle); // apparently, this must be called before SetOptStat
    gStyle->SetOptStat( statOpt );
    TPaveStats* stats = (TPaveStats*)m_rootHistogram->GetListOfFunctions()->FindObject("stats");

    if (stats) {
      double x1=stats->GetX1NDC();
      double x2=stats->GetX2NDC();
      double y1=stats->GetY1NDC();
      double y2=stats->GetY2NDC();
      if (hasOption("STAT_X_OFFS", &fopt)) x1 = fopt;
      if (hasOption("STAT_X_SIZE", &fopt)) x2 = x1 + fopt;
      if (hasOption("STAT_Y_OFFS", &fopt)) y1 = fopt;
      if (hasOption("STAT_Y_SIZE", &fopt)) y2 = y1 + fopt;

      stats->SetX1NDC(x1);
      stats->SetX2NDC(x2);
      stats->SetY1NDC(y1);
      stats->SetY2NDC(y2);
    }
  }
  // title pave
  TPaveText* titpave = (TPaveText*) pad->GetPrimitive("title");
  if (titpave) {
    int optTit=1;
    if(hasOption("NOTITLE", &iopt)) {
      if ( iopt > 0 ) optTit = 0;    //user requires no title window
    }
    if( 0 == optTit) {
      // put window title out of sight (better than using TStyle::SetOptTitle which is too global..)
      titpave->SetX1NDC(-2);
      titpave->SetX2NDC(-1);
    } else {
      double x1=titpave->GetX1NDC();
      double x2=titpave->GetX2NDC();
      double y1=titpave->GetY1NDC();
      double y2=titpave->GetY2NDC();
      if (hasOption("HTIT_X_OFFS", &fopt)) x1 = fopt;
      if (hasOption("HTIT_X_SIZE", &fopt)) x2 = x1 + fopt;
      if (hasOption("HTIT_Y_OFFS", &fopt)) y1 = fopt;
      if (hasOption("HTIT_Y_SIZE", &fopt)) y2 = y1 + fopt;

      titpave->SetX1NDC(x1);
      titpave->SetX2NDC(x2);
      titpave->SetY1NDC(y1);
      titpave->SetY2NDC(y2);
    }
  }
  
  if (hasOption("DRAWOPTS", &sopt) ) {
    if(m_isOverlap && sopt.find("SAME") == std::string::npos ) sopt += "SAME";
    //if (m_fitfunction) { // remove HIST option that disables showing the fit
    //  if (sopt.find("HIST") < sopt.size()) {
    //    sopt.erase(sopt.find("HIST"), 4);
    //  }
    // }
    m_rootHistogram->SetDrawOption(sopt.c_str());
  }
  if (hasOption("LOGX", &iopt)) pad->SetLogx(1);
  if (hasOption("LOGY", &iopt)) {
    if (m_rootHistogram->GetEntries()>0) pad->SetLogy(1) ;
    // Set log scale also if the minimum and maximum scale is not 0
    else if ( ( m_rootHistogram -> GetMinimum() > 0. ) &&
              ( m_rootHistogram -> GetMaximum() > 0. ) ) pad -> SetLogy( 1 ) ;
    else pad->SetLogy( 0 );
  }

  int gridx = gStyle->GetPadGridX();
  int gridy = gStyle->GetPadGridY();
  if (hasOption("GRIDX", &iopt)) gridx=iopt;
  if (hasOption("GRIDY", &iopt)) gridy=iopt;
  pad->SetGrid(gridx, gridy);
  if ( m_rootHistogram->GetDimension() > 1) {
    if (hasOption("LOGZ", &iopt)) pad->SetLogz(1);
    if (hasOption("THETA", &fopt)) pad->SetTheta(fopt);
    if (hasOption("PHI", &fopt)) pad->SetPhi(fopt);
  }

  if(hasOption("TICK_X", &iopt)) pad->SetTickx(iopt);
  if(hasOption("TICK_Y", &iopt)) pad->SetTicky(iopt);
  if(hasOption("PADCOLOR", &iopt)) pad->SetFillColor(iopt);
  //  }
}

//=========================================================================
//  Pass the display options from the OnlineHist to the root histogram
//=========================================================================
void DisplayHistogram::setDisplayOptions ( ) {
  if ( ( 0 == m_onlineHist ) || ( 0 == m_rootHistogram ) ) return ;
  m_rootHistogram->SetStats( false );

  //boost::recursive_mutex::scoped_lock oraLock(*m_oraMutex);
  //boost::recursive_mutex::scoped_lock rootLock(*m_rootMutex);

  //if ( oraLock && rootLock ) {
    int iopt = 0;
    float fopt = 0.0;
    std::string sopt;
    if (hasOption("LABEL_X", &sopt))  m_rootHistogram->SetXTitle (sopt.data());
    if (hasOption("LABEL_Y", &sopt)) m_rootHistogram->SetYTitle (sopt.data());
    if (hasOption("LABEL_Z", &sopt)) m_rootHistogram->SetZTitle (sopt.data());
    double bxmin=m_rootHistogram->GetXaxis()->GetXmin();
    double bxmax=m_rootHistogram->GetXaxis()->GetXmax();

    if (hasOption("XMIN", &fopt)) { bxmin=fopt; }
    if (hasOption("XMAX", &fopt)) { bxmax=fopt; }
    m_rootHistogram->GetXaxis()->SetRangeUser(bxmin,bxmax);

    if (m_onlineHist->dimension() <2) { // 1d histograms
      if (hasOption("YMIN", &fopt)) m_rootHistogram->SetMinimum(fopt);
      if (hasOption("YMAX", &fopt)) m_rootHistogram->SetMaximum(fopt);
    } else {  // 2d histograms
      double bymin = m_rootHistogram->GetYaxis()->GetXmin();
      double bymax = m_rootHistogram->GetYaxis()->GetXmax();
      if (hasOption("YMIN", &fopt)) { bymin=fopt; }
      if (hasOption("YMAX", &fopt)) { bymax=fopt; }
      m_rootHistogram->GetYaxis()->SetRangeUser(bymin, bymax);
      if (hasOption("ZMIN", &fopt)) m_rootHistogram->SetMinimum(fopt);
      if (hasOption("ZMAX", &fopt)) m_rootHistogram->SetMaximum(fopt);
    }
    if (hasOption("STATS", &iopt)) m_rootHistogram->SetStats(0 != iopt);

    if (hasOption("SHOWTITLE", &sopt)) m_rootHistogram->SetTitle(sopt.c_str());

    if (hasOption("FILLSTYLE", &iopt)) m_rootHistogram->SetFillStyle(iopt);
    if (hasOption("FILLCOLOR", &iopt)) m_rootHistogram->SetFillColor(iopt);
    if (hasOption("LINESTYLE", &iopt)) m_rootHistogram->SetLineStyle(iopt);
    if (hasOption("LINECOLOR", &iopt)) m_rootHistogram->SetLineColor(iopt);
    if (hasOption("LINEWIDTH", &iopt)) m_rootHistogram->SetLineWidth(iopt);
    if (hasOption("MARKERSIZE", &fopt)) m_rootHistogram->SetMarkerSize((Size_t)fopt);
    if (hasOption("MARKERSTYLE", &iopt)) m_rootHistogram->SetMarkerStyle(iopt);
    if (hasOption("MARKERCOLOR", &iopt)) m_rootHistogram->SetMarkerColor(iopt);

    if (hasOption("TIT_X_SIZE", &fopt)) m_rootHistogram->GetXaxis()->SetTitleSize(fopt);
    if (hasOption("TIT_X_OFFS", &fopt)) m_rootHistogram->GetXaxis()->SetTitleOffset(fopt);
    if (hasOption("LAB_X_SIZE", &fopt)) m_rootHistogram->GetXaxis()->SetLabelSize(fopt);
    if (hasOption("LAB_X_OFFS", &fopt)) m_rootHistogram->GetXaxis()->SetLabelOffset(fopt);
    if (hasOption("NDIVX", &iopt)) m_rootHistogram->SetNdivisions(iopt,"X");

    if (hasOption("TIT_Y_SIZE", &fopt)) m_rootHistogram->GetYaxis()->SetTitleSize(fopt);
    if (hasOption("TIT_Y_OFFS", &fopt)) m_rootHistogram->GetYaxis()->SetTitleOffset(fopt);
    if (hasOption("LAB_Y_SIZE", &fopt)) m_rootHistogram->GetYaxis()->SetLabelSize(fopt);
    if (hasOption("LAB_Y_OFFS", &fopt)) m_rootHistogram->GetYaxis()->SetLabelOffset(fopt);
    if (hasOption("NDIVY", &iopt)) m_rootHistogram->SetNdivisions(iopt,"Y");

    if (hasOption("TIT_Z_SIZE", &fopt)) m_rootHistogram->GetZaxis()->SetTitleSize(fopt);
    if (hasOption("TIT_Z_OFFS", &fopt)) m_rootHistogram->GetZaxis()->SetTitleOffset(fopt);
    if (hasOption("LAB_Z_SIZE", &fopt)) m_rootHistogram->GetZaxis()->SetLabelSize(fopt);
    if (hasOption("LAB_Z_OFFS", &fopt)) m_rootHistogram->GetZaxis()->SetLabelOffset(fopt);

    if (hasOption("NORM", &fopt)) if (fopt>0.) m_rootHistogram->SetNormFactor(fopt);

    // custom bin labels
    if ( 0 != m_rootHistogram->GetXaxis()) {
      if (m_onlineHist->nXbinlabels() > 0) {
        unsigned int  nbins = m_rootHistogram->GetNbinsX() ;
        if ( m_onlineHist->nXbinlabels() < nbins ) nbins = m_onlineHist->nXbinlabels();
        for (unsigned int il = 0; il < nbins; il++) {
          sopt = m_onlineHist->binlabel(il,0);
          m_rootHistogram->GetXaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
    if (0 != m_rootHistogram->GetYaxis()) {
      if (m_onlineHist->nYbinlabels() > 0) {
        unsigned int  nbins = m_rootHistogram->GetNbinsY() ;
        if ( m_onlineHist->nYbinlabels() < nbins ) nbins = m_onlineHist->nYbinlabels();
        for (unsigned int il = 0; il < nbins; il++) {
          sopt = m_onlineHist->binlabel(il,1);
          m_rootHistogram->GetYaxis()->SetBinLabel(il+1, sopt.c_str());
        }
      }
    }
    //}
}

//=========================================================================
//  Add a fit to the histogram
//=========================================================================
void DisplayHistogram::fit ( OMAlib* analysisLib ) {
  if ( 0 == m_onlineHist ) return ;
  if ( ( m_onlineHist->hasFitFunction() ) && 
       ( 0 != m_rootHistogram ) ) {
    std::string Name;
    std::vector<float> initValues;
    gStyle -> SetOptFit( 1111111 ) ;
    m_onlineHist->getFitFunction( Name, &initValues);

    std::cout << "fitting histogram " << m_onlineHist->identifier() 
              << " with function "<< Name << std::endl;

    OMAFitFunction* requestedFit =  analysisLib->getFitFunction(Name);
    if ( 0 == requestedFit ) {
      std::cerr << "Unknown fit function: " << Name
                << " to fit histogram "
                << m_onlineHist->identifier()
                << std::endl ;
      return ;
    }
    requestedFit->fit(m_rootHistogram, &initValues);
  }
}

//=========================================================================
//  
//=========================================================================
bool DisplayHistogram::hasOption( const char* str, int* iopt ) {
  if ( !m_onlineHist->getDisplayOption( str, iopt ) ) return false;
  //std::cout << m_onlineHist->identifier() << " has option " << str 
  //          << " set to value " << *iopt << std::endl;
  return true;
}

bool DisplayHistogram::hasOption( const char* str, float* fopt ) {
  if ( !m_onlineHist->getDisplayOption( str, fopt ) ) return false;
  //std::cout << m_onlineHist->identifier() << " has option " << str
  //          << " set to value " << *fopt << std::endl;
  return true;
}
  
bool DisplayHistogram::hasOption( const char* str, std::string* sopt ) {
  if ( !m_onlineHist->getDisplayOption( str, sopt ) ) return false;
  //std::cout << m_onlineHist->identifier() << " has option " << str 
  //          << " set to value " << *sopt << std::endl;
  return true;
}

//=========================================================================
//  Save the current histogram as offset
//=========================================================================
void DisplayHistogram::setOffsetHistogram ( ) {
  if ( NULL == m_rootHistogram ) return;
  
  std::cout << "Set offset for " << m_rootHistogram->GetTitle() << std::endl;
  if ( m_offsetHistogram ) { delete m_offsetHistogram; m_offsetHistogram = 0;}
  if ( 0 != dynamic_cast<TProfile*>(m_rootHistogram) ) {
    m_offsetHistogram = new TProfile(*dynamic_cast<TProfile*>(m_rootHistogram));
    m_offsetHistogram->SetBit(kNoContextMenu);
  } else if ( 0 != dynamic_cast<TH1D*>(m_rootHistogram) ) {
    m_offsetHistogram = new TH1D(*dynamic_cast<TH1D*>(m_rootHistogram));
    m_offsetHistogram->SetBit(kNoContextMenu);
  } else if ( 0 != dynamic_cast<TH2D*>(m_rootHistogram) ) {
    m_offsetHistogram = new TH2D(*dynamic_cast<TH2D*>(m_rootHistogram));
    m_offsetHistogram->SetBit(kNoContextMenu);
  }
  if ( m_offsetHistogram ) { m_offsetHistogram->AddDirectory(kFALSE); }
}


//=========================================================================
//  reset (delete) the offset histogram
//=========================================================================
void DisplayHistogram::resetOffsetHistogram ( ) {
  if ( m_offsetHistogram ) { delete m_offsetHistogram; m_offsetHistogram = 0;}
}
//=========================================================================
//  Prepare the histogram for display: Subtract offset, build image.
//=========================================================================
void DisplayHistogram::prepareForDisplay ( ) {
  if ( NULL == m_rootHistogram ) return;
  if ( NULL != m_offsetHistogram ) {
    if ( 0 != dynamic_cast<TH2D*>(m_rootHistogram)  ) {
      for (int i=1; i<= m_rootHistogram->GetNbinsX() ; ++i) {
        for (int j=1; j <= m_rootHistogram->GetNbinsY() ; ++j) {
          m_rootHistogram->SetBinContent(i, j, m_rootHistogram->GetBinContent(i, j) - m_offsetHistogram->GetBinContent(i, j));
          m_rootHistogram->SetBinError(i, j, sqrt( pow(m_rootHistogram->GetBinError(i, j), 2) -
                                                   pow(m_offsetHistogram->GetBinError(i, j), 2)) );
        }
      } 
      std::cout << "Subtract offset 2D size " << m_rootHistogram->GetNbinsX() << " x " 
                << m_rootHistogram->GetNbinsY() << std::endl;
    } else {
      for (int i = 1; i <= m_rootHistogram->GetNbinsX(); ++i) {
        m_rootHistogram->SetBinContent(i, m_rootHistogram->GetBinContent(i) - m_offsetHistogram->GetBinContent(i));
        m_rootHistogram->SetBinError(i, sqrt( pow(m_rootHistogram->GetBinError(i), 2) - 
                                              pow(m_offsetHistogram->GetBinError(i), 2) ) );
      }
      std::cout << "Subtract offset 1D size " << m_rootHistogram->GetNbinsX() << std::endl;
    }
    
  }
  if ( 0 != dynamic_cast<TH2D*>(m_rootHistogram) ) {
    if ((TH2D::Class() == m_rootHistogram->IsA()) && m_histogramImage) {
      m_histogramImage->SetImage( (const Double_t *)((TH2D*)m_rootHistogram)->GetArray(),
                                  ((TH2D*)m_rootHistogram)->GetNbinsX() + 2,
                                  ((TH2D*)m_rootHistogram)->GetNbinsY() + 2, m_prettyPalette);
    }
  }
}
//=============================================================================
