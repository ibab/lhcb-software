// $Id: CopyFlavourTag.h,v 1.1 2008-04-16 10:25:13 jpalac Exp $
#ifndef COPYFLAVOURTAG_H 
#define COPYFLAVOURTAG_H 1

// Include files
// from Gaudi
#include "MicroDST/MicroDSTAlgorithm.h"


/** @class CopyFlavourTag CopyFlavourTag.h
 *  
 *  MicroDSTAlgorithm to clone an LHCb::FlavourTag container from one TES 
 *  location to another one. 
 *  It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::FlavourTags are taken from the 
 *  TES location defined by InputLocation, and are cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the FlavourTags are taken from 
 *  LHCb::FlavourTagLocation::Default
 *
 *  <b>Example</b>: Clone the LHCb::FlavourTags from default location ("Phys/Tagging") 
 *  to "/Event/MyLocation/Phys/Tagging"
 *  @code
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyFlavourTag"};
 *  CopyFlavourTag.OutputPrefix = "MyLocation";
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
class CopyFlavourTag : public MicroDSTAlgorithm {
public: 
  /// Standard constructor
  CopyFlavourTag( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CopyFlavourTag( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
  typedef MicroDST::BasicCopy<LHCb::FlavourTag> FlavourTagCopy;
  typedef MicroDST::CloneKeyedContainerItem<LHCb::FlavourTag, FlavourTagCopy> FlavourTagKeyedItemCopy;
};
#endif // COPYFLAVOURTAG_H
