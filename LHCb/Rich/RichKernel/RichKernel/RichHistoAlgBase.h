
//-----------------------------------------------------------------------------
/** @file RichHistoAlgBase.h
 *
 *  Header file for algorithm base class : RichHistoAlgBase
 *
 *  CVS Log :-
 *  $Id: RichHistoAlgBase.h,v 1.2 2007-02-01 17:24:54 jonrob Exp $
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
      setHistoTopDir( "RICH/" );
    }

  };

}

/** For backwards compatibility
 *  @todo Remove this typedef
 */
typedef Rich::HistoAlgBase RichHistoAlgBase;

#endif // RICHKERNEL_RICHHISTOALGBASE_H
