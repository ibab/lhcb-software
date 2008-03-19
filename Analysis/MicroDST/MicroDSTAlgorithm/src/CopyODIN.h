// $Id: CopyODIN.h,v 1.4 2008-03-19 18:59:53 jpalac Exp $
#ifndef COPYODIN_H 
#define COPYODIN_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"

/** @class CopyODIN CopyODIN.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::ODIN from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::ODIN is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::ODIN::Location::Default
 *
 *  <b>Example</b>: Clone the LHCb::ODIN from default location ("DAQ/ODIN") 
 *  to "/Event/MyLocation/DAQ/ODIN"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyODIN"};
 *  CopyODIN.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-15
 */
class CopyODIN : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyODIN( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyODIN( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::ODIN> ODINCopy;
};
#endif // COPYODIN_H
