// $Id: PrAddUTCoord.h,v 1.5 2010-04-15 11:42:44 decianm Exp $
#ifndef PRADDUTCOORD_H
#define PRADDUTCOORD_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "Event/State.h"
#include "TrackInterfaces/IAddUTClusterTool.h"            // Interface
#include "PrKernel/PrUTHit.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "TfKernel/UTStationHitManager.h"


  /** @class PrAddUTCoord PrAddUTCoord.h
   *
   * \brief  Adds UT clusters to tracks, see note LHCb-INT-2010-20
   *
   * Parameters:
   * - ZUTField: Z-Position of the kink for the state extrapolation
   * - ZMSPoint: Z-Position of the multiple scattering point
   * - UTParam: Parameter of the slope of the state extrapolation
   * - MaxChi2Tol: Offset of the chi2 cut
   * - MaxChi2Slope: Slope of the chi2 cut
   * - MaxChi2POffset: Momentum offest of the chi2 cut
   * - YTolSlope: Offest of the y-tolerance cut
   * - XTol: Offest of the x-window cut
   * - XTolSlope: Slope of the x-window cut
   * - MajAxProj: Major axis of the ellipse for the cut on the projection
   * - MinAxProj: Minor axis of the ellipse for the cut on the projection
   * - ZUTProj: Z-Position which state has to be closest to
   * The parameters were tuned on MC11a and differ from the ones stated in LHCb-INT-2010-20
   *  
   *  @author Olivier Callot
   *  @author Michel De Cian (added method and code restructured)
   *  @date   2012-04-19 
   *  
   */

class PrAddUTCoord : public GaudiTool, virtual public IAddUTClusterTool {
public:
    /// Standard constructor
  PrAddUTCoord( const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~PrAddUTCoord( ); ///< Destructor
  
  virtual StatusCode initialize();

  /// Add UT clusters to matched tracks
  virtual StatusCode addUTClusters( LHCb::Track& track);
  virtual StatusCode returnUTClusters( LHCb::State& state, PrUTHits& utHits, double& finalChi2, double p = 0 );

  virtual StatusCode addUTClusters( LHCb::Track& /*track*/, 
                                    std::vector<LHCb::STCluster*>& /*utClusters*/,
                                    std::vector<double>& /*utChi2s*/ ) { return StatusCode::SUCCESS;};
  

  virtual double distanceToStrip( const LHCb::Track& /*track*/, 
                                  const LHCb::STCluster& /*utCluster*/ ) {return 0.;};
  
  
    
 

private:

  void selectHits(PrUTHits& selected, const LHCb::State& state, const double p);
  void calculateChi2(PrUTHits& goodUT,  double& chi2, const double& bestChi2, double& finalDist, const double& p );
  void printInfo(const PrUTHits& goodUT, double dist, double chi2, const LHCb::State& state);
  

  Tf::UTStationHitManager<PrUTHit> * m_utHitManager;
  double m_utParam;
  double m_zUTField;
  double m_zUTProj;
  double m_zMSPoint;
  
  double m_maxChi2Tol;
  double m_maxChi2Slope;
  double m_maxChi2POffset;
  double m_yTolSlope;
  double m_xTol;
  double m_xTolSlope;
  double m_majAxProj;
  double m_minAxProj;
  std::string m_extrapolatorName;
  
  ILHCbMagnetSvc*     m_magFieldSvc; 

};

#endif // PRADDUTCOORD_H
