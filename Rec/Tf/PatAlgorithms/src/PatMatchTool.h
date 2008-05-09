// $Id: PatMatchTool.h,v 1.2 2008-05-09 08:26:15 cattanem Exp $
#ifndef PATMATCHTOOL_H 
#define PATMATCHTOOL_H 1

// Include files
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"
#include "TrackInterfaces/IAddTTClusterTool.h"

#include "TrackInterfaces/IMatchTool.h"            // Interface
#include "TrackInterfaces/IFastMomentumEstimate.h"

/** @class PatMatchTool PatMatchTool.h
 *  
 *  Match Velo and Seed tracks 
 *
 *  Johannes Albrecht: reorganize match as tool
 *  2008-04-25
 *
 *
 *  @author Olivier Callot
 *  @date   2007-02-07 
 */

class PatMatchTool : public GaudiTool, virtual public IMatchTool {
public: 
  /// Standard constructor
  PatMatchTool( const std::string& type, 
                const std::string& name,
                const IInterface* parent);
  
  virtual ~PatMatchTool( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  
  virtual StatusCode match(const LHCb::Tracks& velos,
                           const LHCb::Tracks& seeds,
                           LHCb::Tracks& matchs );

  virtual StatusCode matchSingle(const LHCb::Track& velo,
                                 const LHCb::Track& seed, 
                                 LHCb::Track& output ,
                                 double& chi2 );
  
  
public:

  /** @class MatchCandidate PatMatch.h
   *
   *  Match candidate for Tf::PatMatch algorithm
   *
   *  @author Olivier Callot
   *  @date   2007-02-07 
   */
  class MatchCandidate {
  public:
    MatchCandidate( LHCb::Track* vTr, LHCb::Track* sTr, double dist ) {
      m_vTr = vTr;
      m_sTr = sTr;
      m_dist = dist;
    };

    ~MatchCandidate() { };

    LHCb::Track* vTr() { return m_vTr; }
    LHCb::Track* sTr() { return m_sTr; }
    double dist()      { return m_dist; }

    class lowerByChi2 {
    public:
      bool operator() ( MatchCandidate first, MatchCandidate second ) const {
        return first.dist() < second.dist();
      }
    };

  private:
    LHCb::Track* m_vTr;
    LHCb::Track* m_sTr;
    double m_dist;
  };



private:

  double getChi2Match(const LHCb::Track& velo,
                      const LHCb::Track& seed );
  
  void makeTrack( const LHCb::Track& velo,
                  const LHCb::Track& seed,
                  LHCb::Track& output,
                  double chi2                 );
  
  
  // use tool for momentum parametrisation
  std::string m_fastMomentumToolName;
  IFastMomentumEstimate *m_fastMomentumTool;
  
  IAddTTClusterTool*   m_addTTClusterTool;
  
  double m_zMagnet;
  double m_zMatchY;
  double m_dxTol;
  double m_dxTolSlope;
  double m_dyTol;
  double m_dyTolSlope;
  double m_momentumScale;
  double m_maxChi2;
  
};
#endif // PATMATCHTOOL_H
