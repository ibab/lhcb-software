// $Id: $
#ifndef MATCHVELOMUON_H 
#define MATCHVELOMUON_H 1

// Include files
#include <vector>

// from Gaudi
#include <GaudiAlg/GaudiHistoTool.h>
#include <TrackInterfaces/ITracksFromTrack.h>

// local
#include "Candidate.h"
#include "Hlt1MuonHit.h"

namespace LHCb {
   class Track;
}
class ILHCbMagnetSvc;
class Hlt1MuonHitManager;
class Candidate;

/** @class MatchVeloMuon MatchVeloMuon.h
 *  Look for Muon hits which match a velo track.
 *
 *  @author Roel Aaij
 *  @date   2010-12-02
 */
class MatchVeloMuon : public GaudiHistoTool, virtual public ITracksFromTrack {
public: 

   /// Standard constructor
   MatchVeloMuon( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

   virtual ~MatchVeloMuon( ); ///< Destructor

   virtual StatusCode initialize();

   virtual StatusCode finalize();
   
   virtual StatusCode tracksFromTrack( const LHCb::Track &seed,
                                       std::vector< LHCb::Track * > &tracks );
      
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
   Hlt1MuonHitManager* m_hitManager;

   // Services
   ILHCbMagnetSvc* m_fieldSvc;

   // Data members
   std::array< unsigned int, 4 > m_order;
   unsigned int m_nRegions;

   // Temporary storage
   Hlt1MuonHit* m_magnetHit;
   Candidates   m_seeds;

   // Helper methods
   void findSeeds( const Candidate& seed, const unsigned int seedStation );

   void addHits( Candidate& seed );

   void fitCandidate( Candidate& seed ) const;

   void clean();

   inline double dtx( const double p ) const {
      return  m_kickScale / ( p - m_kickOffset );
   }

   inline double momentum( const double dtx ) const {
      return m_kickScale / fabs( dtx ) + m_kickOffset;
   }

};
#endif // MATCHVELOMUON_H
