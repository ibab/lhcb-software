
//-----------------------------------------------------------------------------
/** @file RichTupleToolBase.h
 *
 *  Header file for tool base class : RichTupleToolBase
 *
 *  CVS Log :-
 *  $Id: RichTupleToolBase.h,v 1.6 2009-06-10 13:13:00 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHTUPLETOOLBASE_H
#define RICHKERNEL_RICHTUPLETOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class TupleToolBase RichTupleToolBase.h RichKernel/RichTupleToolBase.h
   *
   *  Abstract base class for RICH tools providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleTool.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class TupleToolBase : public Rich::CommonBase<GaudiTupleTool>
  {

  public:

    /// Standard constructor
    TupleToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : Rich::CommonBase<GaudiTupleTool> ( type, name, parent )
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
typedef Rich::TupleToolBase RichTupleToolBase;

#endif // RICHKERNEL_RICHTUPLETOOLBASE_H
