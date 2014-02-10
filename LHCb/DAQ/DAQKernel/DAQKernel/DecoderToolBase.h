
//-----------------------------------------------------------------------------
/** @file DAQToolBase.h
 *
 *  Header file for tool base class : DAQ::ToolBase
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQTOOLBASE_H
#define DAQKERNEL_DAQTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTool.h"

// local
#include "DAQKernel/DAQCommonBase.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class ToolBase DAQToolBase.h DAQKernel/DAQToolBase.h
   *
   *  Abstract base class for DAQ tools providing
   *  some basic functionality.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class ToolBase : public DAQ::CommonBase<GaudiTool>
  {

  public:

    /// Standard constructor
    ToolBase( const std::string& type,
              const std::string& name,
              const IInterface* parent )
      : DAQ::CommonBase<GaudiTool> ( type, name, parent ) { }

  };

}

#endif // DAQKERNEL_DAQTOOLBASE_H
