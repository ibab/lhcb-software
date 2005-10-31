
//-----------------------------------------------------------------------------
/** @file RichHistoToolBase.h
 *
 *  Header file for tool base class : RichHistoToolBase
 *
 *  CVS Log :-
 *  $Id: RichHistoToolBase.h,v 1.1 2005-10-31 13:26:10 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHHISTOTOOLBASE_H
#define RICHKERNEL_RICHHISTOTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoTool.h"

// local
#include "RichKernel/RichCommonBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

//-----------------------------------------------------------------------------
/** @class RichHistoToolBase RichHistoToolBase.h RichKernel/RichHistoToolBase.h
 *
 *  Abstract base class for RICH tools providing some basic functionality.
 *
 *  In addition, uses the histogramming
 *  functionality from the base class GaudiTupleAlg.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

class RichHistoToolBase : public RichCommonBase<GaudiHistoTool>
{

public:

  /// Standard constructor
  RichHistoToolBase( const std::string& type,
                     const std::string& name,
                     const IInterface* parent )
    : RichCommonBase<GaudiHistoTool> ( type, name, parent )
  {
    setHistoTopDir( "RICH/" );
  }

};

#endif // RICHKERNEL_RICHHISTOTOOLBASE_H
