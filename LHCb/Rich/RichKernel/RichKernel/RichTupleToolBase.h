
//-----------------------------------------------------------------------------
/** @file RichTupleToolBase.h
 *
 *  Header file for tool base class : RichTupleToolBase
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
#include "RichKernel/RichHistoBase.h"

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

  class TupleToolBase : public Rich::HistoBase<GaudiTupleTool>
  {

  public:

    /// Standard constructor
    TupleToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : Rich::HistoBase<GaudiTupleTool> ( type, name, parent )
    { }

  };

}

#endif // RICHKERNEL_RICHTUPLETOOLBASE_H
