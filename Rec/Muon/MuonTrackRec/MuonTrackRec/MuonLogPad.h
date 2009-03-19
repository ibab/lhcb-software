// $Id: MuonLogPad.h,v 1.1.1.1 2009-03-19 14:38:47 ggiacomo Exp $
#ifndef MUONLOGPAD_H 
#define MUONLOGPAD_H 1

/** @class MuonLogPad MuonLogPad.h
 *  
 *  Muon logical pad class for standalone cosmics reconstruction 
 *  @author Giacomo
 *  @date   2007-10-26
 */


#include "MuonDet/DeMuonDetector.h"
#include "Kernel/MuonTileID.h"
#include "MuonTrackRec/MuonLogHit.h"
#include <vector>


class MuonLogPad {
 public: 
  /// standard constructor
  MuonLogPad() : m_crossed(false), m_tile( NULL ) , m_padhits(0),
                 m_time( 0 ), m_dtime(0), m_truepad(false),
                 m_intime(false)  {}
  /// constructor for uncrossed pad 
  MuonLogPad( MuonLogHit* hit ) :
    m_crossed(false), m_tile( hit->tile() ) , m_padhits(1),
    m_time( hit->time() ), m_dtime(0), m_truepad(false)
  {
    m_padhits[0] = hit;
    m_intime = hit->intime();
  }
  
  /// constructor fot crossed pad
  MuonLogPad( MuonLogHit* hit1,
              MuonLogHit* hit2 ) :
    m_crossed(true), m_padhits(2), m_truepad(true)
  {
    m_tile = new LHCb::MuonTileID(hit1->tile()->intercept(*(hit2->tile()) ) );
    m_padhits[0] = hit1;
    m_padhits[1] = hit2;
    m_time = ( hit1->time() + hit2->time() ) / 2.;
    m_dtime = ( hit1->time() - hit2->time() ) / 2.;
    m_intime = (hit1->intime() && hit2->intime());
  }
  
  virtual ~MuonLogPad( ) 
  {
    if(m_crossed) delete m_tile;
  }
  
  // public member functions
  inline const LHCb::MuonTileID* tile()
  {
    return m_tile;
  }
  /// return the pad time (averaged for crossed pads), in TDC bits
  inline float time()
  {
    return m_time;
  }  
  /// return the half difference of the two hit times for crossed pads, in TDC bits
  inline float dtime()
  {
    return m_dtime;
  }
  /// true if this is a logical channel that should be crossed 
  inline bool crossed() 
  {
    return m_crossed;
  }
  inline bool truepad() 
  {
    return m_truepad;
  }
  inline void settruepad() {
    m_truepad = true;
  }
  inline bool intime() {
    return m_intime;
  }
  inline MuonLogHit* getHit(unsigned int i) {
    MuonLogHit* out=NULL;
    if(i < m_padhits.size())
      out=m_padhits[i];
    return out;
  }
  inline std::vector<MuonLogHit*> getHits() {
    std::vector<MuonLogHit*> out;
    std::vector<MuonLogHit*>::iterator ih;
    for (ih=m_padhits.begin() ; ih != m_padhits.end(); ih++)
      out.push_back(*ih);
    return out;
  }

 private:
  bool m_crossed;
  const LHCb::MuonTileID* m_tile;
  std::vector<MuonLogHit*> m_padhits;  
  float m_time;
  float m_dtime;
  bool m_truepad;
  bool m_intime;
};




#endif // MUONLOGPAD_H

