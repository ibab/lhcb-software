// $Id: PresenterInformation.cpp,v 1.1 2010-08-12 15:43:25 robbep Exp $
// This class
#include "PresenterInformation.h" 

//====================================================================
// Constructor
//====================================================================
PresenterInformation::PresenterInformation( ) : 
  m_presenterMode( pres::History ) , 
  m_globalHistoryByRun( false ) , 
  m_currentTCK( "" ) ,
  m_historyTrendPlots( true ) , 
  m_currentPartition( "" ) , 
  m_rwTimePoint( "" ) , 
  m_rwPastDuration( "" ) ,  
  m_currentTime( 0 ) , 
  m_globalTimePoint( "20081126T160921" ) ,  
  m_globalPastDuration( "00:05:00" ) , 
  m_globalStepSize( "00:15:00" ) , 
  m_historyMode( pres::s_timeInterval ) {
  if (0 == m_currentTime ) { m_currentTime = new TDatime(); }
}

//====================================================================
// Destructor
//====================================================================
PresenterInformation::~PresenterInformation( ) { 
  if ( 0 != m_currentTime ) delete m_currentTime ; 
}

