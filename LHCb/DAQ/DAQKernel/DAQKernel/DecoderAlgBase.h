
//-----------------------------------------------------------------------------
/** @file DAQAlgBase.h
 *
 *  Header file for algorithm base class : DAQ::AlgBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERALGBASE_H
#define DAQKERNEL_DECODERALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //-----------------------------------------------------------------------------
  /** @class AlgBase DAQAlgBase.h DAQKernel/DecoderAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing
   *  some basic functionality.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   02/10/2014
   */
  //-----------------------------------------------------------------------------

  class AlgBase : public Decoder::CommonBase<GaudiAlgorithm>
  {

  public:

    /// Standard constructor
    AlgBase( const std::string& name,
             ISvcLocator* pSvcLocator )
      : Decoder::CommonBase<GaudiAlgorithm> ( name, pSvcLocator ) { }

  };

}

#endif // DAQKERNEL_DECODERALGBASE_H
