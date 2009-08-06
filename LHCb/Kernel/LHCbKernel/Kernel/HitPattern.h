#ifndef LHCBKERNEL_HITPATTERN_H
#define LHCBKERNEL_HITPATTERN_H

#include <vector>
#include <bitset>
#include "Kernel/LHCbID.h"
#include "OTDet/DeOTDetector.h"


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

    // this is the IT regions
    enum ITType {ITAC=0, ITTopBottom} ;

    // this is the OT monolagyers
    enum OTType {OT1stMonoLayer=0, OT2ndMonoLayer=1 } ;
    
    HitPattern( const std::vector<LHCbID>& ids, const DeOTDetector* otDet ) ;

    HitPattern();

    std::ostream& fillStream(std::ostream& s) const; 

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
      return m_it[ITAC] | m_it[ITTopBottom] ; }
    
    // ot layer pattern by ot type
    std::bitset<NumT> ot( OTType type ) const { return m_ot[type] ; }
    
    // ot layer pattern
    std::bitset<NumT> ot() const {
      return m_ot[OT1stMonoLayer] | m_ot[OT2ndMonoLayer]; }
    
    // muon station pattern
    std::bitset<NumMuon> muon() const { return m_muon ; }

    // number of velo R hits
    size_t numVeloR() const { return m_velo[VeloRA].count() + m_velo[VeloRC].count() ; }
    
    // number of velo phi hits
    size_t numVeloPhi() const { return m_velo[VeloPhiA].count() + m_velo[VeloPhiC].count() ; }
    
    // number of velo clusters: stations with an R and a phi hit.
    size_t numVeloClusters() const { 
      return (m_velo[VeloRA]&m_velo[VeloPhiA]).count() + (m_velo[VeloRC]&m_velo[VeloPhiC]).count() ; }

    // number of OT hits, both monolayers: 
    size_t numOTHits() const { 
      return m_ot[OT1stMonoLayer].count() + m_ot[OT2ndMonoLayer].count(); }

    // number of IT hits
    size_t numITHits() const { 
      return m_it[ITAC].count() + m_it[ITTopBottom].count(); }
    
    // number of TT hits
    size_t numTTHits() const { 
      return m_tt.count(); }

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

    // set velo bit map
    void setVelo( std::bitset<NumVelo> velo[4]){
      for (int i=0; i<4; i++)
	m_velo[i] = velo[i];
    }

    // set TT bit map
    void setTT( std::bitset<NumTT> tt){
      m_tt = tt;
    }

    // set IT bit map
    void setIT( std::bitset<NumT> it[2]){
      m_it[0] = it[0];
      m_it[1] = it[1];
    }

    //set OT bit map
    void setOT( std::bitset<NumT> ot[2]){
      m_ot[0] = ot[0];
      m_ot[1] = ot[1];
    }

    void reset (){
	for (unsigned int i=0; i<4; i++)
	    m_velo[i].reset();
	
	m_tt.reset();
	
	for (unsigned int i=0; i<2; i++){
	    m_it[i].reset();
	    m_ot[i].reset();
	}
	
	m_muon.reset();    
    }
    

  private:
    std::bitset<NumVelo> m_velo[4] ;
    std::bitset<NumTT>   m_tt ;
    std::bitset<NumT>    m_it[2] ;
    std::bitset<NumT>    m_ot[2] ;
    std::bitset<NumMuon> m_muon ;
  } ;
}

#endif
