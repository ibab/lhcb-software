
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STToolBase.h,v 1.2 2008-10-16 13:08:02 mneedham Exp $
 *
 *  @author Matthew Needham
 *  @date   11/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef STKERNEL_STTOOLBASE_H
#define STKERNEL_STTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTool.h"

// local
#include "Kernel/STCommonBase.h"

namespace ST
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase STToolBase.h STKernel/STToolBase.h
   *
   *  Abstract base class for ST algorithms providing
   *  some basic functionality.
   *
   *  @author Matthew Needham
   *  @date   11/10/2008
   */
  //-----------------------------------------------------------------------------

  class ToolBase : public ST::CommonBase<GaudiTool>
  {

  public:

    /// Standard constructor
    ToolBase(const std::string& type, 
             const std::string& name,
             const IInterface* parent )
      : ST::CommonBase<GaudiTool> ( type, name, parent ) { }

  };

}


#endif // STKERNEL_STALGBASE_H
