// $Id: CopyRecHeader.h,v 1.4 2008-03-19 18:59:53 jpalac Exp $
#ifndef COPYRECHEADER_H 
#define COPYRECHEADER_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyRecHeader CopyRecHeader.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::RecHeader from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::RecHeader is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::RecHeaderLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::RecHeader from default location ("Rec/Header") 
 *  to "/Event/MyLocation/Rec/Header"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyRecHeader"};
 *  CopyRecHeader.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-10-15
 */
class CopyRecHeader : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyRecHeader( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyRecHeader( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  typedef MicroDST::BasicCopy<LHCb::RecHeader> RecHeaderCopy;
};
#endif // COPYRECHEADER_H
