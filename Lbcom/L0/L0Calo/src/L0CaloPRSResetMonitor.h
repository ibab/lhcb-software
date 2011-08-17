// $Id:  $
#ifndef L0CALOPRSRESETMONITOR_H 
#define L0CALOPRSRESETMONITOR_H 1

// Include files
// from Gaudi 
#include "GaudiAlg/GaudiHistoAlg.h"

// from Event
#include "Event/L0DUBase.h"

// Forward declarations
class DeCalorimeter ;

/** @class L0CaloPRSResetMonitor L0CaloPRSResetMonitor.h
 *  Finds empty boards in L0Ele and send an alarm to reset the
 *  PRS in case of empty boards.
 *
 *  @author Patrick Robbe
 *  @date   10/08/2011
 */

class L0CaloPRSResetMonitor: public GaudiHistoAlg {
  
public:
  /// Standard constructor
  L0CaloPRSResetMonitor( const std::string& name , ISvcLocator* pSvcLocator ) ;
  
  /// Standard destructor
  virtual ~L0CaloPRSResetMonitor( ) ; 
  
  /// Initialization
  virtual StatusCode initialize() ;
  
  /// Main execution routine: find empty boards
  virtual StatusCode execute   () ;

private:
  
  DeCalorimeter * m_ecal ; ///< Pointer to Ecal detector element

  unsigned int m_nEvents ; ///< Counter of events

  std::string m_inputData ; ///<  Location of input data

  /** Frequency with which the search of empty boards is launched.
   *  Set by option.
   */
  unsigned int  m_updateFrequency ;

  /// Alarm flag set when one board is missing in the L0Electron trigger
  int  m_alarm ;

  /// map to store number of canditates per board
  std::map< int , int > m_electronCounter ;
};
#endif // L0CALOPRSRESETMONITOR_H
