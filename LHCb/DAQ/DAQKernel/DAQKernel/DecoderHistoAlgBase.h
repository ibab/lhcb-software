
//-----------------------------------------------------------------------------
/** @file DAQHistoAlgBase.h
 *
 *  Header file for algorithm base class : DecoderHistoAlgBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   02/10/2014
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERHISTOALGBASE_H
#define DAQKERNEL_DECODERHISTOALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoAlg.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
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

  class HistoAlgBase : public Decoder::CommonBase<GaudiHistoAlg>
  {

  public:

    /// Standard constructor
    HistoAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Decoder::HistoBase<GaudiHistoAlg> ( name, pSvcLocator )
    { }

  };

}

#endif // DAQKERNEL_DECODERHISTOALGBASE_H
