
//-----------------------------------------------------------------------------
/** @file DAQHistoToolBase.h
 *
 *  Header file for tool base class : DAQHistoToolBase
 *
 *  CVS Log :-
 *  $Id: DAQHistoToolBase.h,v 1.6 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQHISTOTOOLBASE_H
#define DAQKERNEL_DAQHISTOTOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiHistoTool.h"

// local
#include "DAQKernel/DAQHistoBase.h"

// histogramming utilities
#include "DAQKernel/DAQHistoID.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class HistoToolBase DAQHistoToolBase.h DAQKernel/DAQHistoToolBase.h
   *
   *  Abstract base class for DAQ tools providing some basic functionality.
   *
   *  In addition, uses the histogramming
   *  functionality from the base class GaudiTupleAlg.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class HistoToolBase : public DAQ::HistoBase<GaudiHistoTool>
  {

  public:

    /// Standard constructor
    HistoToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : DAQ::HistoBase<GaudiHistoTool> ( type, name, parent )
    { }

  };

}

#endif // DAQKERNEL_DAQHISTOTOOLBASE_H
