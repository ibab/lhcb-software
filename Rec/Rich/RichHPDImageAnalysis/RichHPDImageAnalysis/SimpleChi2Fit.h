// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H
#define RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "Minuit2/FCNBase.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class SimpleChi2Fit RichHPDImageAnalysis/SimpleChi2Fit.h
     *
     *  Simple Chi^2 fit
     *
     *  @author Chris Jones
     *  @date   2011-03-05
     */
    class SimpleChi2Fit : public ROOT::Minuit2::FCNBase
    {

    public:

      /// Default Constructor
      SimpleChi2Fit() 
        : m_errDef(0), m_sf(0), m_boundary(NULL) 
      { }

      /// Constructor
      SimpleChi2Fit( const TH2* hist,
                     const Pixel::List& list );

      virtual ~SimpleChi2Fit( ); ///< Destructor

    public:

      virtual double operator()( const std::vector<double>& par ) const ;

      virtual double Up() const{  return m_errDef; }
      void setErrDef( const double def ) { m_errDef = def; }

    private:

      double m_errDef ;
      double m_sf ;
      const Pixel::List * m_boundary ;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_SIMPLECHI2FIT_H
