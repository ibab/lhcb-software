
//-----------------------------------------------------------------------------
/** @file RichAlgBase.h
 *
 *  Header file for algorithm base class : Rich::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichAlgBase.h,v 1.13 2009-07-07 16:24:53 jonrob Exp $
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

/** For backwards compatibility
 *  @todo Remove this typedef
 */
typedef Rich::AlgBase RichAlgBase;

#endif // RICHKERNEL_RICHALGBASE_H
