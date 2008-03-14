// $Id: TaggingTuplesData.h,v 1.1 2008-03-14 17:02:16 pkoppenb Exp $
#ifndef TAGGINGTUPLESDATA_H 
#define TAGGINGTUPLESDATA_H 1

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
#include "Event/RecVertex.h"
#include "Event/MCParticle.h"
#include "MCInterfaces/IForcedBDecayTool.h"
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"
#include "Event/RecHeader.h"
#include "Kernel/IBackgroundCategory.h"

/** @class PlotTuplesbkg PlotTuplesbkg.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2005-11-07
 */
class TaggingTuplesData : public DVAlgorithm {
public: 
  /// Standard constructor
  TaggingTuplesData( const std::string& name , ISvcLocator* pSvcLocator );

  virtual ~TaggingTuplesData( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_tagslocation;
    IPrintDecayTreeTool* m_debugParticle;
    IPrintMCDecayTreeTool* m_debugMCParticle;
  // LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* ) ;
  int nSelected,nrt[100],nwt[100];  
 IBackgroundCategory* m_back;
  
  NTuple::Item<long>      osmuondec;
  NTuple::Item<long>      oselectrondec;
  NTuple::Item<long>      oskaondec;
  NTuple::Item<long>      sskaondec;
  NTuple::Item<long>      vtxchargedec;
  NTuple::Item<long>      sspiondec;
  NTuple::Item<long>      ssjetdec;
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
  NTuple::Item<long>      trigHLT;
  NTuple::Item<long>       m_bkgtooltag;
  NTuple::Item<long>       m_background;
  NTuple::Item<long>       m_bkgtooltagid;
  NTuple::Item<float>      bkgtoolIDmass;
  //NTuple::Item<float>      BsP;
  //NTuple::Item<float>      BsPt;
  //NTuple::Item<float>       BsChi2;
  //NTuple::Item<float>      DsP;
  //NTuple::Item<float>      DsPt;
  //NTuple::Item<float>      DsChi2;
  };
#endif // TAGGINGTUPLESDATA_H
