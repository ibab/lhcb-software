
//-----------------------------------------------------------------------------
/** @file RichHistoToolBase.h
 *
 *  Header file for tool base class : RichHistoToolBase
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
#include "RichKernel/RichHistoBase.h"

// histogramming utilities
#include "RichKernel/RichHistoID.h"

namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @class HistoToolBase RichHistoToolBase.h RichKernel/RichHistoToolBase.h
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

  class HistoToolBase : public Rich::HistoBase<GaudiHistoTool>
  {

  public:

    /// Standard constructor
    HistoToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : Rich::HistoBase<GaudiHistoTool> ( type, name, parent )
    { }

  };

}

#endif // RICHKERNEL_RICHHISTOTOOLBASE_H
