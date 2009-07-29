#ifndef Hlt2DisplVerticesDEV_H
#define Hlt2DisplVerticesDEV_H 1
// Include files:
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/VectorMap.h"

// from Event
#include "Event/Track.h"
#include "Event/State.h"
#include "Event/RecVertex.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

/** @class Hlt2DisplVerticesDEV Hlt2DisplVerticesDEV.h
 *
 * THIS IS A DEVELOPMENT VERSION
 * please refer to Hlt2DisplVertices
 *
 *  @author Neal Gauvin (Gueissaz)
 *  @date   2009-july-1
 */

class Hlt2DisplVerticesDEV : public DVAlgorithm {
public:
  Hlt2DisplVerticesDEV(const std::string& name, ISvcLocator* pSvcLocator);
  // Destructor
  virtual ~Hlt2DisplVerticesDEV();
  // Algorithm initialization
  virtual StatusCode initialize();
  // Algorithm execution
  virtual StatusCode execute();
  // Algorithm finalization
  virtual StatusCode finalize();

private:
  StatusCode fillHeader( Tuple & );
  StatusCode Done( LHCb::Particle::ConstVector & );
  void GetPartsFromRecVtx(const LHCb::RecVertex*, 
			  LHCb::Particle::ConstVector & );
  void CreateMap();
  void Kinematics( LHCb::Particle::ConstVector &,double &,double &,double & );
  bool HasBackwardTracks( const LHCb::RecVertex* RV );
  bool RemVtxFromDet( const LHCb::RecVertex* );
  const LHCb::Particle* DefaultParticle( const LHCb::Track * p );
  Gaudi::XYZPoint Plus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  Gaudi::XYZPoint Minus( const Gaudi::XYZPoint &, const Gaudi::XYZPoint & );
  static bool sortPVz(const LHCb::RecVertex*, const LHCb::RecVertex* );

  //Check up
  void PrintTracksType();
  void PrintParticle();

  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  //Job options
  std::string m_InputDisplVertices;
  int m_MinNbtrks;     //min number of daughters tracks in a RecVertex
  double m_RMin;          //min radial displacement 
  double m_MinMass1;       //min reconstructed mass
  double m_MinMass2;       //min reconstructed mass
  double m_MinSumpt;      //min sum of all daughters track
  double m_RemVtxFromDet ;    // remove vtx reco in detector material
  // if = 0  : disabled
  // if < 0  : remove reco vtx if in detector material
  // if > 0  : remove reco vtx if rad length from decay pos - m_RemVtxFromDet 
  //           to decay pos + m_RemVtxFromDet along z is > threshold

  GaudiUtils::VectorMap<int, const LHCb::Particle *> m_map;

  int m_nbevent;
  int m_nbpassed;

  bool m_Save;
  bool m_DefMom;

  //a pions
  const LHCb::Particle* Pion;
  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions

};
#endif // Hlt2DisplVerticesDEV_H
