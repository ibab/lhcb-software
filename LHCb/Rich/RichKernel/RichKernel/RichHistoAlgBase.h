
//-----------------------------------------------------------------------------
/** @file RichHistoAlgBase.h
 *
 *  Header file for algorithm base class : RichHistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichHistoAlgBase.h,v 1.1 2005-10-31 13:26:10 jonrob Exp $
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

//-----------------------------------------------------------------------------
/** @class RichHistoAlgBase RichHistoAlgBase.h RichKernel/RichHistoAlgBase.h
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

class RichHistoAlgBase : public RichCommonBase<GaudiHistoAlg>
{

public:

  /// Standard constructor
  RichHistoAlgBase( const std::string& name,
                    ISvcLocator* pSvcLocator )
    : RichCommonBase<GaudiHistoAlg> ( name, pSvcLocator )
  {
    setHistoTopDir( "RICH/" );
  }

};

#endif // RICHKERNEL_RICHHISTOALGBASE_H
