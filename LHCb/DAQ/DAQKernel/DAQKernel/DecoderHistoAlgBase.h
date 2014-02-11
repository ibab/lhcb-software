
//-----------------------------------------------------------------------------
/** @file DecoderHistoAlgBase.h
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
  /** @class HistoAlgBase DAQHistoAlgBase.h DAQKernel/DecoderHistoAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming
   *  functionality from the base class GaudiHistoAlg.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   10/02/2014
   */
  //-----------------------------------------------------------------------------

  class HistoAlgBase : public Decoder::CommonBase<GaudiHistoAlg>
  {

  public:

    /// Standard constructor
    HistoAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Decoder::CommonBase<GaudiHistoAlg> ( name, pSvcLocator )
    { }

  protected:
    ///avoid getIfExists name resolution for now
    virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes) const
    {
    	return GaudiHistoAlg::getIfExists<LHCb::RawEvent>(loc,rootOnTes);
    }
  };

}

#endif // DAQKERNEL_DECODERHISTOALGBASE_H
