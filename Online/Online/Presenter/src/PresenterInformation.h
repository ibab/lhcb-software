// $Id: PresenterInformation.h,v 1.2 2010-08-16 16:53:57 robbep Exp $
#ifndef PRESENTER_PRESENTERINFORMATION_H_
#define PRESENTER_PRESENTERINFORMATION_H_

#include "presenter.h"
#include <ctime>

/** @class PresenterInformation PresenterInformation.h
 *
 *  Small class to hold presenter information to exchange
 *  between the various classes
 *
 *  @author Patrick Robbe
 *  @date   2010-08-09
 */
class PresenterInformation
{
 public:
  /// default constructor
  PresenterInformation( ) ;

  virtual ~PresenterInformation( ) ; ///< Destructor
  
  /// Return presenter mode
  const pres::PresenterMode & presenterMode( ) const { return m_presenterMode ; } ;

  /// Set presenter mode
  void setPresenterMode( const pres::PresenterMode & presenterMode ) { m_presenterMode = presenterMode ; } ;

  /// Global history by run
  bool globalHistoryByRun( ) const { return m_globalHistoryByRun ; } ;

  /// Set global history by run
  void setGlobalHistoryByRun( const bool flag ) { m_globalHistoryByRun = flag ; } ;

  /// Get time point
  std::string rwTimePoint( ) const { return m_rwTimePoint ; } ; 

  /// Set time point
  void setRwTimePoint( const std::string & rw ) { m_rwTimePoint = rw ; } ;

  /// Get past duration
  std::string rwPastDuration( ) const { return m_rwPastDuration ; } ;

  /// Set past duration
  void setRwPastDuration( const std::string & rw ) { m_rwPastDuration = rw ; } ;

  /// Current TCK
  std::string currentTCK( ) const { return m_currentTCK ; } ;

  /// Set TCK
  void setTCK( const std::string & tck ) { m_currentTCK = tck ; } ;

  /// History trend plot mode
  bool isHistoryTrendPlotMode( ) const { return m_historyTrendPlots ; } ;

  /// Set history trend plot mode
  void setHistoryTrendPlotMode( bool mode ) { m_historyTrendPlots = mode ; } ;

  /// Current partition
  std::string currentPartition( ) const { return m_currentPartition ; } ;

  /// Set current partition
  void setPartition( const std::string & partition ) { m_currentPartition = partition; };

  /// Get current time
  TDatime * currentTime() const { return m_currentTime ; } ;

  /// Set global time point
  void setGlobalTimePoint( const std::string & tp ) { m_globalTimePoint = tp ; } ;

  /// Get global time point
  std::string globalTimePoint() const { return m_globalTimePoint ; } ;

  /// Set past duration
  void setGlobalPastDuration( const std::string & tp ) { m_globalPastDuration = tp ; } ;

  /// Get past duration
  std::string globalPastDuration( ) const { return m_globalPastDuration ; } ;

  /// Set global step size
  void setGlobalStepSize( const std::string & tp ) {  m_globalStepSize = tp ; } ;

  /// Get global step size
  std::string globalStepSize( ) const { return m_globalStepSize ; } ; 

  /// Set history mode
  void setHistoryMode( const bool mode ) { m_historyMode = mode ; } ; 

  /// Get history mode
  bool historyMode( ) const { return m_historyMode ; } ;

  /// Set reference Run 
  void setReferenceRun( const int rRun ) { m_referenceRun = rRun ; } ; 

  /// Get reference Run
  int referenceRun( ) const { return m_referenceRun; } ;

  std::time_t startTimeC() const { return m_startTimeC; }
  std::time_t endTimeC()   const { return m_endTimeC;   }

  void setOfflineContext( bool ctxt ) { m_offlineContext = ctxt; }
  bool offlineContext()               { return m_offlineContext; }

  void setProcessing( std::string proc ) { m_processing = proc; }
  std::string processing()               { return m_processing; }

  void setEventType( std::string type ) { m_eventType = type; }
  std::string eventType()               { return m_eventType; }

  void setRunType( std::string proc ) { m_runType = proc; }
  std::string runType()               { return m_runType; }

  void setVeloPosition( std::string proc ) { m_veloPosition = proc; }
  std::string veloPosition()               { return m_veloPosition; }

  void setMagnetState( std::string proc ) { m_magnetState = proc; }
  std::string magnetState()               { return m_magnetState; }

  void setBeamEnergy( std::string proc ) { m_beamEnergy = proc; }
  std::string beamEnergy()               { return m_beamEnergy; }

  void setTimeC( std::string endTime, std::string duration, bool add=false );

 private:
  pres::PresenterMode m_presenterMode ;      ///< presenter mode
  bool                m_globalHistoryByRun ; ///< global history mode
  std::string         m_currentTCK ;         ///< current TCK
  int                 m_referenceRun ;       ///< reference run number
  bool                m_historyTrendPlots ;  ///< history trend plot
  std::string         m_currentPartition ;   ///< Current partition
  std::string         m_rwTimePoint ;        ///< History stuff
  std::string         m_rwPastDuration ;     ///< History stuff
  TDatime *           m_currentTime ;        ///< Current time
  std::string         m_globalTimePoint ;    ///< Time point
  std::string         m_globalPastDuration ; ///< Past duration
  std::string         m_globalStepSize ;     ///< Step size
  bool                m_historyMode ;        ///< history mode
  std::time_t         m_startTimeC;
  std::time_t         m_endTimeC;
  bool                m_offlineContext;      ///< Running in offline context, for run file access
  std::string         m_processing;
  std::string         m_eventType;
  std::string         m_runType;
  std::string         m_veloPosition;
  std::string         m_magnetState;
  std::string         m_beamEnergy;
}; 

#endif /*PRESENTER_PRESENTERINFORMATION_H_*/
