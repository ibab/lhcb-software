// $Id: BTagging.h,v 1.5 2004-07-28 14:18:05 pkoppenb Exp $
#ifndef USER_BTagging_H 
#define USER_BTagging_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/GenericAddress.h"
#include "DaVinciTools/DVAlgorithm.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/L0DUReport.h"
//#include "Event/L1Report.h"
#include "Event/FlavourTag.h"
#include "Event/GenMCLink.h"
// from RecoTools
#include "RecoTools/ITrVeloCharge.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

#define MAXSIZETAGS 100

/** @class BTagging BTagging.h 
 *  
 *  @author Marco Musy
 *  @date   21/01/2004
 */


// Forward declarations
class DataObject;
class IDebugTool;
class IParticlePropertySvc;

class BTagging : public DVAlgorithm {

  bool isinTree( Particle*, ParticleVector& );
  void printInfo( Particle* );
  void PileUpIP( Particle*, double&, double& ) ;
  std::vector<Particle*> toStdVector( SmartRefVector<Particle>& );
  ParticleVector FindDaughters( Particle* );
  double VertexCharge( ParticleVector, int& );

public: 
  /// Standard constructor
  BTagging( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTagging( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  //tagging variables
  long      m_Run;
  long      m_Event;
  long      m_trig0;
  long      m_trig1;
  float     m_B0the;
  float     m_B0phi;
  float     m_B0mass;
  long      m_BID;
  float     m_RVz;
  long      m_knrec;

  long      m_N;
  long      m_AXID[MAXSIZETAGS];
  float     m_AXP[MAXSIZETAGS];
  float     m_AXPt[MAXSIZETAGS];
  float     m_AXphi[MAXSIZETAGS];
  long      m_ch[MAXSIZETAGS];
  float     m_AXip[MAXSIZETAGS];
  float     m_AXiperr[MAXSIZETAGS];
  float     m_IPPU[MAXSIZETAGS];
  long      m_trtyp[MAXSIZETAGS];
  float     m_InvMss[MAXSIZETAGS];
  float     m_ThBp[MAXSIZETAGS];
  float     m_veloch[MAXSIZETAGS];
  float     m_Emeas[MAXSIZETAGS];

  Hep3Vector ipVec;
  HepSymMatrix errMatrix;
  IParticlePropertySvc* ppSvc;
  Vertex* RecVert; 
  VertexVector PileUpVtx;

  int nsele;
  int nrt[50];
  int nwt[50];

  //properties ----------------
  bool m_UseVertexCharge, m_WriteToTES;

  ///<  Name of TrVeloCharge Tool
  std::string    m_veloChargeName;
  ///<  TrVeloCharge Tool
  ITrVeloCharge* m_veloCharge;

  double m_AXPt_cut_muon;
  double m_AXP_cut_muon;
  double m_IP_cut_muon;

  double m_AXPt_cut_ele;
  double m_AXP_cut_ele;
  double m_IP_cut_ele ;

  double m_AXPt_cut_kaon;
  double m_AXP_cut_kaon ;
  double m_IP_cut_kaon ;
  double m_IPPU_cut_kaon;

  double m_AXPt_cut_kaonS;
  double m_AXP_cut_kaonS;
  double m_IP_cut_kaonS;
  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dQcut_kaonS;

  double m_AXPt_cut_pionS;
  double m_AXP_cut_pionS;
  double m_IP_cut_pionS;
  double m_dQcut_pionS;
  double m_dQ2cut_pionS;

};
//===========================================================================//
#endif // USER_BTagging_H
