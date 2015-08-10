// $Id: RelInfoBs2MuMuBIsolations.h 
#ifndef RELINFOBS2MUMUBISOLATIONS_H 
#define RELINFOBS2MUMUBISOLATIONS_H 1

#include "GaudiKernel/ToolFactory.h"
#include "Kernel/RelatedInfoNamed.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Event/RelatedInfoMap.h"
#include "GaudiAlg/GaudiTool.h"
#include "CaloUtils/CaloParticle.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include "Kernel/IVertexFit.h"
#include "Kernel/IPVReFitter.h"
#include "Event/RecVertex.h"


/** @class RelInfoBs2MuMuBIsolations RelInfoBs2MuMuBIsolations.h

 * 
 *  @author Fatima Soomro
 *  @date   2014-08-08
 */

class IDistanceCalculator;
class IDVAlgorithm;
class IPVReFitter;

class RelInfoBs2MuMuBIsolations : public GaudiTool, virtual public IRelatedInfoTool{
 public: 
  /// Standard constructor
  RelInfoBs2MuMuBIsolations( const std::string& type, 
              const std::string& name,
              const IInterface* parent );

  virtual StatusCode initialize();    ///< Algorithm initialization

  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*, const LHCb::Particle*);
  
  virtual LHCb::RelatedInfoMap* getInfo(void);

  virtual std::string infoPath(void);

  virtual ~RelInfoBs2MuMuBIsolations( ); ///< Destructor


private:

  bool isTrackInDecay(const LHCb::Track* track);
  void saveDecayParticles( const LHCb::Particle *top);

  StatusCode CDFIsolation(const LHCb::Particle* , 
			  const LHCb::Particle* );
  
  StatusCode OtherB(const LHCb::Particle* ,
		    const LHCb::Particle* );

  
  
  LHCb::RelatedInfoMap m_map;
  std::vector<std::string> m_variables;
  std::vector<short int> m_keys;
  std::vector<const LHCb::Particle*> m_decayParticles;
  LHCb::RecVertex::Container * m_vertices ;
  
  std::string m_TracksPath;
  std::string m_ParticlePath;
  std::string m_PVInputLocation;	

  int m_tracktype;

  double m_ghost_cut;
  double m_trchi2_cut;
  bool m_makeTrackCuts;
  bool m_IsoTwoBody;

  // //// Variables that are calculated and stored 
 
  double m_CDFIso;
  double m_otherB_mag;
  double m_otherB_angle;
  double m_otherB_boost_mag;
  double m_otherB_boost_angle;
  int m_otherBtracks;
  int m_partID;
  int m_topID;

    /// return the arcosine between two vectors
  double arcosine (Gaudi::XYZVector, Gaudi::XYZVector);

  
};



#endif // RELINFOBS2MUMUBISOLATIONS_H
