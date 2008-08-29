// $Id: CopyMCHeader.h,v 1.6 2008-08-29 07:08:56 jpalac Exp $
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
 *  // Add a sequencer
 *  ApplicationMgr.TopAlg += { "GaudiSequencer/MyStuffCopier" } ;
 *  MyStuffCopier.Members += {"CopyMCHeader"};
 *  CopyMCHeader.OutputPrefix = "MyLocation";
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

typedef MicroDST::ObjectClonerAlg<LHCb::MCHeader> CopyMCHeader;
// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( CopyMCHeader )

#endif // COPYMCHEADER_H
