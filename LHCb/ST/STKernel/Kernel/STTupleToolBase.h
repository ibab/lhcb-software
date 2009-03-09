
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STTupleToolBase.h,v 1.2 2009-03-09 15:14:26 akeune Exp $
 *
 *  @author Matthew Needham
 *  @date   11/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef STKERNEL_STHISTOTOOLBASE_H
#define STKERNEL_STHISTOTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "Kernel/STCommonBase.h"

namespace ST
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase STTupleToolBase.h STKernel/STTupleToolBase.h
   *
   *  Abstract base class for ST algorithms providing
   *  some basic functionality.
   *
   *  @author Matthew Needham
   *  @date   11/10/2008
   */
  //-----------------------------------------------------------------------------

  class TupleToolBase : public ST::CommonBase<GaudiTupleTool>
  {

  public:
    
    /// Standard constructor
    TupleToolBase(const std::string& type, 
                  const std::string& name,
                  const IInterface* parent )
      : ST::CommonBase<GaudiTupleTool> ( type, name, parent ) { }
    
  };
  
}


#endif // STKERNEL_STHISTOTOOLBASE_H
