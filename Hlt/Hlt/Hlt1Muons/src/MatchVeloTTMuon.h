// $Id: $
#ifndef MATCHVELOTTMUON_H
#define MATCHVELOTTMUON_H 1

// Include files
#include <vector>

// from Gaudi
#include <GaudiAlg/GaudiHistoTool.h>
#include <TrackInterfaces/ITracksFromTrack.h>
#include <TrackInterfaces/ITrackExtrapolator.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>
#include <Hlt1Muons/IMatchVeloMuon.h>

// MuonID
#include <MuonID/CommonMuonHit.h>

namespace LHCb
{
   class Track;
}
class ILHCbMagnetSvc;
class CommonMuonHitManager;
class Candidate;

/** @class MatchVeloTTMuon MatchVeloTTMuon.h
 *  Look for Muon hits which match a velo track.
 *
 *  @author Roel Aaij, Vasileios Syropoulos
 *  @date   2010-12-02
 */
class MatchVeloTTMuon : public extends1<GaudiHistoTool, IMatchVeloMuon> {
public:
   enum { nStations = 5u, nRegions = 4u };
   /// Standard constructor
   MatchVeloTTMuon( const std::string& type, const std::string& name,
                  const IInterface* parent );
  
   StatusCode initialize() override;

   StatusCode finalize() override;

   StatusCode tracksFromTrack( const LHCb::Track& seed,
                               std::vector<LHCb::Track*>& tracks ) ;

private:

   // Properties
   double m_FoIToleranceX;
   double m_FoIToleranceY;

   bool  m_chi2withVertPlane;
   double m_maxChi2DoFX;

   unsigned int m_maxMissed;

   bool m_setQOverP;

   // Properties for veloTT candidates w/o TT hits
     double m_xWindow;
     double m_yWindow;

     double m_za;
     double m_zb;

     double m_minMomentum;
     double m_kickScale;
     double m_kickOffset;

   // state info
   LHCb::State::Location  m_trackStateLoc ;

   // Tools
   CommonMuonHitManager* m_hitManager;
   ITrackExtrapolator* m_extrapolator;

   // Services
   ILHCbMagnetSvc* m_fieldSvc;

   // Muon Detector
   DeMuonDetector* m_det;
    
   // Data from detector description and conditions
   std::vector<double> m_padSizeX;
   std::vector<double> m_padSizeY;
   
   // function that defines the field of interest size
   // formula is p(1) + p(2)*exp(-p(3)*momentum)
   std::vector<double> m_xFoIParam1;
   std::vector<double> m_xFoIParam2;
   std::vector<double> m_xFoIParam3;
   
   std::vector<double> m_yFoIParam1;
   std::vector<double> m_yFoIParam2;
   std::vector<double> m_yFoIParam3;
   
   double m_FoIFactorX;
   double m_FoIFactorY;
   
   // Temporary storage
   std::unique_ptr<CommonMuonHit> m_magnetHit;
   std::vector<Candidate> m_seeds;
   std::array<std::pair<double,double>,4> m_foiInfo;
   
   // Helper methods
   void i_findSeeds( const Candidate& seed, const unsigned int seedStation );
 
   void i_addHits( Candidate& seed );

     // methods for veloTT candidateds w/o TT hits.
     void i_findSeeds_noTT( const Candidate& seed, const unsigned int seedStation );
    
     void i_addHits_noTT( Candidate& seed );

   void i_fitCandidate( Candidate& seed ) const;

   void i_clean();
 
   const std::vector<Candidate>& seeds() const {
      return m_seeds;
   }

   const std::array<std::pair<double,double>,4>& foiInfo() const override {
     return m_foiInfo;
   }

   // FoI formulas
   double FoIX( const int station, const int region, const double p ) const;
   double FoIY( const int station, const int region, const double p ) const;

   // mothods for veloTT candidates w/o TT hits
      inline double dtx( const double p ) const
      {
	return m_kickScale / ( p - m_kickOffset );
      } 
      
      inline double momentum( const double dtx ) const
      {
	return m_kickScale / fabs( dtx ) + m_kickOffset;
      }
      
    // Interface methods
    void findSeeds( const Candidate& seed, const unsigned int seedStation ) override;

    void addHits( Candidate& seed ) override;

    void fitCandidate( Candidate& seed ) const override;

    void clean() override;

};
#endif // MATCHVELOTTMUON_H
