// $Id: BTaggingTool.h,v 1.4 2005-07-05 12:40:11 pkoppenb Exp $

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
#include "Event/ProtoParticle.h"
#include "Kernel/IVertexFitter.h"
#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"
#include "Event/TrgDecision.h"
#include "Event/HltScore.h"
#include "Event/TamperingResults.h"
// from RecoTools
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IPhysDesktop.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "INNetTool.h"
#include "Kernel/IBTaggingTool.h" 
#include "Kernel/ITagger.h" 

/** @class BTaggingTool BTaggingTool.h 
 *
 *  Tool to tag the B flavour
 *
 *  @author Marco Musy
 *  @date   05/06/2005
 */

class IGeomDispCalculator;

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
  FlavourTag* tag( const Particle* );
  FlavourTag* tag( const Particle*, const Vertex* );
  FlavourTag* tag( const Particle*, const Vertex*, ParticleVector& );
  //-------------------------------------------------------------

private:
  bool isinTree( const Particle*, std::vector<const Particle*>& );
  StatusCode calcIP( const Particle*, const Vertex*, double&, double& );
  StatusCode calcIP( const Particle*, 
		     const VertexVector, double&, double& ) ;
  std::vector<const Particle*> toStdVector( const SmartRefVector<Particle>& );
  std::vector<const Particle*> FindDaughters( const Particle* );
  long trackType( const Particle* );
  double pol2(double, double, double );
  double pol3(double, double, double, double );
  double pol4(double, double, double, double, double );

  std::string m_SecondaryVertexToolName, m_CombinationTechnique;

  ISecondaryVertexTool* m_svtool;
  INNetTool* m_nnet;
  IDataProviderSvc* m_eventSvc;
  IGeomDispCalculator *m_Geom;

  IPhysDesktop *m_physd;
  ITagger *m_taggerMu,*m_taggerEle,*m_taggerKaon;
  ITagger *m_taggerKaonS,*m_taggerPionS, *m_taggerVtx, *m_taggerJetS ;

  //properties ----------------
  double m_thetaMin;
  double m_IPPU_cut;

  double m_ProbMin;
  double m_VchOmega;

  bool m_RequireL0, m_RequireL1, m_RequireHLT;
  bool m_RequireTrigger, m_RequireL1Tamp;
  bool m_EnableMuonTagger,m_EnableElectronTagger,m_EnableKaonOSTagger;
  bool m_EnableKaonSSTagger,m_EnablePionTagger,m_EnableVertexChargeTagger;
  bool m_EnableJetSameTagger;
  std::string m_outputLocation ;
  
};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
