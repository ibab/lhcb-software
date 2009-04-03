#include "HltBase/HltHistogramUtilities.h"
#include "GaudiUtils/Aida2ROOT.h"
// ============================================================================
// ROOT
// ============================================================================
#include "TH1D.h"
#include "TH2D.h"

namespace Hlt {
namespace HistogramUtilities {
    bool setBinLabels( TAxis* axis,  const std::vector<std::pair<unsigned,std::string> >& labels ) {
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

    bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::pair<unsigned,std::string> >& labels ) {
        if (hist==0) return false;
        TH1D *h1d = Gaudi::Utils::Aida2ROOT::aida2root( hist );  
        if (h1d==0) return false;
        return setBinLabels(  h1d->GetXaxis(), labels );
    }

    bool setBinLabels( AIDA::IHistogram1D* hist, const std::vector<std::string>& labels ) {
        std::vector<std::pair<unsigned,std::string> > l;
        for (unsigned i = 0;i<labels.size();++i) {
            l.push_back(std::make_pair( i , labels[i] ) );
        }
        return setBinLabels(hist,l);
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
        return setBinLabels(  h2d->GetXaxis(), lx) && setBinLabels( h2d->GetYaxis(), ly );
    }

  bool setAxisLabels( AIDA::IHistogram1D* hist, const std::string & xAxis,const std::string & yAxis  ) {
    if (hist==0) return false;
    TH1D *h1d = Gaudi::Utils::Aida2ROOT::aida2root( hist );  
    if (h1d==0) return false;
    h1d->SetXTitle(xAxis.c_str());
    h1d->SetYTitle(yAxis.c_str());
    return true;
  }
};
};
