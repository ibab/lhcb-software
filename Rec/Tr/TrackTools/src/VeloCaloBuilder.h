#ifndef VELOCALOBUILDER_H
#define VELOCALOBUILDER_H 1

#include "GaudiAlg/GaudiTupleAlg.h"

#include "Event/Track.h"

#include "CaloInterfaces/ICaloGetterTool.h"

#include "Kernel/ILHCbMagnetSvc.h"

/** @class VeloCaloBuilder VeloCaloBuilder.h
 *  
 * \brief  Make a ValoCaloTrack: Get calo clusters and match them to velo tracks. no trackfit done.
 * 
 * Parameters:
 * - zcut: z position of the first velo state. for Ks reco reduces background
 * - IPcut: minimum pseudo IP.
 * - quali: minimum quality of the match (recommend 0.5 .. 1)
 * - VeloLoc, ClusterLoc, OutputTracks: TES locations of the input containers and the outputcontainer
 * - PtkickConstant: parameter for magnetic field
 * - zKick: z position of the magnet's bending plane (in cm !!!!)
 * - eRes0, eRes1: resolution of the calorimeter (energetic resolution)
 * - eCorrect: scal measured energy
 * 
 *  @author Paul Seyfert
 *  @date   2010-09-16
 */

class VeloCaloBuilder : public GaudiTupleAlg {

 public:
  /// Standard constructor
  VeloCaloBuilder(const std::string& name, ISvcLocator* pSvcLocator);
  /// Standard destructor
  virtual ~VeloCaloBuilder();
  /// initialization
  virtual StatusCode initialize();
  /// execution
  virtual StatusCode execute();

 private:
  // -- Methods
  float matchchi(LHCb::Track* velo, LHCb::Track* Calo); ///< calculate matching chi^2

  /// add track to output
  /// only 3 lines but there is more to be done in an elaborate study
  StatusCode TESpush(LHCb::Track* track,  LHCb::Track* ancestor, LHCb::Track* calotrack);

  // -- Variables
  LHCb::Tracks* m_caloTracks;
  
  std::string m_veloTracksName;
  std::string m_outputTracksName;
  std::string m_clusterlocation;
  
  float m_ipcut;
  float m_zcut;
  float m_qualimodi;
  float m_field;

  // -- tools
  ILHCbMagnetSvc* m_magFieldSvc;
  ICaloGetterTool* m_getter;

  // -- variables from HltVeloEcalMatch
  float 	m_ptkickConstant;
  float 	m_zKick;
  float 	m_eres [2];
  float 	m_eCorrect;

};
#endif
