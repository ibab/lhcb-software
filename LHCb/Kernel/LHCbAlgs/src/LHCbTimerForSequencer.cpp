
// local
#include "LHCbTimerForSequencer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : LHCbTimerForSequencer
//
// 2013-04-21 : Chris Jones
//-----------------------------------------------------------------------------

double LHCbTimerForSequencer::stop()
{
  double cpuTime  =  double(System::cpuTime( System::microSec ) - m_startCpu );
  double lastTime =  double(System::currentTime( System::microSec ) - m_startClock );

  //== Change to normalized millisecond
  cpuTime  *= m_factor;
  lastTime *= m_factor;

  //== Update the counter
  m_num    += 1;
  m_sum    += lastTime;
  m_sumCpu += cpuTime;

  if ( 1 == m_num )
  {
    m_min = lastTime;
    m_max = lastTime;
  }
  else
  {
    if ( lastTime < m_min ) m_min = lastTime;
    if ( lastTime > m_max ) m_max = lastTime;
  }
  m_lastTime = lastTime;
  m_lastCpu  = cpuTime;
  return lastTime;
}

MsgStream & LHCbTimerForSequencer::fillStream(MsgStream & s) const
{
  double ave = 0.;
  double cpu = 0.;

  if ( 0 != m_num )
  {
    ave = m_sum    / m_num;
    cpu = m_sumCpu / m_num;
  }

  return s << m_name.substr(0,m_size)
           << format( "| %9.3f | %9.3f | %8.3f %9.1f | %7d | %9.3f |",
                      cpu, ave, m_min, m_max, m_num, m_sum * 0.001 );
}

std::string LHCbTimerForSequencer::header( std::string::size_type size )
{
  if ( size < 21 ) size = 21;
  const std::string blank( size - 20, ' ' );
  const std::string s = 
    "Algorithm" + blank + "(millisec) |    <user> |   <clock> |" +
    "      min       max | entries | total (s) |";
  return s;
}
