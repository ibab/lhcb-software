
//-----------------------------------------------------------------------------
/** @file RichAlgBase.h
 *
 *  Header file for algorithm base class : RichAlgBase
 *
 *  CVS Log :-
 *  $Id: RichAlgBase.h,v 1.10 2005-10-13 15:03:41 jonrob Exp $
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
/** @class RichAlgBase RichAlgBase.h RichKernel/RichAlgBase.h
 *
 *  Abstract base class for RICH algorithms providing
 *  some basic functionality.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichAlgBase : public RichCommonBase<GaudiAlgorithm>
{

public:

  /// Standard constructor
  RichAlgBase( const std::string& name,
               ISvcLocator* pSvcLocator )
  : RichCommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }

};

#endif // RICHKERNEL_RICHALGBASE_H
