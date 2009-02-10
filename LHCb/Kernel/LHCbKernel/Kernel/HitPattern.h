#ifndef LHCBKERNEL_HITPATTERN_H
#define LHCBKERNEL_HITPATTERN_H

#include <vector>
#include <bitset>
#include "Kernel/LHCbID.h"

namespace LHCb
{
  class HitPattern
  {
  public:
    enum {NumVelo=23,NumTT=4,NumT=12, NumMuon=5} ;
    // in velo we look at stations. the first two bits are reserved
    // for the pile-up detector (so no phi hits)
    //enum {VeloRA=0, VeloRC=1, VeloPhiA, VeloPhiC, VeloPUA, VeloPUC} ;
    enum VeloType {VeloRA=0, VeloRC=1, VeloPhiA, VeloPhiC} ;

    // this is the IT region (minus 1)
    enum ITType {ITC=0, ITA, ITTop, ITBottom } ;

    // this is the OT quarter (did we switch A and C?)
    enum OTType {OTBottomC=0, OTBottomA=1, OTTopC=2, OTTopA=3 } ;
    
    HitPattern( const std::vector<LHCbID>& ids) ;
    std::ostream& fillStream(std::ostream& s) const;

    // velo station pattern by velo type
    std::bitset<NumVelo> velo( VeloType type ) const { return m_velo[type] ; }

    // velo station pattern
    std::bitset<NumVelo> velo() const { 
      return m_velo[VeloRA] | m_velo[VeloPhiA] | m_velo[VeloRC] | m_velo[VeloPhiC] ; }

    // velo R station pattern
    std::bitset<NumVelo> veloR() const { return m_velo[VeloRA] | m_velo[VeloRC] ; }

    // velo phi station pattern
    std::bitset<NumVelo> veloPhi() const { return m_velo[VeloPhiA] | m_velo[VeloPhiC] ; }

    // tt layer pattern
    std::bitset<NumTT> tt() const { return m_tt ; }

    // it layer pattern by it type
    std::bitset<NumT> it( ITType type ) const { return m_it[type] ; }
    
    // it layer pattern
    std::bitset<NumT> it() const { 
      return m_it[ITC] | m_it[ITBottom] | m_it[ITTop] | m_it[ITA] ; }
    
    // ot layer pattern by ot type
    std::bitset<NumT> ot( OTType type ) const { return m_ot[type] ; }
    
    // ot layer pattern
    std::bitset<NumT> ot() const {
      return m_ot[OTTopA] | m_ot[OTTopC] | m_ot[OTBottomA] | m_ot[OTBottomC] ; }
    
    // muon station pattern
    std::bitset<NumMuon> muon() const { return m_muon ; }

    // number of velo R hits
    size_t numVeloR() const { return m_velo[VeloRA].count() + m_velo[VeloRC].count() ; }
    
    // number of velo phi hits
    size_t numVeloPhi() const { return m_velo[VeloPhiA].count() + m_velo[VeloPhiC].count() ; }
    
    // number of velo clusters: stations with an R and a phi hit.
    size_t numVeloClusters() const { 
      return (m_velo[VeloRA]&m_velo[VeloPhiA]).count() + (m_velo[VeloRC]&m_velo[VeloPhiC]).count() ; }
    
    // number of velo stations with one R or phi hit
    size_t numVeloStations() const { return velo().count() ; }
    
    // number of velo stations with one R or phi hit on both A and C side
    size_t numVeloStationsOverlap() const  ;

    // number of it stations with hit in more than one box
    size_t numITStationsOverlap() const ;
    
    // number of T stations with hits in both IT and OT
    size_t numITOTStationsOverlap() const ;

    // number of holes in velo (layer) pattern
    size_t numVeloHoles() const  ;
    
    // number of T-layers
    size_t numTLayers() const { return (ot() | it()).count() ; }

    // number of holes in T-layer pattern
    size_t numTHoles() const ;


  private:
    std::bitset<NumVelo> m_velo[4] ;
    std::bitset<NumTT>   m_tt ;
    std::bitset<NumT>    m_it[4] ;
    std::bitset<NumT>    m_ot[4] ;
    std::bitset<NumMuon> m_muon ;
  } ;
}

#endif
