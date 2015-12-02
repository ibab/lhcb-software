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
#include <TGraph.h>

// Trending
#include "Trending/ITrendingTool.h"

// Boost
#include <boost/filesystem.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/posix_time_types.hpp"

#include "OnlineHistDB/OnlineHistogram.h"
#include "OMAlib/OMAlib.h"
// local
#include "Presenter/DisplayHistogram.h"
#include "Presenter/presenter.h"

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
  m_previous(-1),
  m_identifier( "?not set?" ),
  m_shortName( "" ),
  m_title( "" ),
  m_isOverlap( false ),
  m_isTrendPlot( false ),
  m_historyTrendPlot( false ),
  m_rootHistogram( NULL ),
  m_offsetHistogram( NULL),
  m_referenceHist( NULL ),
  m_hostingPad( NULL ),
  m_titPave( NULL),
  m_statPave( NULL ),
  m_histogramImage( NULL ),
  m_prettyPalette( NULL ),
  m_timeGraph( NULL ),
  m_minGraph( NULL ),
  m_maxGraph( NULL ),
  m_timeArray( NULL ),
  m_valueArray( NULL ),
  m_nOverlap( 0 ),
  m_hasTitle( false ),
  m_optionsAreLoaded( false ),
  m_showReference( false ),
  m_isDummy( false )
{
  if ( NULL != hist ) {
    m_identifier = m_onlineHist->identifier();
    if ( hist->type() == OnlineHistDBEnv::TRE ) {
      m_isTrendPlot = true;
      m_shortName = hist->htitle();  // this is in fact the variable name!
    }
    hist->getDisplayOption( "SHOWTITLE", &m_title );
    if ( "" == m_title ) m_title = m_shortName;
  }
}
//=============================================================================
// Destructor
//=============================================================================
DisplayHistogram::~DisplayHistogram() {
  if ( NULL != m_rootHistogram ) delete m_rootHistogram;
  m_rootHistogram = 0;
  if ( NULL != m_offsetHistogram ) delete m_offsetHistogram;
  m_offsetHistogram = 0;
  if ( NULL != m_referenceHist ) delete m_referenceHist;
  m_referenceHist = 0;
  if ( NULL != m_histogramImage ) delete m_histogramImage;
  m_histogramImage = NULL;
  if ( NULL != m_prettyPalette ) delete m_prettyPalette;
  m_prettyPalette = NULL;
  if ( NULL != m_timeGraph ) delete m_timeGraph;
  m_timeGraph = NULL;
  if ( NULL != m_minGraph ) delete m_minGraph;
  m_minGraph = NULL;
  if ( NULL != m_maxGraph ) delete m_maxGraph;
  m_maxGraph = NULL;
  if ( NULL != m_timeArray ) delete m_timeArray;
  m_timeArray = NULL;
  if ( NULL != m_valueArray ) delete m_valueArray;
  m_valueArray = NULL;

}

//=========================================================================
//
//=========================================================================
std::string DisplayHistogram::rootName ( ) {
  if ( NULL != m_onlineHist ) return m_onlineHist->rootName();
  return m_identifier;
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
  m_referenceHist = ref;  // needed for 2D analysis plots even if not displayed
  if ( NULL == m_hostingPad ) return;
  if ( NULL == m_rootHistogram ) return;
  if ( 1 != m_rootHistogram->GetDimension() ) return;

  // standard plot style
  m_hostingPad->cd();
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
    double normFactor = 0.;

    std::string refOption = pres::s_Area;
    std::string sopt;
    if ( hasOption("REF", &sopt) ) refOption = sopt;
    if (pres::s_Entries == refOption) {
      normFactor = m_rootHistogram->GetEntries();
    } else if (pres::s_Area == refOption) {
      normFactor = m_rootHistogram->Integral();
    }

    m_referenceHist->SetNormFactor(normFactor);
    std::cout << "Normalization of reference option " << refOption  << " value = " << normFactor << std::endl;
  }
}
//=========================================================================
//  Creata a dummy histogram...
//=========================================================================
void DisplayHistogram::createDummyHisto( std::string title ) {
  //deleteRootHist();

  std::string histoName = m_identifier;
  unsigned int pos = histoName.find( "/" );   // remove the task name prefix
  if ( pos < histoName.size() ) {
    histoName.erase( 0, pos+1 );
  }
  if ( !m_isTrendPlot ) m_shortName = histoName;  // as we create a temporary dummy histo for trending!
  std::string dummyTitle = "** " + title + " ** " + m_shortName;

  m_rootHistogram = new TH1F( m_identifier.c_str(), dummyTitle.c_str(), 1, 0., 1.);
  m_rootHistogram->SetBit(kNoContextMenu);
  m_rootHistogram->AddDirectory(kFALSE);
  m_rootHistogram->SetStats( false ) ;
  m_isDummy = true;
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
    std::cout << "xlow " << xlow << " ylow " << ylow << " xup " << xup << " yup " << yup << std::endl;
    pad = new TPad( m_identifier.c_str(),
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
    if ( 0 < m_nOverlap ) {
      float topMargin = 0.1f + 0.05f * ( m_nOverlap/2 );
      pad->SetTopMargin( topMargin );
    }
  }
  m_hostingPad = pad;

  pad->cd();

  //== Title suppressed if overlapped plots AND no positioning of the title box
  m_hasTitle = m_nOverlap == 0;
  float fopt = 0.;
  if ( hasOption("HTIT_X_OFFS", &fopt) ) m_hasTitle = true;
  if ( hasOption("HTIT_X_SIZE", &fopt) ) m_hasTitle = true;
  if ( hasOption("HTIT_Y_OFFS", &fopt) ) m_hasTitle = true;
  if ( hasOption("HTIT_Y_SIZE", &fopt) ) m_hasTitle = true;

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
      std::cout << "Draw by image" << std::endl;
      m_histogramImage->Draw();
    } else {
      if ( !m_hasTitle ) m_rootHistogram->SetBit( TH1::kNoTitle, true );
      std::string opt = m_isOverlap ? "SAME" : "";
      std::string sopt("");
      if (hasOption("DRAWOPTS", &sopt) ) opt += sopt;
      m_rootHistogram->Draw(opt.c_str());
      if ( NULL != m_referenceHist && 1 == m_rootHistogram->GetDimension() ){
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
    }
  } else if ( 0 != m_timeGraph ) {
    if ( !m_hasTitle ) m_timeGraph->SetTitle( "" );
    std::string opt =  m_isOverlap ? "SAME" : "AL";
    m_timeGraph->Draw(opt.c_str());
    if ( NULL != m_minGraph ) m_minGraph->Draw( "" );
    if ( NULL != m_maxGraph ) m_maxGraph->Draw( "" );
  } else {
    std::cout << "??? No root nor TGraph to draw!" << std::endl;
  }

  setDrawingOptions( pad );

  fit( analysisLib );    // fit if requested

  if ( !m_isOverlap ) pad->SetName( m_identifier.c_str() );

  std::string sopt("");
  if ( 0 != m_onlineHist && ! m_isDummy && NULL != analysisLib ) {
    if ( hasOption("DRAWPATTERN", &sopt) && false == m_isOverlap) {

      std::cout << "root from analysis " << analysisLib->refRoot() << std::endl;
      std::cout << "task " << m_onlineHist->task() << std::endl;
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
        drawPattern -> SetName(  m_identifier.c_str() ) ;

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
  std::cout << "Draw: rootHist " << m_rootHistogram << std::endl;
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

  if ( m_isTrendPlot ) return;
  if ( m_historyTrendPlot ) {
    m_rootHistogram->SetDrawOption("E1");
    m_rootHistogram->SetStats(0);
    TPaveStats* stats = (TPaveStats*) m_rootHistogram->GetListOfFunctions()->FindObject("stats");

    if (stats) stats->Delete();
    pad->SetLogx(0);
    pad->SetLogy(0);
    fopt=.16f;
    pad->SetBottomMargin( (Float_t) fopt);
    m_rootHistogram->GetXaxis()->SetTitle("run");
    fopt=1.2f;
    m_rootHistogram->GetXaxis()->SetTitleOffset( (Float_t) fopt);
    std::string ylab="Average";
    if ( m_onlineHist->getDisplayOption("LABEL_X", &sopt) ) ylab = ylab + " (" + sopt + ")";
    m_rootHistogram->SetYTitle ( ylab.data() );
    return;
  }

  int statOpt =  pres::s_defStatOptions;
  hasOption( "STATS", &statOpt );
  if (0 != statOpt) {
    int statStyle=0;
    if ( hasOption("STATTRANSP", &iopt) ) if ( iopt > 0 ) statStyle=1001;
    gStyle->SetStatStyle(statStyle); // apparently, this must be called before SetOptStat
    gStyle->SetOptStat( statOpt );
    m_statPave = (TPaveStats*)m_rootHistogram->GetListOfFunctions()->FindObject("stats");
    if (m_statPave) {
      double x1=m_statPave->GetX1NDC();
      double x2=m_statPave->GetX2NDC();
      double y1=m_statPave->GetY1NDC();
      double y2=m_statPave->GetY2NDC();
      if (hasOption("STAT_X_OFFS", &fopt)) x1 = fopt;
      if (hasOption("STAT_X_SIZE", &fopt)) x2 = x1 + fopt;
      if (hasOption("STAT_Y_OFFS", &fopt)) y1 = fopt;
      if (hasOption("STAT_Y_SIZE", &fopt)) y2 = y1 + fopt;

      std::cout << "Stat pave: x1 " << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2  << std::endl;

      m_statPave->SetX1NDC(x1);
      m_statPave->SetX2NDC(x2);
      m_statPave->SetY1NDC(y1);
      m_statPave->SetY2NDC(y2);
    }
  }
  // title pave
  m_titPave = (TPaveText*) pad->GetPrimitive("title");
  if (m_titPave) {
    int optTit=1;
    if(hasOption("NOTITLE", &iopt) ) {
      if ( iopt > 0 ) optTit = 0;    //user requires no title window
    }

    if ( 0 == optTit ) {
      // put window title out of sight (better than using TStyle::SetOptTitle which is too global..)
      m_titPave->SetX1NDC(-2);
      m_titPave->SetX2NDC(-1);
      std::cout << "Set title pave aside" << std::endl;
      pad->SetName( "" );
    } else {
      double x1=m_titPave->GetX1NDC();
      double x2=m_titPave->GetX2NDC();
      double y1=m_titPave->GetY1NDC();
      double y2=m_titPave->GetY2NDC();
      if (hasOption("HTIT_X_OFFS", &fopt)) x1 = fopt;
      if (hasOption("HTIT_X_SIZE", &fopt)) x2 = x1 + fopt;
      if (hasOption("HTIT_Y_OFFS", &fopt)) y1 = fopt;
      if (hasOption("HTIT_Y_SIZE", &fopt)) y2 = y1 + fopt;

      std::cout << "Tit pave: x1 " << x1 << " y1 " << y1 << " x2 " << x2 << " y2 " << y2  << std::endl;
      m_titPave->SetX1NDC(x1);
      m_titPave->SetX2NDC(x2);
      m_titPave->SetY1NDC(y1);
      m_titPave->SetY2NDC(y2);

      m_titPave->SetBorderSize( 4 );
      m_titPave->SetFillStyle( 1001 );
    }
  }

  if (hasOption("DRAWOPTS", &sopt) ) m_rootHistogram->SetDrawOption(sopt.c_str());
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
  std::cout << "Grid: gridx " << gridx << " gridy " << gridy  << std::endl;
  pad->SetGrid(gridx, gridy);
  if ( m_rootHistogram->GetDimension() > 1) {
    if (hasOption("LOGZ", &iopt)) pad->SetLogz(1);
    if (hasOption("THETA", &fopt)) pad->SetTheta(fopt);
    if (hasOption("PHI", &fopt)) pad->SetPhi(fopt);
  }

  if(hasOption("TICK_X", &iopt)) pad->SetTickx(iopt);
  if(hasOption("TICK_Y", &iopt)) pad->SetTicky(iopt);
  if(hasOption("PADCOLOR", &iopt)) { pad->SetFillColor(iopt); std::cout << "set PADCOLOR=" << iopt << std::endl; }

}
//=========================================================================
//  Pass the display options from the OnlineHist to the root histogram
//=========================================================================
void DisplayHistogram::setDisplayOptions ( ) {
  if ( ( 0 == m_onlineHist ) || ( 0 == m_rootHistogram ) ) return ;
  m_rootHistogram->SetStats( false );

  int iopt = 0;
  float fopt = 0.0;
  std::string sopt;
  if (hasOption("LABEL_X", &sopt)) m_rootHistogram->SetXTitle (sopt.data());
  if (hasOption("LABEL_Y", &sopt)) m_rootHistogram->SetYTitle (sopt.data());
  if (hasOption("LABEL_Z", &sopt)) m_rootHistogram->SetZTitle (sopt.data());
  double bxmin=m_rootHistogram->GetXaxis()->GetXmin();
  double bxmax=m_rootHistogram->GetXaxis()->GetXmax();

  if (hasOption("XMIN", &fopt)) { bxmin=fopt; }
  if (hasOption("XMAX", &fopt)) { bxmax=fopt; }
  m_rootHistogram->GetXaxis()->SetRangeUser(bxmin,bxmax);

  if (m_rootHistogram->GetDimension() < 2) { // 1d histograms
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
}

//=========================================================================
//  Add a fit to the histogram
//=========================================================================
void DisplayHistogram::fit ( OMAlib* analysisLib ) {
  if ( 0 == m_onlineHist ) return ;
  if ( ( m_onlineHist->hasFitFunction() ) &&
       ( 0 != m_rootHistogram ) &&
       0 < m_rootHistogram->GetEntries() ) {
    std::string Name;
    std::vector<float> initValues;
    std::vector<float> fitRanges;
    gStyle -> SetOptFit( 1111111 ) ;
    m_onlineHist->getFitFunction( Name, &initValues, &fitRanges);

    std::cout << "fitting histogram " << m_identifier
              << " with function "<< Name << std::endl;

    OMAFitFunction* requestedFit =  analysisLib->getFitFunction(Name);
    if ( 0 == requestedFit ) {
      std::cerr << "Unknown fit function: " << Name
                << " to fit histogram "
                << m_identifier
                << std::endl ;
      return ;
    }
    requestedFit->fit(m_rootHistogram, &initValues, &fitRanges);
  }
}

//=========================================================================
//  Accessor to retrieve the options
//=========================================================================
bool DisplayHistogram::hasOption( const char* str, int* iopt ) {
  if ( !m_optionsAreLoaded ) loadOptions();
  std::string name(str);
  std::map<std::string,int>::iterator it = m_intOptions.find( name );
  if ( it == m_intOptions.end() ) return false;
  *iopt = it->second;
  //std::cout << "IOption " << name << " value " << *iopt << std::endl;
  return true;
}

bool DisplayHistogram::hasOption( const char* str, float* fopt ) {
  if ( !m_optionsAreLoaded ) loadOptions();
  std::string name(str);
  std::map<std::string,float>::iterator it = m_floatOptions.find( name );
  if ( it == m_floatOptions.end() ) return false;
  *fopt = it->second;
  //std::cout << "FOption " << name << " value " << *fopt << std::endl;
  return true;
}

bool DisplayHistogram::hasOption( const char* str, std::string* sopt ) {
  if ( !m_optionsAreLoaded ) loadOptions();
  std::string name(str);
  std::map<std::string,std::string>::iterator it = m_stringOptions.find( name );
  if ( it == m_stringOptions.end() ) return false;
  *sopt = it->second;
  //std::cout << "SOption " << name << " value " << *sopt << std::endl;
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
//=========================================================================
// Set the time graph from argument
//=========================================================================
void DisplayHistogram::createGraph( TrendData& aTrend, bool update ) {
  int size = 2*aTrend.values.size() - 1;
  if ( NULL != m_timeArray ) delete m_timeArray;
  if ( NULL != m_valueArray ) delete m_valueArray;
  m_timeArray = new double[ size ] ;
  m_valueArray = new double[ size ] ;
  double* p = m_timeArray;
  double* q = m_valueArray;
  double lastV = -9999.;
  for ( std::vector<std::pair<int,double> >::iterator itT = aTrend.values.begin(); aTrend.values.end() != itT; ++itT ) {
    *p++ = (*itT).first - 3600.;
    if ( lastV != -9999. ) {
      *p++ = (*itT).first - 3600.;
      *q++ = lastV;
    }
    *q++ = (*itT).second;
    lastV = (*itT).second;
  }
  std::cout << "Created graph '" << aTrend.tag << "' size " << size << std::endl;

  float fopt=0.;
  int lineColor = 1;
  hasOption("LINECOLOR", &lineColor );

  if ( !update ) {
    if ( NULL != m_timeGraph ) delete m_timeGraph;
    if ( NULL != m_minGraph  ) delete m_minGraph;
    if ( NULL != m_maxGraph  ) delete m_maxGraph;
    m_minGraph = NULL;
    m_maxGraph = NULL;
    m_timeGraph = new TGraph( size , m_timeArray , m_valueArray ) ;
    m_timeGraph->SetEditable ( kFALSE );
    if ( m_showReference && hasOption("YMIN", &fopt) ) {
      m_minGraph = new TGraph( 2, m_timeArray, m_valueArray );
      m_minGraph->SetDrawOption( "SAME" );
      m_minGraph->SetLineStyle( 2 );
      m_minGraph->SetLineColor( lineColor );
    }
    if ( m_showReference && hasOption("YMAX", &fopt) ) {
      m_maxGraph = new TGraph( 2, m_timeArray, m_valueArray );
      m_maxGraph->SetDrawOption( "SAME" );
      m_maxGraph->SetLineStyle( 2 );
      m_maxGraph->SetLineColor( lineColor );
    }
  } else {
    int nbPrev = m_timeGraph->GetN();
    if ( nbPrev > size ) {
      for ( int kk = nbPrev; size <= kk ; --kk ) {
        m_timeGraph->RemovePoint( kk );
      }
    }
    for ( int ll = 0 ; size > ll ; ++ll ) {
      m_timeGraph->SetPoint( ll, m_timeArray[ll], m_valueArray[ll] );
    }
    if ( m_showReference ) {
      if ( NULL == m_minGraph && hasOption("YMIN", &fopt) ) {
        m_minGraph = new TGraph( 2, m_timeArray, m_valueArray );
        m_minGraph->SetDrawOption( "SAME" );
        m_minGraph->SetLineStyle( 2 );
        m_minGraph->SetLineColor( lineColor );
        m_hostingPad->cd( );
        m_minGraph->Draw( "SAME" );
        m_minGraph->Draw( "" );
      }
      if ( NULL == m_maxGraph && hasOption("YMAX", &fopt) ) {
        m_maxGraph = new TGraph( 2, m_timeArray, m_valueArray );
        m_maxGraph->SetDrawOption( "SAME" );
        m_maxGraph->SetLineStyle( 2 );
        m_maxGraph->SetLineColor( lineColor );
        m_hostingPad->cd( );
        m_maxGraph->Draw( "SAME" );
        m_maxGraph->Draw( "" );
      }
    } else {
      if ( NULL != m_minGraph ) {
        delete m_minGraph;
        m_minGraph = NULL;
      }
      if ( NULL != m_maxGraph ) {
        delete m_maxGraph;
        m_maxGraph = NULL;
      }
    }
  }

  if ( 0 < m_nOverlap ) {
    m_timeGraph->SetTitle( "" );
  } else {
    m_timeGraph->SetTitle( m_title.c_str() );
  }
  m_timeGraph->GetXaxis()->SetTimeDisplay( 1 ) ;
  m_timeGraph->GetXaxis()->SetTimeFormat( "#splitline{%d/%m/%y}{%H:%M:%S}" ) ;
  m_timeGraph->GetXaxis()->SetTimeOffset( 0 ) ;

  if ( m_isOverlap ) {
    std::string sopt = "SAME";
    m_timeGraph->SetDrawOption(sopt.c_str());
  }

  //== Set good defaults if not set by the individual histogram
  createDummyHisto();
  m_rootHistogram->GetXaxis()->SetLabelOffset( 0.03f );
  m_rootHistogram->GetXaxis()->SetLabelSize( 0.03f );

  double yMin = aTrend.min;
  double yMax = aTrend.max;

  if ( NULL != m_minGraph ) {
    hasOption("YMIN", &fopt);
    double xmin[2], ymin[2];
    xmin[0] = m_timeArray[0];
    xmin[1] = m_timeArray[size-1];
    ymin[0] = fopt;
    ymin[1] = fopt;
    m_minGraph->RemovePoint( 0 );
    m_minGraph->RemovePoint( 1 );
    m_minGraph->SetPoint( 0, xmin[0], ymin[0] );
    m_minGraph->SetPoint( 1, xmin[1], ymin[1] );
    if ( yMin > fopt ) yMin = fopt;
  }
  if ( NULL != m_maxGraph ) {
    hasOption("YMAX", &fopt);
    double xmax[2], ymax[2];
    xmax[0] = m_timeArray[0];
    xmax[1] = m_timeArray[size-1];
    ymax[0] = fopt;
    ymax[1] = fopt;
    m_maxGraph->RemovePoint( 0 );
    m_maxGraph->RemovePoint( 1 );
    m_maxGraph->SetPoint( 0, xmax[0], ymax[0] );
    m_maxGraph->SetPoint( 1, xmax[1], ymax[1] );
    if ( yMax < fopt ) yMax = fopt;
  }

  //== Set properties...
  setDisplayOptions();

  TAxis * grax = m_timeGraph -> GetYaxis() ;
  TAxis * hax  = m_rootHistogram -> GetYaxis() ;

  (( TNamed ) (*hax)).Copy( *grax ) ;
  (( TAttAxis ) (*hax)).Copy( *grax ) ;

  yMin = yMin - 0.05 * fabs( yMin );
  yMax = yMax + 0.05 * fabs( yMax );
  grax -> SetRangeUser( yMin, yMax ) ;
  std::cout << "Set User Range for YAxis of "<< aTrend.tag<<" to " << yMin << " " <<yMax<< std::endl;

  grax = m_timeGraph -> GetXaxis() ;
  hax  = m_rootHistogram -> GetXaxis() ;
  (( TAttAxis ) (*hax)).Copy( *grax ) ;
  grax -> SetRangeUser( m_timeArray[0], m_timeArray[size-1] ) ;

  (( TAttMarker ) (*m_rootHistogram)).Copy( *m_timeGraph ) ;
  (( TAttLine )   (*m_rootHistogram)).Copy( *m_timeGraph ) ;

  deleteRootHist();

  if ( update && m_hostingPad != NULL ) {
    m_hostingPad->Modified();
  }
}

//=========================================================================
//  Copy the argument into the root histogram
//=========================================================================
void DisplayHistogram::copyFrom ( TH1* src) {
  if ( 0 == m_rootHistogram ) return;
  if ( 0 != dynamic_cast<TH2D*>(m_rootHistogram) ) {
    if ( 0 ==  dynamic_cast<TH2D*>(src) ) return;
    if ( m_rootHistogram->GetNbinsX() != src->GetNbinsX() ) return;
    if ( m_rootHistogram->GetNbinsY() != src->GetNbinsY() ) return;
    for (int i=1; i<= m_rootHistogram->GetNbinsX() ; ++i) {
      for (int j=1; j <= m_rootHistogram->GetNbinsY() ; ++j) {
        m_rootHistogram->SetBinContent(i, j, src->GetBinContent(i, j) );
        m_rootHistogram->SetBinError(  i, j, src->GetBinError( i, j ) );
      }
    }
  } else {
    if ( m_rootHistogram->GetNbinsX() != src->GetNbinsX() ) return;
    for (int i = 1; i <= m_rootHistogram->GetNbinsX(); ++i) {
      m_rootHistogram->SetBinContent(i, src->GetBinContent(i) );
      m_rootHistogram->SetBinError(i,   src->GetBinError(i) );
    }
  }
  m_rootHistogram->SetEntries( src->GetEntries() );
}

//=========================================================================
//
//=========================================================================
TObject* DisplayHistogram::myObject ( ) {
  if ( 0 != m_rootHistogram ) {
    if ( 0 != dynamic_cast<TH1D*>( m_rootHistogram) ) return m_rootHistogram;
    if ( 0 != dynamic_cast<TH1F*>( m_rootHistogram) ) return m_rootHistogram;
  } else if ( 0 != m_timeGraph ) {
    return m_timeGraph;
  }
  return 0;
}

//=========================================================================
//  Save the options for this display histogram
//=========================================================================
void DisplayHistogram::saveOptionsToDB ( TPad* pad ) {
  if ( ( 0 == m_onlineHist ) ||
       ( ( 0 == m_rootHistogram ) && ( 0 == m_timeGraph ) ) ) return;

  bool out = false;

  int iopt = 0;
  float fopt = 0.0;
  std::string sopt("");

  // now title options

  TPaveText* tit = (TPaveText*)pad->GetPrimitive("title");
  if( tit && m_titPave && tit != m_titPave ) {
    fopt = float(tit->GetX1NDC());
    out |= updateDBOption("HTIT_X_OFFS", &fopt,
                          TMath::Abs(fopt - m_titPave->GetX1NDC())<0.0001);
    fopt = float(tit->GetX2NDC() - tit->GetX1NDC());
    out |= updateDBOption("HTIT_X_SIZE", &fopt,
                          TMath::Abs(fopt - (m_titPave->GetX2NDC()-m_titPave->GetX1NDC())) <0.0001);
    fopt = float(tit->GetY1NDC());
    out |= updateDBOption("HTIT_Y_OFFS", &fopt,
                          TMath::Abs(fopt - m_titPave->GetY1NDC())<0.001);
    fopt = float(tit->GetY2NDC() - tit->GetY1NDC());
    out |= updateDBOption("HTIT_Y_SIZE", &fopt,
                          TMath::Abs(fopt - (m_titPave->GetY2NDC()-m_titPave->GetY1NDC())) <0.0001);
    delete m_titPave;
    m_titPave = (TPaveText*) tit->Clone();
  }

  if ( 0 != m_rootHistogram ) {

    sopt = m_rootHistogram->GetXaxis()->GetTitle();
    out |= updateDBOption("LABEL_X", &sopt, sopt.empty());

    sopt = m_rootHistogram->GetYaxis()->GetTitle();
    out |= updateDBOption("LABEL_Y", &sopt, sopt.empty());
    // note: axis minima and maxima should not be set in this way, but
    // through the web interface

    TPaveStats* stats = (TPaveStats*)m_rootHistogram->GetListOfFunctions() ->FindObject("stats");
    // if histogram has not been plotted (or has been plotted without stats), do nothing
    if (stats && m_statPave && stats != m_statPave ) {

      iopt = (int) stats->GetOptStat();
      // 111110 seems to be hardcoded in root
      out |= updateDBOption("STATS", &iopt, iopt == 111110 );
      fopt = float(stats->GetX1NDC());
      out |= updateDBOption("STAT_X_OFFS", &fopt,
                            TMath::Abs(fopt - m_statPave->GetX1NDC())<0.001);
      fopt = float(stats->GetX2NDC() - stats->GetX1NDC());
      out |= updateDBOption("STAT_X_SIZE", &fopt,
                            TMath::Abs(fopt - (m_statPave->GetX2NDC()-m_statPave->GetX1NDC())) <0.001);
      fopt = float(stats->GetY1NDC());
      out |= updateDBOption("STAT_Y_OFFS", &fopt,
                            TMath::Abs(fopt - m_statPave->GetY1NDC())<0.001);
      fopt = float(stats->GetY2NDC() - stats->GetY1NDC());
      out |= updateDBOption("STAT_Y_SIZE", &fopt,
                            TMath::Abs(fopt - (m_statPave->GetY2NDC()-m_statPave->GetY1NDC())) <0.001);
      //delete m_statPave;
      //m_statPave = (TPaveStats*)stats->Clone();
    }


    iopt = (int) m_rootHistogram->GetFillStyle();
    out |= updateDBOption("FILLSTYLE", &iopt,
                          iopt == (int) gStyle->GetHistFillStyle());
    iopt = (int) m_rootHistogram->GetFillColor();
    out |= updateDBOption("FILLCOLOR", &iopt,
                          iopt == (int) gStyle->GetHistFillColor());
    iopt = (int) m_rootHistogram->GetLineStyle();
    out |= updateDBOption("LINESTYLE", &iopt,
                          iopt == (int) gStyle->GetHistLineStyle());
    iopt = (int) m_rootHistogram->GetLineColor();
    out |= updateDBOption("LINECOLOR", &iopt,
                          iopt == (int) gStyle->GetHistLineColor());
    iopt = (int) m_rootHistogram->GetLineWidth();
    out |= updateDBOption("LINEWIDTH", &iopt,
                          iopt == (int) gStyle->GetHistLineWidth());
    iopt = (int) m_rootHistogram->GetMarkerStyle();
    out |= updateDBOption("MARKERSTYLE", &iopt,
                          iopt == (int) gStyle->GetMarkerStyle());
    fopt = (float) m_rootHistogram->GetMarkerSize();
    out |= updateDBOption("MARKERSIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetMarkerSize())<0.00001 );
    iopt = (int) m_rootHistogram->GetMarkerColor();
    out |= updateDBOption("MARKERCOLOR", &iopt,
                          iopt == (int) gStyle->GetMarkerColor());

    sopt = m_rootHistogram->GetDrawOption();
    if (sopt.empty()) { sopt = m_rootHistogram->GetOption(); }
    out |= updateDBOption("DRAWOPTS",  &sopt, sopt.empty());

    fopt = m_rootHistogram->GetXaxis()->GetTitleSize();
    out |= updateDBOption("TIT_X_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleSize("X"))<0.0001);
    fopt = m_rootHistogram->GetXaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_X_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleOffset("X"))<0.0001);
    fopt = m_rootHistogram->GetXaxis()->GetLabelSize();
    out |= updateDBOption("LAB_X_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelSize("X"))<0.0001);
    fopt = m_rootHistogram->GetXaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_X_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelOffset("X"))<0.0001);
    iopt = m_rootHistogram->GetNdivisions("X");
    out |= updateDBOption("NDIVX", &iopt,
                          iopt == (int) gStyle->GetNdivisions("X"));

    fopt = m_rootHistogram->GetYaxis()->GetTitleSize();
    out |= updateDBOption("TIT_Y_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleSize("Y"))<0.0001);
    fopt = m_rootHistogram->GetYaxis()->GetTitleOffset();
    out |= updateDBOption("TIT_Y_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Y"))<0.0001);
    fopt = m_rootHistogram->GetYaxis()->GetLabelSize();
    out |= updateDBOption("LAB_Y_SIZE", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelSize("Y"))<0.0001);
    fopt = m_rootHistogram->GetYaxis()->GetLabelOffset();
    out |= updateDBOption("LAB_Y_OFFS", &fopt,
                          TMath::Abs(fopt - (float) gStyle->GetLabelOffset("Y"))<0.0001);
    iopt = m_rootHistogram->GetNdivisions("Y");
    out |= updateDBOption("NDIVY", &iopt,
                          iopt == (int) gStyle->GetNdivisions("Y"));

    if (m_onlineHist->dimension() > 1) {
      sopt = m_rootHistogram->GetZaxis()->GetTitle();
      out |= updateDBOption("LABEL_Z", &sopt, sopt.empty());
      fopt = m_rootHistogram->GetZaxis()->GetTitleSize();
      out |= updateDBOption("TIT_Z_SIZE",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetTitleSize("Z"))<0.0001);
      fopt = m_rootHistogram->GetZaxis()->GetTitleOffset();
      out |= updateDBOption("TIT_Z_OFFS",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetTitleOffset("Z"))<0.0001);
      fopt = m_rootHistogram->GetZaxis()->GetLabelSize();
      out |= updateDBOption("LAB_Z_SIZE",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetLabelSize("Z"))<0.0001);
      fopt = m_rootHistogram->GetZaxis()->GetLabelOffset();
      out |= updateDBOption("LAB_Z_OFFS",  &fopt,
                            TMath::Abs(fopt - (float) gStyle->GetLabelOffset("Z"))<0.0001);
    }
  }

  // now options from Pad ... be sure we are in the right Pad
  if (pad) {
    iopt = pad->GetLogx();
    out |= updateDBOption("LOGX", &iopt, iopt == gStyle->GetOptLogx());
    iopt = pad->GetLogy();
    out |= updateDBOption("LOGY", &iopt, iopt == gStyle->GetOptLogy());
    iopt = pad->GetGridx() ? 1 : 0;
    out |= updateDBOption("GRIDX", &iopt, (iopt>0) == gStyle->GetPadGridX());
    iopt = pad->GetGridy() ? 1 : 0;
    out |= updateDBOption("GRIDY", &iopt, (iopt>0) == gStyle->GetPadGridY());
    iopt = pad->GetTickx();
    out |= updateDBOption("TICK_X", &iopt, iopt == gStyle->GetPadTickX());
    iopt = pad->GetTicky();
    out |= updateDBOption("TICK_Y", &iopt, iopt == gStyle->GetPadTickY());
    fopt = (float)pad->GetTopMargin();
    out |= updateDBOption("MARGIN_TOP", &fopt,
                          TMath::Abs(fopt - (float)gStyle->GetPadTopMargin())<0.0001);
    fopt = (float)pad->GetBottomMargin();
    out |= updateDBOption("MARGIN_BOTTOM", &fopt,
                          TMath::Abs(fopt - (float)gStyle->GetPadBottomMargin())<0.0001);
    fopt = (float)pad->GetLeftMargin();
    out |= updateDBOption("MARGIN_LEFT", &fopt,
                          TMath::Abs(fopt - (float)gStyle->GetPadLeftMargin())<0.0001);
    fopt = (float)pad->GetRightMargin();
    out |= updateDBOption("MARGIN_RIGHT", &fopt,
                          TMath::Abs(fopt - (float)gStyle->GetPadRightMargin())<0.0001);
    iopt = pad->GetFillColor();
    out |= updateDBOption("PADCOLOR", &iopt, iopt == 10 ); // 10 corresponds to GetWhitePixel()
    //(int)gStyle->GetPadColor() );

    if (m_onlineHist->dimension() > 1) {
      iopt = pad->GetLogz();
      out |= updateDBOption("LOGZ", &iopt, iopt == gStyle->GetOptLogz());
      fopt = (float)pad->GetTheta();
      // seems to be hardcoded in root:
      out |= updateDBOption("THETA", &fopt, (int)fopt == 30);
      fopt = (float)pad->GetPhi();
      // seems to be hardcoded in root
      out |= updateDBOption("PHI", &fopt, (int)fopt == 30);
    }
  }
  if (out) m_onlineHist->saveDisplayOptions();
}

//=========================================================================
//  Update the option
//=========================================================================
bool DisplayHistogram::updateDBOption( std::string opt, int* value, bool isDefault) {
  if ( 0 == m_onlineHist  ) return false ;

  bool update = false;
  bool out = false;
  if ( m_onlineHist->isSetDisplayOption(opt) ) {
    update = m_onlineHist->changedDisplayOption( opt, value);
  } else if (!isDefault) {
    update=true;
  }
  if ( update ) {
    out = m_onlineHist->setDisplayOption(opt, value);
    std::cout << "Database option " << opt << " set to " << *value << " isDefault " << isDefault << std::endl;
  }
  return out;
}

//=========================================================================
//  Update the option
//=========================================================================
bool DisplayHistogram::updateDBOption( std::string opt, float* value, bool isDefault) {
  if ( 0 == m_onlineHist  ) return false ;

  bool update = false;
  bool out = false;
  if ( m_onlineHist->isSetDisplayOption(opt) ) {
    update = m_onlineHist->changedDisplayOption( opt, value);
  } else if (!isDefault) {
    update=true;
  }
  if ( update ) {
    out = m_onlineHist->setDisplayOption(opt, value);
    std::cout << "Database option " << opt << " set to " << *value << " isDefault " << isDefault << std::endl;
  }
  return out;
}

//=========================================================================
//  Update the option
//=========================================================================
bool DisplayHistogram::updateDBOption( std::string opt, std::string* value, bool isDefault) {
  if ( 0 == m_onlineHist  ) return false ;

  bool update = false;
  bool out = false;
  if ( m_onlineHist->isSetDisplayOption(opt) ) {
    update = m_onlineHist->changedDisplayOption( opt, value);
  } else if (!isDefault) {
    update=true;
  }
  if ( update ) {
    out = m_onlineHist->setDisplayOption(opt, value);
    std::cout << "Database option " << opt << " set to " << *value << " isDefault " << isDefault << std::endl;
  }
  return out;
}

//=========================================================================
//
//=========================================================================
void DisplayHistogram::setOnlineHistogram( OnlineHistogram* hist ) {
  if ( NULL != m_onlineHist &&
       hist != m_onlineHist ) delete m_onlineHist;
  m_onlineHist = hist;
}

//=========================================================================
//  Set the local copy of the options, by type
//=========================================================================
void DisplayHistogram::loadOptions ( ) {
  if ( NULL == m_onlineHist ) {
    m_intOptions.clear();
    m_floatOptions.clear();
    m_stringOptions.clear();
    m_optionsAreLoaded = true;
    return;
  }

  m_onlineHist->getIntDisplayOptions( m_intOptions );
  m_onlineHist->getFloatDisplayOptions( m_floatOptions );
  m_onlineHist->getStringDisplayOptions( m_stringOptions );
  std::cout << "Retrieved " << m_intOptions.size() << " int, " << m_floatOptions.size()
            << " float and " << m_stringOptions.size() << " string options." << std::endl;
  m_optionsAreLoaded = true;
}

//=============================================================================
