
//-----------------------------------------------------------------------------
/** @file DAQTupleAlgBase.h
 *
 *  Header file for algorithm base class : DAQTupleAlgBase
 *
 *  CVS Log :-
 *  $Id: DAQTupleAlgBase.h,v 1.7 2009-07-27 20:29:01 jonrob Exp $
 *
 *  @author Rob Lambert   Rob.Lambert@cern.ch
 *  @date   05/04/2002
 */
//-----------------------------------------------------------------------------

#ifndef DAQKERNEL_DAQTUPLEALGBASE_H
#define DAQKERNEL_DAQTUPLEALGBASE_H 1

// GaudiAlg
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "DAQKernel/DAQHistoBase.h"

// histogramming utilities
#include "DAQKernel/DAQHistoID.h"

namespace DAQ
{

  //-----------------------------------------------------------------------------
  /** @class TupleAlgBase DAQTupleAlgBase.h DAQKernel/DAQTupleAlgBase.h
   *
   *  Abstract base class for DAQ algorithms providing some basic functionality.
   *
   *  In addition, uses the histogramming and ntupling
   *  functionality from the base class GaudiTupleAlg.
   *
   *  @author Rob Lambert   Rob.Lambert@cern.ch
   *  @date   05/04/2002
   */
  //-----------------------------------------------------------------------------

  class TupleAlgBase : public DAQ::HistoBase<GaudiTupleAlg>
  {

  public:

    /// Standard constructor
    TupleAlgBase( const std::string& name,
                  ISvcLocator* pSvcLocator )
      : DAQ::HistoBase<GaudiTupleAlg> ( name, pSvcLocator )
    { }

  };

}


#endif // DAQKERNEL_DAQTUPLEALGBASE_H
