
//-----------------------------------------------------------------------------
/** @file RichAlgBase.h
 *
 *  Header file for algorithm base class : Rich::AlgBase
 *
 *  CVS Log :-
 *  $Id: RichAlgBase.h,v 1.11 2007-02-01 17:24:54 jonrob Exp $
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

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
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
