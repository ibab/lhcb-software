// $Id: RONodeCounter.h,v 1.4 2010-10-19 14:38:23 ocallot Exp $
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
    m_ovf = 0;
    m_con = 0;
    m_snd = 0;
    m_acc = 0;
    m_tsk = 0;
    m_mepRate = 0.;
    m_ovfRate = 0.;
    m_conRate = 0.;
    m_sndRate = 0.;
    m_accRate = 0.;
    m_lastTime = 0.;
  }

  void updateMep( int k ) { m_mep += k; }
  void updateOvf( int k ) { m_ovf += k; }
  void updateCon( int k ) { m_con += k; }
  void updateSnd( int k ) { m_snd += k; }
  void updateAcc( int k ) { m_acc += k; }
  void updateTsk( int k ) { m_tsk += k; }

  void sum( RONodeCounter& cnt ) {
    m_mep += cnt.mep();
    m_ovf += cnt.ovf();
    m_con += cnt.con();
    m_snd += cnt.snd();
    m_acc += cnt.acc();
    m_tsk += cnt.tsk();
    m_mepRate += cnt.mepRate();
    m_ovfRate += cnt.ovfRate();
    m_conRate += cnt.conRate();
    m_sndRate += cnt.sndRate();
    m_accRate += cnt.accRate();
  }
  
  int mep() const { return m_mep; }
  int ovf() const { return m_ovf; }
  int con() const { return m_con; }
  int snd() const { return m_snd; }
  int acc() const { return m_acc; }
  int tsk() const { return m_tsk; }
  std::string name() const { return m_name; }
  float mepRate() const { return m_mepRate; }
  float ovfRate() const { return m_ovfRate; }
  float conRate() const { return m_conRate; }
  float sndRate() const { return m_sndRate; }
  float accRate() const { return m_accRate; }
  double lastTime() const { return m_lastTime; }

  void increment( RONodeCounter& cnt, double newTime ) {
    float invTime = 0.;
    if ( cnt.lastTime() < newTime ) invTime = float( 1. / (newTime - cnt.lastTime() ) );
    m_lastTime = newTime;
    m_mepRate = invTime * ( m_mep - cnt.mep() );
    if ( 0 > m_mepRate ) m_mepRate = 0.;
    m_ovfRate = invTime * ( m_ovf - cnt.ovf() );
    if ( 0 > m_ovfRate ) m_ovfRate = 0.;
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
  int m_mep;   // produced MEP
  int m_ovf;   // produced in OVERFLOW buffer
  int m_snd;   // produced in SEND buffer
  int m_con;   // consumed by Moore
  int m_acc;   // consumed by the sender task
  int m_tsk;   // Number of tasks
  float m_mepRate;
  float m_ovfRate;
  float m_sndRate;
  float m_conRate;
  float m_accRate;
  double m_lastTime;
};
#endif // RONODECOUNTER_H
