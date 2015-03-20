#ifndef HLT1MUONS_IMATCHVELOMUON_H 
#define HLT1MUONS_IMATCHVELOMUON_H 1

// Include files
// from STL
#include <string>
#include <vector>

// Interface
#include <TrackInterfaces/ITracksFromTrack.h>

class Candidate;

/** @class IMatchVeloMuon IMatchVeloMuon.h Hlt1Muons/IMatchVeloMuon.h
 *  
 *
 *  @author Roel Aaij
 *  @date   2014-06-04
 */
class GAUDI_API IMatchVeloMuon : virtual public extend_interfaces1<ITracksFromTrack> {
public: 
   // interface ID
   DeclareInterfaceID( IMatchVeloMuon, 2, 0 );

   virtual void findSeeds( const Candidate& seed, const unsigned int seedStation ) = 0;

   virtual void addHits( Candidate& seed ) = 0;

   virtual void fitCandidate( Candidate& seed ) const = 0;

   virtual void clean() = 0;

   virtual const std::vector<Candidate>& seeds() const = 0;

   virtual const std::array<std::pair<double,double>,4>& foiInfo() const = 0;

};
#endif // HLT1MUONS_IMATCHVELOMUON_H
