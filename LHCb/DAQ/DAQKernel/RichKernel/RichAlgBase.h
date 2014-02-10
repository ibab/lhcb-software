
//-----------------------------------------------------------------------------
/** @file RichAlgBase.h
 *
 *  Header file for algorithm base class : Rich::AlgBase
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHALGBASE_H
#define RICHKERNEL_RICHALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "RichKernel/RichCommonBase.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase RichAlgBase.h RichKernel/RichAlgBase.h
   *
   *  Abstract base class for RICH algorithms providing
   *  some basic functionality.
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class AlgBase : public Rich::CommonBase<GaudiAlgorithm>
  {
    
  public:
    
    /// Standard constructor
    AlgBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : Rich::CommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }
    
  };
  
}

#endif // RICHKERNEL_RICHALGBASE_H
