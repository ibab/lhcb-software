
//-----------------------------------------------------------------------------
/** @file DecoderTupleAlgBase.h
 *
 *  Header file for algorithm base class : DecoderTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: DecoderTupleAlgBase.h,v 1.7 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   02/10/2014
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERTUPLEALGBASE_H
#define DAQKERNEL_DECODERTUPLEALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //-----------------------------------------------------------------------------
  /** @class TupleAlgBase DecoderTupleAlgBase.h DAQKernel/DecoderTupleAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleAlg.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   10/02/2014
   */
  //-----------------------------------------------------------------------------

  class TupleAlgBase : public Decoder::CommonBase<GaudiTupleAlg>
  {

  public:

    /// Standard constructor
    TupleAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : Decoder::CommonBase<GaudiTupleAlg> ( name, pSvcLocator )
    { }

  protected:
      ///avoid getIfExists name resolution for now
      virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes) const
      {
      	return GaudiTupleAlg::getIfExists<LHCb::RawEvent>(loc,rootOnTes);
      }
  };

}


#endif // DAQKERNEL_DECODERTUPLEALGBASE_H
