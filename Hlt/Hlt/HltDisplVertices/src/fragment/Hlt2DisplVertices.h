#ifndef Hlt2DisplVertices_H
#define Hlt2DisplVertices_H 1
// Include files:
// from Gaudi
#include "Kernel/DVAlgorithm.h"
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/VectorMap.h"

// Detector description
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/IDetectorElement.h"
#include "DetDesc/ILVolume.h"
#include "DetDesc/IPVolume.h"
#include "DetDesc/Material.h"
#include "DetDesc/ITransportSvc.h"

/** @class Hlt2DisplVertices Hlt2DisplVertices.h
 *
 *  @author Neal Gauvin 
 *  @date   2009-july-16
 */

class Hlt2DisplVertices : public DVAlgorithm {
public:
  Hlt2DisplVertices(const std::string& name, ISvcLocator* pSvcLocator);
  /// Destructor
  virtual ~Hlt2DisplVertices();
  /// Algorithm initialization
  virtual StatusCode initialize();
  /// Algorithm execution
  virtual StatusCode execute();
  /// Algorithm finalization
  virtual StatusCode finalize();

private:
  /// Get all Particles related to the Tracks participating in a RecVertex
  void GetPartsFromRecVtx(const LHCb::RecVertex*, 
			  LHCb::Particle::ConstVector & );
  /// Create a map between the Particles and the Velo Tracks
  void CreateMap();
  /// Compute some kinematical values for a ConstVector of Particles
  void Kinematics( LHCb::Particle::ConstVector &,double &,double & );
  /// Has a RecVertex a backward track ?
  bool HasBackwardTracks( const LHCb::RecVertex* RV );
  /// Returns true if a RecVertex is found to be too close to a detector material
  bool RemVtxFromDet( const LHCb::RecVertex* );
  /// Creates a pions with 400 MeV pt from track slopes.
  const LHCb::Particle* DefaultParticle( const LHCb::Track * p );

  ITransportSvc * m_transSvc;
  IGeometryInfo* m_lhcbGeo;

  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;


  //Job options
  std::string m_InputDisplVertices;
  int m_MinNbtrks;     ///< Min number of daughters tracks in a RecVertex
  /*****************************************************************
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromUpstreamPVOpt"  : cut with respect to the upstream RV 
   ******************************************************************/
  std::string m_RCutMethod; 
  double m_RMin1;          ///< Min radial displacement 
  double m_RMin2;          ///< Min radial displacement 
  double m_MinMass1;       ///< Min reconstructed mass
  double m_MinMass2;       ///< Min reconstructed mass
  double m_MinSumpt1;      ///< Min sum of all daughters track
  double m_MinSumpt2;      ///< Min sum of all daughters track
  /*****************************************************************
   * Remove vtx reco in detector material
   * if = 0  : disabled
   * if < 0  : remove reco vtx if in detector material
   * if > 0  : remove reco vtx if rad length from decay pos - m_RemVtxFromDet 
   *           to decay pos + m_RemVtxFromDet along z is > threshold
   ******************************************************************/
  double m_RemVtxFromDet ;
  /*****************************************************************
   * Remove vtx reco in detector material
   * if < 0  : disabled
   *  if m_RemVtxFromDetSig > 0 : remove reco vtx if rad length along 
   *                             +- m_RemVtxFromDetSig * PositionCovMatrix
   ******************************************************************/
  double m_RemVtxFromDetSig ;

  GaudiUtils::VectorMap<int, const LHCb::Particle *> m_map;

  double m_piMass;  ///< the pion mass
  double m_pt;      ///< default pt for default pions

};
#endif // Hlt2DisplVertices_H
