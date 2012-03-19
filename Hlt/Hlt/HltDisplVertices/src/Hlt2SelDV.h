// $Id: Hlt2SelDV.h,v 1.4 2010-08-21 09:24:44 ngueissa Exp $
#ifndef HLT2SELDV_H 
#define HLT2SELDV_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"


/** @class Hlt2SelDV Hlt2SelDV.h
 *  
 *  @brief Apply a set of cuts on given Particles.
 *  @author Neal Gauvin
 *  @date   12 janvier 2010
 */
class Hlt2SelDV : public DVAlgorithm {
public: 
  /// Standard constructor
  Hlt2SelDV( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~Hlt2SelDV( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Is a point in the RF-Foil ?
  bool HasBackAndForwardTracks( const LHCb::RecVertex* );
  double GetSumPt( const LHCb::Particle * );

  //bool parametricPrescaler( double mass , double r , int cand); ///<Return True if the vertex should be triggered
  

  /***************************************************************//**
   * the type of R cut to be applied 
   * ""                   : cut with respect to (0,0,z)
   * "FromUpstreamPV"     : cut with respect to the upstream PV (PV3D)
   * "FromBeamLine"       : cut with respect to given beam line
   ******************************************************************/
  std::string m_RCut;         

  // cuts
  unsigned int m_NbCands;     ///< Min nb of desired candidates  
  bool m_NbCandsExclusive;    ///< Is the number of candidate exclusive
  unsigned int m_PVnbtrks;    ///< Min nb of trks for upstream PV candidate
  int    m_nTracks ;          ///< Min # of tracks at reconstructed vertex
  double m_PreyMinMass ;      ///< Minimum reconstructed mass
  double m_PreyMaxMass ;      ///< Maximum reconstructed mass
  double m_PreyMinAngle ;      ///< Minimum angle wrt smallest IP PV
  double m_PreyMinFD ;         ///< Minimum fligth distance wrt closest PV
  double m_SumPt ;            ///< Sumpt of all daughters tracks
  double m_PreyMaxSumPt ;      ///< Maximum reconstructed mass
  double m_RMin;              ///< Min dist to the z axis
  double m_RMax;              ///< Max dist to the z axis
  double m_MaxChi2OvNDoF;     ///< Max chi2 of a vertex
  double m_MuonpT;            ///< Muon with a pT of min m_MuonpT
  double m_MinRecpt;          ///< Min measured pT
  double m_SigmaZ;            ///< Max on error Z
  double m_SigmaR;            ///< Max on error R
  double m_MinX;              ///< Min X position     
  double m_MaxX;              ///< Max X position     
  double m_MinY;              ///< Min Y position     
  double m_MaxY;              ///< Max Y position     
  double m_MinZ;              ///< Min Z position     
  double m_MaxZ;              ///< Max Z position     
  double m_fracE;              ///< Fraction of Energy carried by highest E track
  double m_fracT;              ///< Fraction of track with first hit 5sigma before vertex
  bool m_allOutDet;           ///< Should all candidates be out of materVeto? 
  double m_PreyMinHighMass ;  ///< Minimum reconstructed mass of highest mass candidate  
  int m_RemVtxFromDet ;    
  bool m_useVelo;
  



  std::string m_Prey ;        ///< LHCb Name of the prey
  LHCb::ParticleID m_PreyID;  // PDG ID of this particle
  int    m_PreyPID ;          ///< PID of the prey (for MC and Gen use)
  //bool m_phaseSpacePS;

  struct sortPVdz {
    double refz; 
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      
      return std::abs( first->position().z() - refz ) < 
	std::abs( second->position().z() - refz );
    }
  } SortPVdz;
  ///To sort PVs with ascending z position 
  struct sortPVz {
    bool operator() ( const LHCb::RecVertex* first, 
		      const LHCb::RecVertex* second ) { 
      return first->position().z() < second->position().z();
    }
  } SortPVz;

};

#endif // HLT2SELDVTUPLE_H
