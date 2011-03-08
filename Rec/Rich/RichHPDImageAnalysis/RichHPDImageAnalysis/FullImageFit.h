// $Id: $
#ifndef RICHHPDIMAGEANALYSIS_FullImageFit_H
#define RICHHPDIMAGEANALYSIS_FullImageFit_H 1

#include "RichHPDImageAnalysis/HPDPixel.h"
#include "Minuit2/FCNBase.h"
#include "TH2D.h"

namespace Rich
{
  namespace HPDImage
  {

    /** @class FullImageFit RichHPDImageAnalysis/FullImageFit.h
     *
     *  Fit to the full histogram image.
     *
     *  @author Chris Jones
     *  @date   2011-03-05
     */
    class FullImageFit : public ROOT::Minuit2::FCNBase
    {

    public:

      /// Default Constructor
      FullImageFit() : m_errDef(0), m_hist(NULL), m_sf(0) { }

      /// Constructor
      FullImageFit( const TH2* hist );

      virtual ~FullImageFit( ); ///< Destructor

    public:

      virtual double operator()( const std::vector<double>& par ) const ;

      virtual double Up() const{  return m_errDef; }
      void setErrDef( const double def ) { m_errDef = def; }

    private:

      double m_errDef ;
      const TH2* m_hist ;
      double m_sf ;

    };

  }
}

#endif // RICHHPDIMAGEANALYSIS_FullImageFit_H
