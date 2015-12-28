// $Id: MuonTrackMomRec.h,v 1.1 2010-02-10 19:20:17 ggiacomo Exp $
#ifndef COMPONENT_MUONTRACKMOMREC_H 
#define COMPONENT_MUONTRACKMOMREC_H 1

// Include files
#include <vector>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IIncidentListener.h"
#include "MuonInterfaces/IMuonTrackMomRec.h"            // Interface

class IBIntegrator;
class DeMuonDetector;

/** @class MuonTrackMomRec MuonTrackMomRec.h component/MuonTrackMomRec.h
 *    
 *
 *  @author Giacomo Graziani
 *  @date   2010-02-10
 *  
 *  tool to compute momentum for standalone muon track
 *  adapted from the TrackPtKick tool by M. Needham
 *
 *
 */
class MuonTrackMomRec : public extends<GaudiTool, IMuonTrackMomRec, IIncidentListener> {
public: 
  /// Standard constructor
  MuonTrackMomRec( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MuonTrackMomRec( ); ///< Destructor

  // from GaudiTool
  virtual StatusCode initialize();    
  
  // from IMuonTrackMomRec
  virtual StatusCode recMomentum(MuonTrack* track, 
                                 LHCb::Track* lbTrack) const;
  virtual double getBdl() const {return m_bdlX;}
  virtual double getZcenter() const {return m_zCenter;}

  // from IIncidentListener
  virtual void handle ( const Incident& incident );   

private:
  
  std::vector<double> m_ParabolicCorrection;
  std::vector<double> m_resParams;
  double m_Constant = 0;

  StatusCode  initBdl();

  IBIntegrator* m_bIntegrator = nullptr; // magnetic field tool
  DeMuonDetector* m_muonDetector = nullptr;
  double m_zCenter = 0; // Bx field center position in z
  double m_bdlX = 0;    // integrated Bx field
  int m_FieldPolarity = 0;

};
#endif // COMPONENT_MUONTRACKMOMREC_H
