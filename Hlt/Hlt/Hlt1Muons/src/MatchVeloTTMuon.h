// $Id: $
#ifndef MATCHVELOTTMUON_H
#define MATCHVELOTTMUON_H 1

// Include files
#include <vector>
#include <unordered_map>

// from Gaudi
#include <GaudiAlg/GaudiHistoTool.h>
#include <TrackInterfaces/ITracksFromTrack.h>

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
                               std::vector<LHCb::Track*>& tracks ) const override;

private:

   // Properties for Velo seeds
   double m_z0;
   double m_z1;
   double m_c0;
   double m_c1;

   //double m_xWindow;
   //double m_yWindow;
   
   using WindowMap = std::unordered_map<unsigned int, std::pair<double, double>>;
   WindowMap m_window;

   double m_minMomentum;
   double m_maxPt;
   double m_kickScale;
   double m_kickOffset;

   double m_maxChi2DoFX;

   bool m_fitY;

   // Corrections
   using CorrectMap = std::unordered_map<std::string, std::vector<double> >;
   CorrectMap m_errCorrect;

   std::array<double, 5> m_corX;
   std::array<double, 5> m_corY;

   // General properties
   unsigned int m_maxMissed;
   bool m_setQOverP;

   // Tools
   CommonMuonHitManager* m_hitManager;

   // Services
   ILHCbMagnetSvc* m_fieldSvc;

   // Muon Detector
   DeMuonDetector* m_det;

   // Data from detector description and conditions
   std::vector<double> m_padSizeX;
   std::vector<double> m_padSizeY;

   // Temporary storage
   mutable std::vector<std::unique_ptr<CommonMuonHit>> m_magnetHits;
   mutable std::vector<Candidate> m_seeds;

   // Helper methods
   void i_findSeeds      ( const LHCb::Track& seed, const unsigned int seedStation ) const;

   void i_addHits( Candidate& seed ) const;

   void i_fitCandidate( Candidate& seed ) const;

   void i_clean() const;

   const std::vector<Candidate>& seeds() const {
      return m_seeds;
   }

   // Interface methods
   void findSeeds( const LHCb::Track& seed, const unsigned int seedStation ) override;

   void addHits( Candidate& seed ) override;

   void fitCandidate( Candidate& seed ) const override;

   void clean() override;

   inline double dtx( const double p ) const {
      return m_kickScale / ( p - m_kickOffset );
   }

   inline double momentum( const double dtx ) const{
      return m_kickScale / fabs( dtx ) + m_kickOffset;
   }


   inline std::pair<double, double> hitx(const CommonMuonHit& hit, bool correct) const
   {
      unsigned int i = (hit.tile().key() == 0 ? 0 : hit.station());
      return std::make_pair(hit.x(), 2 * hit.dx() * (correct ? m_corX[i] : 1.));
   }
   inline std::pair<double, double> hity(const CommonMuonHit& hit, bool correct) const
   {
     unsigned int i = (hit.tile().key() == 0 ? 0 : hit.station());
     return std::make_pair(hit.y(), 2 * hit.dy() * (correct ? m_corY[i] : 1.));
   }

};
#endif // MATCHVELOTTMUON_H
