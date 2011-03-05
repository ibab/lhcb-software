// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H
#define RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H 1

#include "Minuit2/FCNBase.h"
#include "TH2D.h"
#include "RichHPDImageAnalysis/HPDPixel.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class SimpleChi2Fit SimpleChi2Fit.h RichHPDImageAnalysis/SimpleChi2Fit.h
     *
     *  Simple Chi^2 fit
     *
     *  @author Chris Jones
     *  @date   2011-03-05
     */
    class SimpleChi2Fit : public ROOT::Minuit2::FCNBase
    {

    public:

      /// Standard constructor
      SimpleChi2Fit( const TH2* hist,
                     const Pixel::List& list );

      virtual ~SimpleChi2Fit( ); ///< Destructor

    public:

      virtual double operator()( const std::vector<double>& par ) const ;

      virtual double Up() const{  return m_errDef; }
      void setErrDef( const double def ){ m_errDef = def; }

    private:

      double m_errDef ;
      const TH2* m_hist ;
      double m_sf ;
      Pixel::List m_boundary ;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H
