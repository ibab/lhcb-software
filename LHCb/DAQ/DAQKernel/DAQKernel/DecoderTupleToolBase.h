
//-----------------------------------------------------------------------------
/** @file DecoderTupleToolBase.h
 *
 *  Header file for tool base class : DecoderTupleToolBase
 *
 *  CVS Log :-
 *  $Id: DecoderTupleToolBase.h,v 1.7 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date  10/02/2014
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERTUPLETOOLBASE_H
#define DAQKERNEL_DECODERTUPLETOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //-----------------------------------------------------------------------------
  /** @class TupleToolBase DecoderTupleToolBase.h DAQKernel/DecoderTupleToolBase.h
   *
   *  Abstract base class for DAQ tools providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleTool.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   02/10/2014
   */
  //-----------------------------------------------------------------------------

  class TupleToolBase : public Decoder::CommonBase<GaudiTupleTool>
  {

  public:

    /// Standard constructor
    TupleToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : Decoder::CommonBase<GaudiTupleTool> ( type, name, parent )
    { }

  protected:
      ///avoid getIfExists name resolution for now
      virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes) const
      {
      	return GaudiTupleTool::getIfExists<LHCb::RawEvent>(loc,rootOnTes);
      }
  };

}

#endif // DAQKERNEL_DECODERTUPLETOOLBASE_H
