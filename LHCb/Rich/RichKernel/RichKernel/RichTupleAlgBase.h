
//-----------------------------------------------------------------------------
/** @file RichTupleAlgBase.h
 *
 *  Header file for algorithm base class : RichTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: RichTupleAlgBase.h,v 1.6 2009-06-10 13:13:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTUPLEALGBASE_H
#define RICHKERNEL_RICHTUPLEALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class TupleAlgBase RichTupleAlgBase.h RichKernel/RichTupleAlgBase.h
   *
   *  Abstract base class for RICH algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleAlg.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class TupleAlgBase : public Rich::CommonBase<GaudiTupleAlg>
  {

  public:

    /// Standard constructor
    TupleAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Rich::CommonBase<GaudiTupleAlg> ( name, pSvcLocator )
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
typedef Rich::TupleAlgBase RichTupleAlgBase;

#endif // RICHKERNEL_RICHTUPLEALGBASE_H
