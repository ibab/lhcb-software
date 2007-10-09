// $Id: PatMatch.h,v 1.1.1.1 2007-10-09 18:23:10 smenzeme Exp $
#ifndef PATMATCH_H
#define PATMATCH_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

#include "Event/Track.h"
#include "GaudiKernel/IRegistry.h"
#include "TrackInterfaces/IAddTTClusterTool.h"


  /** @class PatMatch PatMatch.h
   *  Match Velo and Seed tracks
   *
   *  @author Olivier Callot
   *  @date   2007-02-07 
   */
  class PatMatch : public GaudiAlgorithm
  {

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

  public:
    /// Standard constructor
    PatMatch( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~PatMatch( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution
    virtual StatusCode finalize  ();    ///< Algorithm finalization

      
  private:
    std::string m_veloLocation;
    std::string m_seedLocation;
    std::string m_matchLocation;

    double m_zMagnet;
    double m_zMatchY;
    double m_dxTol;
    double m_dxTolSlope;
    double m_dyTol;
    double m_dyTolSlope;
    double m_momentumScale;
    IAddTTClusterTool*   m_addTTClusterTool;
  };

#endif // PATMATCH_H

