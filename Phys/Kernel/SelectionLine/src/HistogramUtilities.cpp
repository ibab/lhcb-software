#include "HistogramUtilities.h"
#include "GaudiUtils/Aida2ROOT.h"
// ============================================================================
// ROOT
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"
#include "TProfile.h"

namespace {
  template <typename R, typename A>
  bool setAxisLabels_( A* aida, const std::string& xAxis, const std::string& yAxis ) {
    if (aida==0) return false;
    R *root = Gaudi::Utils::Aida2ROOT::aida2root( aida );
    if (root==0) return false;
    root->SetXTitle(xAxis.c_str());
    root->SetYTitle(yAxis.c_str());
    return true;
  }

  bool setBinLabels_( TAxis* axis,  const std::vector<std::pair<unsigned,std::string> >& labels ) {
    if (axis==0) return false;
    unsigned nbins = axis->GetNbins();
    for (std::vector<std::pair<unsigned,std::string> >::const_iterator i = labels.begin();i!=labels.end();++i ) {
      //TODO: check bin exists...
      if (1+i->first <= 0 ||  1+i->first > nbins ) return false;
      // Argh... ROOT bins start counting at '1' instead of '0'
      axis -> SetBinLabel(1 + i->first  ,i->second.c_str() );
    }
    return true;
  }

  template <typename R, typename A>
  bool setBinLabels_( A* aida, const std::vector<std::pair<unsigned,std::string> >& labels ) {
    if (aida==0) return false;
    R *root = Gaudi::Utils::Aida2ROOT::aida2root( aida );
    if (root==0) return false;
    return setBinLabels_( root->GetXaxis(), labels );
  }

  template <typename Histogram>
  bool setBinLabels_( Histogram* hist, const std::vector<std::string>& labels ) {
    std::vector<std::pair<unsigned,std::string> > l;
    for (unsigned i = 0;i<labels.size();++i) l.push_back(std::make_pair( i , labels[i] ) );
    return HistogramUtilities::setBinLabels(hist,l);
  }
}

namespace HistogramUtilities {

  bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::pair<unsigned,std::string> >& labels ) {
    return setBinLabels_<TH1D>(hist,labels);
  }
  bool setBinLabels( AIDA::IProfile1D* hist, const std::vector<std::pair<unsigned,std::string> >& labels ) {
    return setBinLabels_<TProfile>(hist, labels);
  }

  bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::string>& labels ) {
    return setBinLabels_(hist,labels);
  }
  bool setBinLabels( AIDA::IProfile1D* hist, const std::vector<std::string>& labels ) {
    return setBinLabels_(hist,labels);
  }


  bool setBinLabels( AIDA::IHistogram2D* hist, const std::vector<std::string>& xlabels,
                     const std::vector<std::string>& ylabels) {
    if (hist==0) return false;
    std::vector<std::pair<unsigned,std::string> > lx;
    for (unsigned i = 0;i<xlabels.size();++i) {
      lx.push_back(std::make_pair( i , xlabels[i] ) );
    }
    std::vector<std::pair<unsigned,std::string> > ly;
    for (unsigned i = 0;i<ylabels.size();++i) {
      ly.push_back(std::make_pair( i , ylabels[i] ) );
    }
    TH2D *h2d = Gaudi::Utils::Aida2ROOT::aida2root( hist );
    if (h2d==0) return false;
    return setBinLabels_(  h2d->GetXaxis(), lx) && setBinLabels_( h2d->GetYaxis(), ly );
  }

  bool setAxisLabels( AIDA::IHistogram1D* hist, const std::string & xAxis,const std::string & yAxis  ) {
    return setAxisLabels_<TH1D>( hist, xAxis, yAxis );
  }

  bool setAxisLabels( AIDA::IProfile1D* hist, const std::string & xAxis,const std::string & yAxis  ) {
    return setAxisLabels_<TProfile>( hist, xAxis, yAxis );
  }


#if 0
  bool shiftBinsLeft( AIDA::IProfile1D* profile ) {
    if (profile==0) return false;
    TProfile *prof = Gaudi::Utils::Aida2ROOT::aida2root( profile );
    if (prof==0) return false;
  }
#endif

}
