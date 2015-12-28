// $Id: MuonLogPad.h,v 1.2 2010-02-11 09:29:51 ggiacomo Exp $
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
#include "MuonInterfaces/MuonLogHit.h"
#include <vector>
#include <ostream>

class MuonLogPad {
 public:
  /// standard constructor
  MuonLogPad() : m_crossed(false), m_tile( 0 ) , m_padhits(0),
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

  /// constructor fot crossed pad, note that hit1 must be the strip (giving X coordinate)
  ///  and hit2 must be the pad  (giving Y coordinate)
  MuonLogPad( MuonLogHit* hit1,
              MuonLogHit* hit2 ) :
    m_crossed(true), m_padhits(2), m_truepad(true)
  {
    m_tile = hit1->tile().intercept(hit2->tile());
    m_padhits[0] = hit1;
    m_padhits[1] = hit2;
    assignTimes(hit1->time(), hit2->time() );
    m_intime = (hit1->intime() && hit2->intime());
  }

  virtual ~MuonLogPad( ) = default;


  // public member functions
  typedef enum { NOX=0, XONEFE=1, XTWOFE=2, UNPAIRED=3} LogPadType;
  /// returns the type of logical pad (uncrossed, crossed with 1 or 2 FE channels, or unpaired log. channel)
  inline LogPadType type() {
    if (!m_truepad) return UNPAIRED;
    LogPadType t=XONEFE;
    if (m_tile.station() == 0 ||
        (m_tile.station()>2 && m_tile.region()==0) ) {
      t=NOX; // single log. channel
    }
    else {
      if (m_tile.station()>0 && m_tile.station()<3 && m_tile.region()<2)
        t=XTWOFE; // double readout for M23R12
    }
    return t;
  }

  inline LHCb::MuonTileID tile() const
  {
    return m_tile;
  }

  inline void assignTimes(float t1, float t2) {
    m_time = (float) (( t1 + t2 ) / 2.);
    m_dtime = (float) (( t1 - t2 ) / 2.);
  }



  inline void shiftTimes(float d1, float d2) {
    if (m_padhits.size() < 2) return;
    float t1=(m_padhits[0])->time() + d1;
    float t2=(m_padhits[1])->time() + d2;
    assignTimes(t1, t2);
  }

  inline void shiftTime(float delay) {
    m_time += delay;
  }

  /// return the pad time (averaged for crossed pads), in TDC bits
  inline float time()
  {
    return m_time;
  }
  inline float timeX() {
    LogPadType t=type();
    return (float) ( (t == XONEFE || t == XTWOFE) ? m_time+m_dtime : -9999. );
  }
  inline float timeY() {
    LogPadType t=type();
    return (float) ( (t == XONEFE || t == XTWOFE) ? m_time-m_dtime : -9999. );
  }
  /// return the half difference of the two hit times for crossed pads, in TDC bits
  inline float dtime()
  {
    return m_dtime;
  }
  /// true if this is the crossing of two logical channels
  inline bool crossed()
  {
    return m_crossed;
  }
  /// true if it's a real logical pad (if false, it's an unpaired logical channel)
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
  LHCb::MuonTileID m_tile;
  std::vector<MuonLogHit*> m_padhits;
  float m_time;
  float m_dtime;
  bool m_truepad;
  bool m_intime;
};




#endif // MUONLOGPAD_H

