// $Id: CopyFlavourTag.h,v 1.2 2008-08-08 12:41:34 jpalac Exp $
#ifndef COPYFLAVOURTAG_H 
#define COPYFLAVOURTAG_H 1

// Include files
// from MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"

namespace MicroDST
{
  class ICloneFlavourTag;
}


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
 *  The cloning action os performed by an implementation of the 
 *  ICloneFlavourTag interface. This is steerable via the ICloneFlavourTag
 *  property, with default value FlavourTagCloner.
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

  ICloneFlavourTag* m_cloner;

  std::string m_tagClonerName;

};
#endif // COPYFLAVOURTAG_H
