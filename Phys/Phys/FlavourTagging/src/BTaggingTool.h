// $Id: BTaggingTool.h,v 1.24 2010-03-05 15:54:18 jpalac Exp $
#ifndef USER_BTAGGINGTOOL_H
#define USER_BTAGGINGTOOL_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
#include <algorithm>
#include <cstdio>

// Boost
#include <boost/foreach.hpp>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"

// from Event
#include "Event/FlavourTag.h"
#include "Event/RecHeader.h"
#include "Event/Track.h"

// Kernels
#include "Kernel/ITagger.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IRelatedPVFinder.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>

// local
#include "ICombineTaggersTool.h"
#include "FlavourTagging/ITaggingUtils.h"
#include "MultiplePersonalityCall.h"
#include "TaggingHelpers.h"

//class DaVinciAlgorithm;

/** @class BTaggingTool BTaggingTool.h
 *
 *  Tool to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class BTaggingTool : public GaudiTool,
                     virtual public IBTaggingTool
{

public:

  /// Standard constructor
  BTaggingTool( const std::string& type,
                const std::string& name,
                const IInterface* parent );

  virtual ~BTaggingTool( ); ///< Destructor

  StatusCode initialize();  ///< initialization

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

  MultiplePersonalityCall<boost::function<
                            const LHCb::Particle::ConstVector(const LHCb::Particle* /* AXB */,
                                                              const LHCb::Particle::Range& /* parts */,
                                                              const LHCb::RecVertex::ConstVector& /* PileUpVtx*/) > >
  m_chooseCandidates;

  const LHCb::Particle::ConstVector
  chooseCandidates(const LHCb::Particle* AXB,
                   const LHCb::Particle::Range& parts,
                   const LHCb::RecVertex::ConstVector& PileUpVtx);
  const LHCb::Particle::ConstVector
  chooseCandidatesReco14(const LHCb::Particle* AXB,
                         const LHCb::Particle::Range& parts,
                         const LHCb::RecVertex::ConstVector& PileUpVtx);
  const LHCb::Particle::ConstVector
  chooseCandidatesReco12(const LHCb::Particle* AXB,
                         const LHCb::Particle::Range& parts,
                         const LHCb::RecVertex::ConstVector& PileUpVtx);

  void clearExtraInfo();

private:

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend; // had to break const correctness

  ITagger *m_taggerMu,    *m_taggerEle,   *m_taggerKaon,     *m_taggerVtxCh,     *m_taggerJetS;
  ITagger *m_taggerKaonS, *m_taggerPionS, *m_taggerNNetKaon, *m_taggerNNetKaonS, *m_taggerProtonS, *m_taggerPionBDTS;
  ITagger *m_taggerCharm;
  ICombineTaggersTool *m_combine;
  const IPVReFitter* m_pvReFitter;

  IDVAlgorithm* m_dva;

  //properties ----------------
  double m_thetaMin, m_distphi_cut;
  double m_IPPU_cut, m_ghostprob_cut;

  std::string m_taggerLocation, m_CombineTaggersName, m_ChoosePV;
  std::string m_ForceSignalID;

  bool m_EnableMuon,m_EnableElectron,m_EnableKaonOS;
  bool m_EnableKaonSS,m_EnablePionSS,m_EnableVertexCharge,m_EnablePionBDTSS;
  bool m_EnableJetSame,m_EnableNNetKaonOS,m_EnableNNetKaonSS;
  bool m_EnableProtonSS;
  bool m_EnableCharm;
  
  std::string m_PVReFitterAlgName;
  bool m_UseReFitPV, m_VetoFailedRefits; 
  bool m_CombineWithNNetTagger, m_CombineWithCharmTagger;
  std::string m_personality;

  std::vector<LHCb::Particle*> m_extraInfoToClear; ///< Particles to clear extra info from

};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
