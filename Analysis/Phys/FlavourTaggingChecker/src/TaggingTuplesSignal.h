// $Id: TaggingTuplesSignal.h,v 1.2 2008-03-14 17:41:30 pkoppenb Exp $
#ifndef TAGGINGTUPLESSIGNAL_H 
#define TAGGINGTUPLESSIGNAL_H 1

// Include files
// Include files
#include "GaudiKernel/AlgFactory.h"
#include "Event/FlavourTag.h"
#include "GaudiKernel/INTupleSvc.h"
#include "Kernel/IPrintDecayTreeTool.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"
#include <iomanip>
#include "Event/GenCollision.h"
#include "Event/GenHeader.h"
#include "Kernel/IEvtTypeSvc.h"
#include "MCInterfaces/IMCDecayFinder.h"
#include "MCInterfaces/IForcedBDecayTool.h"
#include "Event/RecVertex.h"
#include "Event/MCParticle.h"
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Event/RecHeader.h"

/** @class TaggingTuplesSignal TaggingTuplesSignal.h
 *  
 * Implementation file for class :TaggingTuplesSignal
 *
 * @author  Onuora Awunor
 * @date    01-10-2007 
 *
 * To be run over MC truth Signal events.
 * Algorithm called by ../options/DoubleTAggingAnalysis.opts to store tagging 
 * decisions into NTUPLE to be used for DoubleTagging analysis
 * NB: The ntuple is used as input in macro .../root/DoubleTaggingAnalysis.C
 *
 *  @author Onuora Awunor
 *  @date   01-10-2008
 **/
class TaggingTuplesSignal : public DVAlgorithm {
public: 
  /// Standard constructor
  TaggingTuplesSignal( const std::string& name , ISvcLocator* pSvcLocator );

  virtual ~TaggingTuplesSignal( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_tagslocation;
  IPrintDecayTreeTool* m_debugParticle;
  IPrintMCDecayTreeTool* m_debugMCParticle;

  IForcedBDecayTool* m_forcedBtool;
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;

  int nSelected,nrt[100],nwt[100];  


  NTuple::Item<long>      osmuondec;
  NTuple::Item<long>      oselectrondec;
  NTuple::Item<long>      oskaondec;
  NTuple::Item<long>      sskaondec;
  NTuple::Item<long>      vtxchargedec;
  NTuple::Item<long>      sspiondec;
  NTuple::Item<long>      ssjetdec;
  NTuple::Item<long>      Truetag;
  NTuple::Item<long>      EvtNu;
  NTuple::Item<long>      RunNu;
  NTuple::Item<long>      Event;
  NTuple::Item<long>      BsidTag;
  NTuple::Item<long>      MCBsid;
  NTuple::Item<float>      MCBsmass;
  NTuple::Item<long>      Bsid;
  NTuple::Item<float>      Bsmass;
  NTuple::Item<long>      Muid;
  NTuple::Item<float>      Mumass;
  NTuple::Item<long>      Dsid;
  NTuple::Item<float>      Dsmass;
  NTuple::Item<long>      trig0;
  NTuple::Item<long>      hlt0;
  // NTuple::Item<float>      BsP;
  // NTuple::Item<float>      BsPt;
  // NTuple::Item<float>       BsChi2;
  // NTuple::Item<float>      DsP;
  // NTuple::Item<float>      DsPt;
  // NTuple::Item<float>      DsChi2;
  };
#endif // TAGGINGTUPLESSIGNAL_H

