// $Id: PatAddTTCoordF.h,v 1.5 2010-04-15 11:42:44 decianm Exp $
#ifndef PATADDTTCOORD_H
#define PATADDTTCOORD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/IAddTTClusterTool.h"            // Interface
#include "PatKernel/PatTTHit.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "TfKernel/TTStationHitManager.h"
#include "GaudiKernel/IIncidentListener.h"


  /** @class PatAddTTCoord PatAddTTCoord.h
   *
   * \brief  Adds TT clusters to tracks, see note LHCb-INT-2010-20 for the basic principle
   *
   *
   * Parameters:
   * - ZTTField: Z-Position of the kink for the state extrapolation
   * - ZMSPoint: Z-Position of the multiple scattering point
   * - TTParam: Parameter of the slope of the state extrapolation
   * - MaxChi2Tol: Offset of the chi2 cut
   * - MaxChi2Slope: Slope of the chi2 cut
   * - MaxChi2POffset: Momentum offest of the chi2 cut
   * - YTolSlope: Offest of the y-tolerance cut
   * - XTol: Offest of the x-window cut
   * - XTolSlope: Slope of the x-window cut
   * - MajAxProj: Major axis of the ellipse for the cut on the projection
   * - MinAxProj: Minor axis of the ellipse for the cut on the projection
   * - ZTTProj: Z-Position which state has to be closest to
   * The parameters were tuned on MC11a and differ from the ones stated in LHCb-INT-2010-20
   *  
   *  @author Olivier Callot
   *  @author Michel De Cian (added method and code restructured)
   *  @date   2014-11-13 
   *  
   */

class PatAddTTCoord : public GaudiTool, virtual public IAddTTClusterTool, public IIncidentListener {
public:
    /// Standard constructor
  PatAddTTCoord( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PatAddTTCoord( ); ///< Destructor
  
  virtual StatusCode initialize();

  /** @brief Add TT clusters to matched tracks. This calls returnTTClusters internally
      @param track Track to add the TT hits to
  */
  virtual StatusCode addTTClusters( LHCb::Track& track);
  
  /** Return TT clusters without adding them.
      @param state State closest to TT for extrapolation (normally Velo state)
      @param ttHits Container to fill TT hits in
      @param finalChi2 internal chi2 of the TT hit adding
      @param p momentum estimate. If none given, the one from the state will be taken
  */
  virtual StatusCode returnTTClusters( LHCb::State& state, PatTTHits& ttHits, double& finalChi2, double p = 0 );

  
  void handle ( const Incident& incident );
  
  
 private:

  void selectHits(const LHCb::State& state, const double p);
  void calculateChi2(double& chi2, const double& bestChi2, double& finalDist, const double& p );
  void printInfo(double dist, double chi2, const LHCb::State& state);
  void initEvent();
  
  bool m_newEvent;
  double m_invMajAxProj2;
  
  Tf::TTStationHitManager<PatTTHit> * m_ttHitManager;
  double m_ttParam;
  double m_zTTField;
  double m_zTTProj;
  double m_zMSPoint;
  
  double m_maxChi2Tol;
  double m_maxChi2Slope;
  double m_maxChi2POffset;
  double m_yTolSlope;
  double m_xTol;
  double m_xTolSlope;
  double m_majAxProj;
  double m_minAxProj;
  
  ILHCbMagnetSvc*     m_magFieldSvc; 
  double m_bendProtoParam;
  
  PatTTHits m_selected; // -- The container for all the hits compatible with the track
  PatTTHits m_goodTT; // -- The container with one "group" of hits
    
  
};

#endif // PATADDTTCOORD_H
