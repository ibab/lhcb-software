// $Id: $
#ifndef MATCHVELOTTMUON_H
#define MATCHVELOTTMUON_H 1

// Include files
#include <vector>

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
                               std::vector<LHCb::Track*>& tracks ) const ;

private:

   // Properties for Velo seeds
   double m_za;
   double m_zb;

   double m_xWindow;
   double m_yWindow;

   double m_minMomentum;
   double m_maxPt;
   double m_kickScale;
   double m_kickOffset;

   // Properties for VeloTT seeds
   double m_FoIToleranceX;
   double m_FoIToleranceY;

   bool  m_chi2withVertPlane;
   double m_maxChi2DoFX;

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
   mutable std::unique_ptr<CommonMuonHit> m_magnetHit;
   mutable std::vector<Candidate> m_seeds;
    
   // Helper methods
   void i_findSeeds      ( const LHCb::Track& seed, const unsigned int seedStation ) const;
   void i_findVeloSeeds  ( const Candidate& seed,   const unsigned int seedStation ) const;
   void i_findVeloTTSeeds( const Candidate& seed,   const unsigned int seedStation ) const;

   void i_addHits( Candidate& seed ) const;

   void i_fitCandidate( Candidate& seed ) const;

   void i_clean() const;
 
   const std::vector<Candidate>& seeds() const {
      return m_seeds;
   }

   // FoI formulas
   double FoIX( const int station, const int region, const double p ) const;
   double FoIY( const int station, const int region, const double p ) const;

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

};
#endif // MATCHVELOTTMUON_H
