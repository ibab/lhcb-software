
//-----------------------------------------------------------------------------
/** @file STHistoBase.h
 *
 *  Header file for algorithm base class : ST::AlgBase
 *
 *  CVS Log :-
 *  $Id: STHistoAlgBase.h,v 1.1 2008-10-14 08:47:05 mneedham Exp $
 *
 *  @author Matthew Needham
 *  @date   11/10/2008
 */
//-----------------------------------------------------------------------------

#ifndef STKERNEL_STHISTOALGBASE_H
#define STKERNEL_STHISTOALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "Kernel/STCommonBase.h"

namespace ST
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase STHistoBase.h STKernel/STHistoBase.h
   *
   *  Abstract base class for ST algorithms providing
   *  some basic functionality.
   *
   *  @author Matthew Needham
   *  @date   11/10/2008
   */
  //-----------------------------------------------------------------------------

  class HistoAlgBase : public ST::CommonBase<GaudiHistoAlg>
  {

  public:

    /// Standard constructor
    HistoAlgBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : ST::CommonBase<GaudiHistoAlg> ( name, pSvcLocator ) { }

  };

}


#endif // STKERNEL_STALGBASE_H
