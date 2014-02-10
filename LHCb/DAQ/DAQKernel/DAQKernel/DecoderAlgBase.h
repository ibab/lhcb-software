
//-----------------------------------------------------------------------------
/** @file DAQAlgBase.h
 *
 *  Header file for algorithm base class : DAQ::AlgBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQALGBASE_H
#define DAQKERNEL_DAQALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "DAQKernel/DAQCommonBase.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase DAQAlgBase.h DAQKernel/DAQAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing
   *  some basic functionality.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class AlgBase : public DAQ::CommonBase<GaudiAlgorithm>
  {
    
  public:
    
    /// Standard constructor
    AlgBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : DAQ::CommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }
    
  };
  
}

#endif // DAQKERNEL_DAQALGBASE_H
