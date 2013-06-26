// $Id: PrVeloUTTool.h,v 1.8 2010-01-09 12:46:27 witekma Exp $
#ifndef PRVELOUTTOOL_H
#define PRVELOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "TfKernel/UTStationHitManager.h"

// local
#include "PrVUTTrack.h"
#include "PrUTMagnetTool.h"
#include "PrKernel/PrUTHit.h"

static const InterfaceID IID_PrVeloUTTool ( "PrVeloUTTool", 1, 0 );


  /** @class PrVeloUTTool PrVeloUTTool.h
   *
   *  PrVeloUT tool
   *  
   *  @author Mariusz Witek
   *  @date   2007-05-08
   *  @update for A-Team framework 2007-08-20 SHM
   *
   */

  class PrVeloUTTool : public GaudiTool {
  public:

    // Return the interface ID
    static const InterfaceID& interfaceID() { return IID_PrVeloUTTool; }

    /// Standard constructor
    PrVeloUTTool( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

    virtual ~PrVeloUTTool( ); ///< Destructor

    StatusCode initialize ( );


    void recoVeloUT(LHCb::Track & velotrack, std::vector<LHCb::Track*>& outtracks );
    void simpleFit( PrVUTTrack& vtt);

  protected:

    void simpleFitTracks( std::vector<PrVUTTrack>&);
    void selectBestTracks( std::vector<PrVUTTrack>& vttTracks);
    void prepareOutputTracks( std::vector<PrVUTTrack>& vttTracks, std::vector<LHCb::Track*>& outtracks);
    void localCleanUp(std::vector<PrVUTTrack>&);
    void getCandidates(LHCb::Track& veloTrack, std::vector<PrVUTTrack>& vtt);
    void saveCandidate( PrUTHits& theClusters, PrVUTTrack& candidate);

  private:
    class compPseudoChi2  {
    public:
      bool operator() (PrVUTTrack* first, PrVUTTrack* second ) {
        return fabs(first->chi2PerDoF()) < fabs(second->chi2PerDoF()) ;
      }
    };
    double m_maxXSlope;
    double m_maxYSlope;
    double m_centralHoleSize;
    double m_maxXSize;
    double m_maxYSize;
    double m_minMomentum;
    double m_minPT;
    double m_maxPseudoChi2;
    int m_maxSolutionsPerTrack;
    double m_xTol;
    double m_xTolSlope;
    double m_yTol;
    double m_yTolSlope;
    double m_dxGroupTol;
    double m_dxGroupFactor;
    double m_zMidUT;

    Tf::UTStationHitManager<PrUTHit> *      m_utHitManager;

    PrUTMagnetTool*    m_PrUTMagnetTool;  ///< Multipupose tool for Bdl and deflection
    bool m_debug;
    bool m_verbose;

  };

#endif // PRVELOUTTOOL_H
