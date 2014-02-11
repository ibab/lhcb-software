
//-----------------------------------------------------------------------------
/** @file DecoderToolBase.h
 *
 *  Header file for tool base class : Decoder::ToolBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   10/02/2014
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DECODERTOOLBASE_H
#define DAQKERNEL_DECODERTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTool.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //-----------------------------------------------------------------------------
  /** @class ToolBase DAQToolBase.h DAQKernel/DecoderToolBase.h
   *
   *  Abstract base class for DAQ tools providing
   *  some basic functionality.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class ToolBase : public Decoder::CommonBase<GaudiTool>
  {

  public:

    /// Standard constructor
    ToolBase( const std::string& type,
              const std::string& name,
              const IInterface* parent )
      : Decoder::CommonBase<GaudiTool> ( type, name, parent ) { }

  protected:
      ///avoid getIfExists name resolution for now
      virtual inline LHCb::RawEvent* getIfExistsRaw(const std::string loc, bool rootOnTes) const
      {
      	return GaudiTool::getIfExists<LHCb::RawEvent>(loc,rootOnTes);
      }
  };

}

#endif // DAQKERNEL_DECODERTOOLBASE_H
