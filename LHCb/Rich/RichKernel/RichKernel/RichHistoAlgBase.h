
//-----------------------------------------------------------------------------
/** @file RichHistoAlgBase.h
 *
 *  Header file for algorithm base class : RichHistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichHistoAlgBase.h,v 1.5 2009-06-10 13:13:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOALGBASE_H
#define RICHKERNEL_RICHHISTOALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HistoAlgBase RichHistoAlgBase.h RichKernel/RichHistoAlgBase.h
   *
   *  Abstract base class for RICH algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming
   *  functionality from the base class GaudiHistoAlg.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class HistoAlgBase : public Rich::CommonBase<GaudiHistoAlg>
  {

  public:

    /// Standard constructor
    HistoAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Rich::CommonBase<GaudiHistoAlg> ( name, pSvcLocator )
    {
      declareProperty ( "NBins1DHistos", m_nBins1D = 100 );
      declareProperty ( "NBins2DHistos", m_nBins2D = 50  );
    }

  protected:

    /// Number of bins for 1D histograms
    inline unsigned int nBins1D() const { return m_nBins1D; }

    /// Number of bins for 2D histograms
    inline unsigned int nBins2D() const { return m_nBins2D; }

  private:

    unsigned int m_nBins1D; ///< Number of bins for 1D histograms
    unsigned int m_nBins2D; ///< Number of bins for 2D histograms

  };

}

/** For backwards compatibility
 *  @todo Remove this typedef
 */
typedef Rich::HistoAlgBase RichHistoAlgBase;

#endif // RICHKERNEL_RICHHISTOALGBASE_H
