
//-----------------------------------------------------------------------------
/** @file DAQTupleToolBase.h
 *
 *  Header file for tool base class : DAQTupleToolBase
 *
 *  CVS Log :-
 *  $Id: DAQTupleToolBase.h,v 1.7 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQTUPLETOOLBASE_H
#define DAQKERNEL_DAQTUPLETOOLBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "DAQKernel/DAQHistoBase.h"

// histogramming utilities
#include "DAQKernel/DAQHistoID.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class TupleToolBase DAQTupleToolBase.h DAQKernel/DAQTupleToolBase.h
   *
   *  Abstract base class for DAQ tools providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleTool.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class TupleToolBase : public DAQ::HistoBase<GaudiTupleTool>
  {

  public:

    /// Standard constructor
    TupleToolBase( const std::string& type,
                   const std::string& name,
                   const IInterface* parent )
      : DAQ::HistoBase<GaudiTupleTool> ( type, name, parent )
    { }

  };

}

#endif // DAQKERNEL_DAQTUPLETOOLBASE_H
