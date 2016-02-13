#ifndef MATCHVELOMUON_H
#define MATCHVELOMUON_H 1

// Include files
#include <vector>

// from Gaudi
#include <GaudiAlg/GaudiHistoTool.h>
#include <TrackInterfaces/ITracksFromTrack.h>

// Hlt1Muons
#include <Hlt1Muons/Candidate.h>
#include <Hlt1Muons/IMatchVeloMuon.h>

// from MuonID
#include <MuonID/CommonMuonHit.h>

namespace LHCb
{
   class Track;
}
class ILHCbMagnetSvc;
class CommonMuonHitManager;
class Candidate;

/** @class MatchVeloMuon MatchVeloMuon.h
 *  Look for Muon hits which match a velo track.
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class MatchVeloMuon : public extends1<GaudiHistoTool, IMatchVeloMuon> {
public:
   /// Standard constructor
   MatchVeloMuon( const std::string& type, const std::string& name,
                  const IInterface* parent );
  
   StatusCode initialize() override;

   StatusCode finalize() override;

   StatusCode tracksFromTrack( const LHCb::Track& seed,
                               std::vector<LHCb::Track*>& tracks ) const;

private:

   // Properties
   double m_za;
   double m_zb;

   double m_xWindow;
   double m_yWindow;

   double m_minMomentum;
   double m_kickScale;
   double m_kickOffset;

   double m_maxChi2DoFX;

   unsigned int m_maxMissed;

   bool m_setQOverP;

   // Tools
   CommonMuonHitManager* m_hitManager;

   // Services
   ILHCbMagnetSvc* m_fieldSvc;

   // Temporary storage
   mutable std::unique_ptr<CommonMuonHit> m_magnetHit;
   mutable std::vector<Candidate> m_seeds;

   // Helper methods
   void i_findSeeds( const LHCb::Track& seed, const unsigned int seedStation ) const;

   void i_addHits( Candidate& seed ) const;

   void i_fitCandidate( Candidate& seed ) const;

   void i_clean() const;

   inline double dtx( const double p ) const
   {
      return m_kickScale / ( p - m_kickOffset );
   }

   inline double momentum( const double dtx ) const
   {
      return m_kickScale / fabs( dtx ) + m_kickOffset;
   }
 
   const std::vector<Candidate>& seeds() const {
      return m_seeds;
   }
 
    // Interface methods
   void findSeeds( const LHCb::Track& seed, const unsigned int seedStation );

   void addHits( Candidate& seed ) override;

   void fitCandidate( Candidate& seed ) const override;

   void clean() override;

};
#endif // MATCHVELOMUON_H
