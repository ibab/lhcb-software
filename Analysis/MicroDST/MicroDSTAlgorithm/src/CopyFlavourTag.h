// $Id: CopyFlavourTag.h,v 1.8 2010-08-11 13:52:27 jpalac Exp $
#ifndef COPYFLAVOURTAG_H 
#define COPYFLAVOURTAG_H 1

// Include files
// from LHCb
#include <Event/FlavourTag.h>
// from MicroDST
#include "MicroDST/KeyedContainerClonerAlg.h"
#include <MicroDST/ICloneFlavourTag.h>
#include "MicroDST/BindType2Cloner.h"

// local
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
 *  ICloneFlavourTag interface. This is steerable via the ClonerType
 *  property, with default value FlavourTagCloner.
 *
 *  <b>Example</b>: Clone the LHCb::FlavourTags from default location ("Phys/Tagging") 
 *  to "/Event/MyLocation/Phys/Tagging"
 *  @code
 *  # grab a sequencer
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyFT = CopyFlavourTag()
 *  mySeq.Members += [copyFT]
 *  copyFT.OutputPrefix = "MyLocation"
 *  @endcode
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2008-04-16
 */
//=============================================================================
template <> struct BindType2Cloner<LHCb::FlavourTag> 
{
  typedef LHCb::FlavourTag Type;
  typedef ICloneFlavourTag Cloner;
};
//=============================================================================
template<> struct Defaults<LHCb::FlavourTag>
{
  const static std::string clonerType;
};
const std::string Defaults<LHCb::FlavourTag>::clonerType = "FlavourTagDeepCloner";
//=============================================================================
template<> struct Location<LHCb::FlavourTag>
{
  const static std::string Default;
};
const std::string Location<LHCb::FlavourTag>::Default = LHCb::FlavourTagLocation::Default;
//=============================================================================
typedef MicroDST::KeyedContainerClonerAlg<LHCb::FlavourTag> CopyFlavourTag;
DECLARE_NAMED_ALGORITHM_FACTORY( CopyFlavourTag, CopyFlavourTag )
#endif // COPYFLAVOURTAG_H
