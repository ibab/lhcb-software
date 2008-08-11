// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OMAlib/algorithms/OMAHMerge.cpp,v 1.2 2008-08-11 08:05:15 ggiacomo Exp $
#include <TH2D.h>
#include "OMAlib/OMAAlgorithms.h"
#include "OMAlib/OMAlib.h"


OMAHMerge::OMAHMerge(OMAcommon* Env) : 
  OMAHcreatorAlg("HMerge", Env) {
  m_ninput = 8;
  m_histSetFlag = true;
  m_npars = 0;
  m_outHType = OnlineHistDBEnv::SAM;
  m_doc = "Merge histograms into a single histogram with possibly variable bin sizes";
}

TH1* OMAHMerge::exec( const std::vector<TH1*> *sources,
			  const std::vector<float> *params,
			  std::string outName,
			  std::string outTitle,
			  TH1* existingHisto) {
  TH1* out=NULL;
  if (!sources) return out;
  if(sources->empty()) return out;
  params=params; // avoid compil. warning
  if(existingHisto) {
    fillMerged(existingHisto, sources);
    out = existingHisto;
  } else {    
    out = hMerge(outName.data(), outTitle.data(), sources);
  }  
  return out;
}

bool OMAHMerge::approxeq(double x, double y ) 
{
  return ( ((int) ( (x-y)/(OMAconstants::epsilon)  +0.1) ) == 0);
}


TH1* OMAHMerge::hMerge(const char* newname, const char* newtitle, const std::vector<TH1*> *in) 
{
  std::vector<Double_t> steps[2];
  std::vector<Double_t>::iterator ix;
  TH1* newH = NULL;
  int dimension =in->at(0)->GetDimension();

  std::vector<TH1*>::const_iterator ih;
  for (int ia=0; ia<dimension; ia++) {
    for (ih=in->begin(); ih != in->end(); ih++) {
      if( (*ih)->GetDimension() != dimension ) return newH;
      TAxis* ax = ( (ia == 0 ) ? (*ih)->GetXaxis() :  (*ih)->GetYaxis() );
      Double_t delta = (ax->GetXmax() - ax->GetXmin()) / ax->GetNbins();
      
      if(ih == in->begin()) {
        (steps[ia]).clear();
        for (int ib=0; ib<= ax->GetNbins(); ib++)
          (steps[ia]).push_back(ax->GetXmin() + ib*delta );
      }
      else {
      ix=(steps[ia]).begin();
      Double_t x=0.;
      for (int ib=0; ib<= ax->GetNbins(); ib++) {
        x = ax->GetXmin() + ib*delta;
        while ( ix != (steps[ia]).end() && 
                *ix < x  &&
                (! approxeq(x, *ix)) ) 
          ix++;
        if ( ! approxeq(x, *ix) ) 
          ix = (steps[ia]).insert(ix,x);
      }
      
      }
    }
  }

  if ( false == (steps[0]).empty() ) {
    const Double_t* xl = &((steps[0]).at(0));
    if (dimension == 1) {
      newH = new TH1D(newname , newtitle, 
                      (steps[0]).size() -1 , xl);
    }
    else if(dimension == 2) {
      if ( false == (steps[1]).empty() ) {
        const Double_t* yl = &((steps[1]).at(0));
        newH = new TH2D(newname , newtitle, 
                        (steps[0]).size() -1 , xl,
                        (steps[1]).size() -1 , yl );
      }
    }
    if (newH) {
      newH->Sumw2();
      fillMerged((TH1*) newH, in);    
    }
  }
  return (TH1*) newH;    
}

void OMAHMerge::fillMerged(TH1* newH, const std::vector<TH1*> *in)
{
  int dimension =newH->GetDimension();
  
  std::vector<TH1*>::const_iterator ih;
  for (ih=in->begin(); ih != in->end(); ih++) {
     if( (*ih)->GetDimension() != dimension  ) return;
    for (int ix=1 ; ix<= (*ih)->GetNbinsX(); ix++) {
      for (int iy=1 ; iy<= (*ih)->GetNbinsY(); iy++) {
        double cont = (*ih)->GetBinContent(ix,iy);
        double xlow = (*ih)->GetXaxis()->GetBinLowEdge(ix);
        double xup = (*ih)->GetXaxis()->GetBinUpEdge(ix);
        double ylow = (*ih)->GetYaxis()->GetBinLowEdge(iy);
        double yup = (*ih)->GetYaxis()->GetBinUpEdge(iy);
        int Bin;
        if (dimension == 2) {
          Bin = (dynamic_cast<TH2*>(newH))->Fill(xlow+OMAconstants::epsilon,ylow+OMAconstants::epsilon,cont);
        }
        else {
          Bin = newH->Fill(xlow+OMAconstants::epsilon,cont);
        }
        
        // check there are no other bins to fill        
        int wx,wy,wz;        
        newH->GetBinXYZ(Bin,wx,wy,wz);
        int wxmin=wx;
        while (  wx < newH->GetNbinsX() &&
                 newH->GetXaxis()->GetBinCenter(wx+1) < xup ) {
          if (dimension == 2) {
            (dynamic_cast<TH2*>(newH))->Fill(newH->GetXaxis()->GetBinCenter(wx+1), ylow+OMAconstants::epsilon ,cont);
          }
           else {
             newH->Fill(newH->GetXaxis()->GetBinCenter(wx+1),cont);
           }
          wx++;
        }
        if (dimension == 2) {
          while (  wy < newH->GetNbinsY() &&
                   newH->GetYaxis()->GetBinCenter(wy+1) < yup ) {
            for (int ixy= wxmin ; ixy<=wx ; ixy++) {
              (dynamic_cast<TH2*>(newH))->Fill(newH->GetXaxis()->GetBinCenter(ixy),
                                               newH->GetYaxis()->GetBinCenter(wy+1),cont);
            }
            wy++;
          }
        }
      }
    }
  }
  
}
