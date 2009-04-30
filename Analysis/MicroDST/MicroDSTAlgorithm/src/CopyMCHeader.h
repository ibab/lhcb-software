// $Id: CopyMCHeader.h,v 1.9 2009-04-30 15:43:48 jpalac Exp $
#ifndef COPYMCHEADER_H 
#define COPYMCHEADER_H 1

// Include files
// from MicroDST
#include "MicroDST/ObjectClonerAlg.h"
// from LHCb
#include "Event/MCHeader.h"

/** @class CopyMCHeader CopyMCHeader.h
 *  
 *
 *  MicroDSTAlgorithm to clone an LHCb::MCHeader from one TES location to 
 *  another one. It inherits the std::string properties InputLocation and 
 *  OutputPrefix from MicroDSTCommon. The LHCb::MCHeader is taken from the 
 *  TES location defined by InputLocation, and is cloned and put in 
 *  TES location "/Event" + OutputPrefix + InputLocation. 
 *  If InputLocation already contains a leading "/Event" it is removed.
 *  If no InputLocation is specified the header is taken from LHCb::MCHeaderLocation::Default
 *
 *  The cloned LHCb::MCHeader's SmartRefVector<LHCb::MCVertex> is cleared 
 *  and the original LCHb::MCVertex* are added as Primary Vertices to the clone.
 *
 *  <b>Example</b>: Clone the LHCb::Header from default location ("MC/Header") 
 *  to "/Event/MyLocation/MC/Header"
 *  @code
 *  mySeq = GaudiSequencer("SomeSequence")
 *  copyMCHeader = CopyMCHeader()
 *  copyMCHeader.OutputPrefix = "MyLocation"
 *  mySeq.Members += [copyMCHeader]
 *  @endcode
 *
 * 
 *  @todo This code needs to be tested.
 *
 *  @author Juan PALACIOS juan.palacios@nikhef.nl
 *  @date   2007-11-02
 */

struct MCHeaderCopy 
{
  LHCb::MCHeader* operator() (const LHCb::MCHeader* mcHeader) 
  { 
    return copy(mcHeader); 
  }
    
  static LHCb::MCHeader* copy(const LHCb::MCHeader* mcHeader)
  {

    LHCb::MCHeader* newHeader = new LHCb::MCHeader(*mcHeader);
    
    newHeader->clearPrimaryVertices();
      
    const SmartRefVector<LHCb::MCVertex>& PVs = 
      mcHeader->primaryVertices();

    for (SmartRefVector<LHCb::MCVertex>::const_iterator iPV = PVs.begin();
         iPV != PVs.end();
         ++iPV) newHeader->addToPrimaryVertices(*iPV);

    return newHeader;
    
  }

};
//=============================================================================
template <> struct BindType2Cloner<LHCb::MCHeader> 
{
  typedef LHCb::MCHeader type;
  typedef MCHeaderCopy cloner;
};
//=============================================================================
template <> struct Location<LHCb::MCHeader> 
{
  const static std::string Default;
};
const std::string Location<LHCb::MCHeader>::Default = LHCb::MCHeaderLocation::Default;
//=============================================================================
typedef MicroDST::ObjectClonerAlg<LHCb::MCHeader> CopyMCHeader;
// Declaration of the Algorithm Factory
DECLARE_NAMED_ALGORITHM_FACTORY( CopyMCHeader, CopyMCHeader )

#endif // COPYMCHEADER_H
