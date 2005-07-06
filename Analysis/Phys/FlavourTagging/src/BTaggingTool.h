// $Id: BTaggingTool.h,v 1.7 2005-07-06 00:33:19 musy Exp $

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
#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"
#include "Event/TrgDecision.h"
#include "Event/HltScore.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IPhysDesktop.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h" 
#include "Kernel/ITagger.h" 
#include "ICombineTaggersTool.h" 

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
  StatusCode tag( FlavourTag& theTag, const Particle* );
  StatusCode tag( FlavourTag& theTag, const Particle*, const Vertex* );
  StatusCode tag( FlavourTag& theTag, const Particle*, const Vertex*, ParticleVector& );
  //-------------------------------------------------------------

private:
  bool isinTree( const Particle*, std::vector<const Particle*>& );
  StatusCode calcIP( const Particle*, const Vertex*, double&, double& );
  StatusCode calcIP( const Particle*, 
		     const VertexVector, double&, double& ) ;
  std::vector<const Particle*> toStdVector( const SmartRefVector<Particle>& );
  std::vector<const Particle*> FindDaughters( const Particle* );
  long trackType( const Particle* );

  ISecondaryVertexTool* m_svtool;
  IDataProviderSvc* m_eventSvc;
  IGeomDispCalculator *m_Geom;
  IPhysDesktop *m_physd;
  ITagger *m_taggerMu,*m_taggerEle,*m_taggerKaon,*m_taggerVtxCh;
  ITagger *m_taggerKaonS,*m_taggerPionS, *m_taggerVtx, *m_taggerJetS ;
  ICombineTaggersTool *m_combine;

  //properties ----------------
  double m_thetaMin;
  double m_IPPU_cut;

  std::string m_SecondaryVertexToolName;
  std::string m_outputLocation, m_CombineTaggersName;

  bool m_RequireL0, m_RequireL1, m_RequireHLT;
  bool m_EnableMuon,m_EnableElectron,m_EnableKaonOS;
  bool m_EnableKaonSS,m_EnablePionSS,m_EnableVertexCharge;
  bool m_EnableJetSame;
};

//=======================================================================//
#endif // USER_BTAGGINGTOOL_H
