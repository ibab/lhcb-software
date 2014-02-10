
//-----------------------------------------------------------------------------
/** @file DAQHistoAlgBase.h
 *
 *  Header file for algorithm base class : DAQHistoAlgBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQHISTOALGBASE_H
#define DAQKERNEL_DAQHISTOALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "DAQKernel/DAQHistoBase.h"

// histogramming utilities
#include "DAQKernel/DAQHistoID.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class HistoAlgBase DAQHistoAlgBase.h DAQKernel/DAQHistoAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming
   *  functionality from the base class GaudiHistoAlg.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class HistoAlgBase : public DAQ::HistoBase<GaudiHistoAlg>
  {

  public:

    /// Standard constructor
    HistoAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : DAQ::HistoBase<GaudiHistoAlg> ( name, pSvcLocator )
    { }

  };

}

#endif // DAQKERNEL_DAQHISTOALGBASE_H
