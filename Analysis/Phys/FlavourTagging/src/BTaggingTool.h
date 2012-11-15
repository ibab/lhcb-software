// $Id: BTaggingTool.h,v 1.24 2010-03-05 15:54:18 jpalac Exp $
#ifndef USER_BTAGGINGTOOL_H 
#define USER_BTAGGINGTOOL_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event 
#include "Event/FlavourTag.h" 
#include "Event/RecHeader.h"

#include "ICombineTaggersTool.h" 
#include "Kernel/ITagger.h" 
#include "Kernel/IBTaggingTool.h" 
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IRelatedPVFinder.h"

#include "ITaggingUtils.h"
//
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
class DaVinciAlgorithm;

/** @class BTaggingTool BTaggingTool.h 
 *
 *  Tool to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class BTaggingTool : public GaudiTool,
                     virtual public IBTaggingTool {

public: 

  /// Standard constructor
  BTaggingTool( const std::string& type,
                const std::string& name,
                const IInterface* parent );
  virtual ~BTaggingTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  StatusCode tag( LHCb::FlavourTag& theTag, const LHCb::Particle* );

  StatusCode tag( LHCb::FlavourTag& theTag, 
                  const LHCb::Particle*, const LHCb::RecVertex* );

  StatusCode tag( LHCb::FlavourTag& theTag, 
                  const LHCb::Particle*, const LHCb::RecVertex*, 
                  LHCb::Particle::ConstVector& ); 

private:

  const LHCb::RecVertex::ConstVector
  choosePrimary(const LHCb::Particle* AXB,
                const LHCb::RecVertex::Range& verts, 
                const LHCb::RecVertex*& RecVert,
                LHCb::RecVertex& RefitRecVert);

  const LHCb::Particle::ConstVector 
  chooseCandidates(const LHCb::Particle* AXB,
                    LHCb::Particle::Range& parts,
                   const LHCb::RecVertex::ConstVector& PileUpVtx);

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend;

  ITagger *m_taggerMu, *m_taggerEle, *m_taggerKaon, *m_taggerVtxCh;
  ITagger *m_taggerKaonS, *m_taggerPionS, *m_taggerVtx, *m_taggerJetS;
  ICombineTaggersTool *m_combine;
  IPVReFitter* m_pvReFitter;

  IDVAlgorithm* m_dva;

  //  IRelatedPVFinder* m_pFinder;

  //properties ----------------
  double m_thetaMin, m_distphi_cut;
  double m_IPPU_cut;

  std::string m_taggerLocation, m_CombineTaggersName, m_ChoosePV;
  std::string m_ForceSignalID;

  bool m_EnableMuon,m_EnableElectron,m_EnableKaonOS;
  bool m_EnableKaonSS,m_EnablePionSS,m_EnableVertexCharge;
  bool m_EnableJetSame;

  bool m_UseVtxOnlyWithoutOS, m_UseReFitPV;

};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
