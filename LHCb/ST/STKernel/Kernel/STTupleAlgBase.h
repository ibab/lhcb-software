
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STTupleAlgBase.h,v 1.2 2008-10-16 13:08:02 mneedham Exp $
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
    TupleAlgBase(const std::string& type, 
             const std::string& name,
             const IInterface* parent )
      : ST::CommonBase<GaudiTupleAlg> ( type, name, parent ) { }

  };

}


#endif // STKERNEL_STALGBASE_H
