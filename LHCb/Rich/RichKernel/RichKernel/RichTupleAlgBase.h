
//-----------------------------------------------------------------------------
/** @file RichTupleAlgBase.h
 *
 *  Header file for algorithm base class : RichTupleAlgBase
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
#include "RichKernel/RichHistoBase.h"

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

  class TupleAlgBase : public Rich::HistoBase<GaudiTupleAlg>
  {

  public:

    /// Standard constructor
    TupleAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Rich::HistoBase<GaudiTupleAlg> ( name, pSvcLocator )
    { }

  };

}

#endif // RICHKERNEL_RICHTUPLEALGBASE_H
