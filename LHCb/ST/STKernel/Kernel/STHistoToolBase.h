
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STHistoToolBase.h,v 1.2 2008-10-16 13:08:02 mneedham Exp $
 *
 *  @author Matthew Needham
 *  @date   11/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef STKERNEL_STHISTOTOOLBASE_H
#define STKERNEL_STHISTOTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoTool.h"

// local
#include "Kernel/STCommonBase.h"

namespace ST
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase STHistoToolBase.h STKernel/STHistoToolBase.h
   *
   *  Abstract base class for ST algorithms providing
   *  some basic functionality.
   *
   *  @author Matthew Needham
   *  @date   11/10/2008
   */
  //-----------------------------------------------------------------------------

  class HistoToolBase : public ST::CommonBase<GaudiHistoTool>
  {

  public:

    /// Standard constructor
    HistoToolBase(const std::string& type, 
             const std::string& name,
             const IInterface* parent )
      : ST::CommonBase<GaudiHistoTool> ( type, name, parent ) { }

  };

}


#endif // STKERNEL_STHISTOTOOLBASE_H
