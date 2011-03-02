// $Id: $
#ifndef LIB_HPDBOUNDARYFCN_H
#define LIB_HPDBOUNDARYFCN_H 1

#include "Minuit2/FCNBase.h"
#include "TH2D.h"
#include <cmath>

namespace Rich
{
  namespace HPDImage
  {

    /** @class RichHPDImageSummary RichHPDImageSummary.h
     *
     *  Boundary function for HPD fit
     *
     *  @author Thomas BLAKE
     *  @date   2010-03-16
     */
    class HPDBoundaryFcn : public ROOT::Minuit2::FCNBase
    {

    public:

      /// Standard constructor
      HPDBoundaryFcn( const TH2* hist = NULL,
                      const double thr = 0.0 );

      virtual ~HPDBoundaryFcn( ); ///< Destructor

    public:

      virtual double operator()(const std::vector<double>& par ) const ;
      virtual double Up() const{  return m_errDef; }
      void setErrDef( const double def ){ m_errDef = def; }

      unsigned int findBoundary() const;
      double nPixels() const;

    protected:

      bool hasNeighbour( const int COL, const int ROW, const double thr ) const ;

    private:

      double m_errDef ;
      double m_threshold ;
      const TH2* m_hist ;
      double m_sf ;
      mutable std::vector< std::pair< int, int > > m_boundary ;

    };

  }
}

inline double Rich::HPDImage::HPDBoundaryFcn::nPixels() const
{
  return ( m_hist ? m_hist->Integral() : 0.0 );
}

#endif // LIB_HPDBOUNDARYFCN_H
