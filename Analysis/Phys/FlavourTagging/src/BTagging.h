// $Id: BTagging.h,v 1.1 2004-01-28 18:54:33 gcorti Exp $
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
#include "MCTools/IVisPrimVertTool.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/Vertex.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/L0DUReport.h"
#include "Event/L1Report.h"
// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Vector/LorentzVector.h"

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
  ParticleVector BTagging::FindDaughters( Particle* );

public: 
  /// Standard constructor
  BTagging( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~BTagging( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:
 
  //NTuples  
  NTuple::Item<long>      m_Run;
  NTuple::Item<long>      m_Event;
  NTuple::Item<long>      m_Tag;
  NTuple::Item<long>      m_TagCat;
  NTuple::Item<long>      m_trig0;
  NTuple::Item<long>      m_trig1;
  NTuple::Item<float>     m_B0the;
  NTuple::Item<float>     m_B0phi;
  NTuple::Item<float>     m_B0mass;
  NTuple::Item<long>      m_BID;
  NTuple::Item<float>     m_RVz;
  NTuple::Item<long>      m_K;
  NTuple::Item<long>      m_knrec;
  NTuple::Item<long>      m_kType;

  NTuple::Item<long>      m_N;
  NTuple::Array<long>      m_AXID;
  NTuple::Array<float>     m_AXP;
  NTuple::Array<float>     m_AXPt;
  NTuple::Array<float>     m_AXphi;
  NTuple::Array<long>      m_ch;
  NTuple::Array<float>     m_AXip;
  NTuple::Array<float>     m_AXiperr;
  NTuple::Array<float>     m_IPPU;
  NTuple::Array<long>      m_trtyp;
  NTuple::Array<float>     m_InvMss;
  NTuple::Array<float>     m_ThBp;

  IParticlePropertySvc* ppSvc;
  IVisPrimVertTool* m_visTool;

  Vertex* RecVert; 
  VertexVector PileUpVtx;

  //properties
  bool m_WriteNtpl;
  double m_AXPt_cut_muon;
  double m_AXP_cut_muon;
  double m_IP_cut_muon;

  double m_AXPt_cut_ele;
  double m_AXP_cut_ele;
  double m_IP_cut_ele ;

  double m_AXPt_cut_kaon;
  double m_AXP_cut_kaon ;
  double m_IP_cut_kaon ;

  double m_AXPt_cut_kaonS;
  double m_AXP_cut_kaonS;
  double m_IP_cut_kaonS;

  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dQcut_kaonS;

};
//===========================================================================//
#endif // USER_BTagging_H
