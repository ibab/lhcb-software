
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STTupleAlgBase.h,v 1.3 2009-03-09 15:14:26 akeune Exp $
 *
 *  @author Matthew Needham
 *  @date   11/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef STKERNEL_STTUPLEALGBASE_H
#define STKERNEL_STTUPLEALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "Kernel/STCommonBase.h"

namespace ST
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase STTupleBase.h STKernel/STTupleBase.h
   *
   *  Abstract base class for ST algorithms providing
   *  some basic functionality.
   *
   *  @author Matthew Needham
   *  @date   11/10/2008
   */
  //-----------------------------------------------------------------------------

  class TupleAlgBase : public ST::CommonBase<GaudiTupleAlg>
  {

  public:
    
    /// Standard constructor
    TupleAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : ST::CommonBase<GaudiTupleAlg> ( name, pSvcLocator ) { }
    
  };
  
}


#endif // STKERNEL_STALGBASE_H
