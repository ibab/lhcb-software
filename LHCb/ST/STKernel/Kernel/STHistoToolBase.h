
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STHistoToolBase.h,v 1.1 2008-10-14 08:47:05 mneedham Exp $
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
    HistoToolBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : ST::CommonBase<GaudiHistoTool> ( name, pSvcLocator ) { }

  };

}


#endif // STKERNEL_STHISTOTOOLBASE_H
