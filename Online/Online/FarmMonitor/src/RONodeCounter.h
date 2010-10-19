// $Id: RONodeCounter.h,v 1.3 2010-10-19 14:34:47 ocallot Exp $
#ifndef RONODECOUNTER_H 
#define RONODECOUNTER_H 1

// Include files

/** @class RONodeCounter RONodeCounter.h
 *  Simple class to store the information retrieved from a node.
 *
 *  @author Olivier Callot
 *  @date   2008-03-19
 */
class RONodeCounter {
public: 
  /// Standard constructor
  RONodeCounter( const char* name ) : m_name( name ) {
    reset();
  }

  virtual ~RONodeCounter( ) {}; ///< Destructor

  void reset() {
    m_mep = 0;
    m_evt = 0;
    m_con = 0;
    m_snd = 0;
    m_acc = 0;
    m_tsk = 0;
    m_mepRate = 0.;
    m_evtRate = 0.;
    m_conRate = 0.;
    m_sndRate = 0.;
    m_accRate = 0.;
    m_lastTime = 0.;
  }

  void updateMep( int k ) { m_mep += k; }
  void updateEvt( int k ) { m_evt += k; }
  void updateCon( int k ) { m_con += k; }
  void updateSnd( int k ) { m_snd += k; }
  void updateAcc( int k ) { m_acc += k; }
  void updateTsk( int k ) { m_tsk += k; }

  void sum( RONodeCounter& cnt ) {
    m_mep += cnt.mep();
    m_evt += cnt.evt();
    m_con += cnt.con();
    m_snd += cnt.snd();
    m_acc += cnt.acc();
    m_tsk += cnt.tsk();
    m_mepRate += cnt.mepRate();
    m_evtRate += cnt.evtRate();
    m_conRate += cnt.conRate();
    m_sndRate += cnt.sndRate();
    m_accRate += cnt.accRate();
  }
  
  int mep() const { return m_mep; }
  int evt() const { return m_evt; }
  int con() const { return m_con; }
  int snd() const { return m_snd; }
  int acc() const { return m_acc; }
  int tsk() const { return m_tsk; }
  std::string name() const { return m_name; }
  float mepRate() const { return m_mepRate; }
  float evtRate() const { return m_evtRate; }
  float conRate() const { return m_conRate; }
  float sndRate() const { return m_sndRate; }
  float accRate() const { return m_accRate; }
  double lastTime() const { return m_lastTime; }

  void increment( RONodeCounter& cnt, double newTime ) {
    float invTime = 0.;
    if ( cnt.lastTime() < newTime ) invTime = float( 1. / (newTime - cnt.lastTime() );
    m_lastTime = newTime;
    m_mepRate = invTime * ( m_mep - cnt.mep() );
    if ( 0 > m_mepRate ) m_mepRate = 0.;
    m_evtRate = invTime * ( m_evt - cnt.evt() );
    if ( 0 > m_evtRate ) m_evtRate = 0.;
    m_conRate = invTime * ( m_con - cnt.con() );
    if ( 0 > m_conRate ) m_conRate = 0.;
    m_sndRate = invTime * ( m_snd - cnt.snd() );
    if ( 0 > m_sndRate ) m_sndRate = 0.;
    m_accRate = invTime * ( m_acc - cnt.acc() );
    if ( 0 > m_accRate ) m_accRate = 0.;
  }

protected:

private:
  std::string m_name;
  int m_mep;
  int m_evt;
  int m_snd;
  int m_con;
  int m_acc;
  int m_tsk;
  float m_mepRate;
  float m_evtRate;
  float m_sndRate;
  float m_conRate;
  float m_accRate;
  double m_lastTime;
};
#endif // RONODECOUNTER_H
