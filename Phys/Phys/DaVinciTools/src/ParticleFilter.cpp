// $Id: ParticleFilter.cpp,v 1.2 2002-07-27 20:24:53 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IHistogramSvc.h"
#include "AIDA/IHistogram1D.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// from Event
#include "Event/Particle.h"

// local
#include "ParticleFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleFilter
//
// 15/03/2002 : Paul Colrain
// 20/06/2002 : Sandra Amato: modified to include counters
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<ParticleFilter>          s_factory ;
const        IToolFactory& ParticleFilterFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleFilter::ParticleFilter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : AlgTool ( type, name , parent ) {

  // declare additional Interface
  declareInterface<IParticleFilter>(this);

  // declare properties
  declareProperty( "CriteriaNames", m_criteriaNames );
  declareProperty( "HistogramFlag", m_produceHistogram = false );
  
}

//=============================================================================
// initialize() method, build the vector of FilterCriterions
//=============================================================================
StatusCode ParticleFilter::initialize() {
  MsgStream          log( msgSvc(), name() );
  log << MSG::DEBUG << ">>> ParticleFilter::initialize() " << endreq;
  
  IToolSvc* tlSvc = 0;
  IHistogramSvc* histoSvc = 0;
  if ( serviceLocator() ) {
    StatusCode sc = StatusCode::FAILURE;
    sc = serviceLocator()->service( "ToolSvc", tlSvc );
    if (!sc){
      log << MSG::ERROR << "    Cannot retrieve Tool Service" << endreq;
      return StatusCode::FAILURE;
    }
    if(m_produceHistogram )   {    
      sc = serviceLocator()->service( "HistogramDataSvc", histoSvc,true );
      if (!sc){
        log << MSG::ERROR << "    Cannot retrieve Histo Service" << endreq;
        return StatusCode::FAILURE;
      }
    }
    
  }


  
  log << MSG::DEBUG << ">>> Setting FilterCriterion... " << endreq;
  IFilterCriterion* icrit;
  std::vector<std::string>::iterator it;
  for ( it = m_criteriaNames.begin(); it != m_criteriaNames.end(); it++ ) {
    StatusCode sc = tlSvc->retrieveTool( *it, icrit, this );
    if( sc.isFailure() ) return StatusCode::FAILURE;
    m_criteria.push_back( icrit );
    log << MSG::INFO << ">>> Criteria " << *it << " set successfully" 
        << endreq;
  }
  
  if( m_produceHistogram )   {
    log << MSG::INFO << "    histograms will be produced" << endreq;

    // Create the histograms 
   m_hIndCounter  = histoSvc-> book("/stat/pfilter/1",
      "N. of Particles passing each criteria independently", 
      10,0.0,10.0);
    if( 0 == m_hIndCounter ) {
      log << MSG::ERROR << "    Cannot register histogram 1" << endreq;
      return StatusCode::FAILURE;
    }
   m_hCumCounter  = histoSvc-> book("/stat/pfilter/2",
      "N. of Particles passing each criteria cumulatively", 
      10,0.0,10.0);
    if( 0 == m_hCumCounter ) {
      log << MSG::ERROR << "    Cannot register histogram 2" << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  m_independentCounter.clear();
  m_cumulativeCounter.clear();


  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter
//=============================================================================
StatusCode ParticleFilter::filter( const ParticleVector& input,
                                   ParticleVector& output ){

  MsgStream          log( msgSvc(), name() );
  log << MSG::INFO << ">>> Criteria size " <<m_criteria.size()    << endreq;
  log << MSG::INFO << "input size " << input.size()   << endreq;

  m_independentCounter.clear();
  m_cumulativeCounter.clear();
  
  std::vector<IFilterCriterion*>::iterator ic;
  for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
    m_independentCounter.push_back(0.);
    m_cumulativeCounter.push_back(0.);
  }

  std::vector< bool > partPassed;
  
  ParticleVector::const_iterator ip;  
  int index_part = 0;
  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    partPassed.push_back(true);
    
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    int index_criteria = 0;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if ((*ic)->isSatisfied(*ip)) {
        ++m_independentCounter[index_criteria] ;
  
        if(m_produceHistogram )   m_hIndCounter->fill(index_criteria,1);
        
        if (partPassed[index_part]){
          ++m_cumulativeCounter[index_criteria] ; 
          if(m_produceHistogram )  m_hCumCounter->fill(index_criteria,1);
        }
        
      }
      else {
        passed = false;
        partPassed[index_part] = false;
        
      }
    index_criteria++;

    }
    index_part++;
    
    if ( passed ) output.push_back( *ip );
  }

   
  return StatusCode::SUCCESS;
}

//=============================================================================
// Filter positive particles
//=============================================================================
StatusCode ParticleFilter::filterPositive( const ParticleVector& input,
                                   ParticleVector& output ){

  MsgStream          log( msgSvc(), name() );
  log << MSG::INFO << ">>> Criteria size " <<m_criteria.size()    << endreq;
  log << MSG::INFO << "input size " << input.size()   << endreq;

  m_independentCounter.clear();
  m_cumulativeCounter.clear();
  
  std::vector<IFilterCriterion*>::iterator ic;
  for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
    m_independentCounter.push_back(0.);
    m_cumulativeCounter.push_back(0.);
  }

  std::vector< bool > partPassed;
  
  ParticleVector::const_iterator ip;  
  int index_part = 0;
  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    partPassed.push_back(true);
    
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    int index_criteria = 0;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if ((*ic)->isSatisfied(*ip) and (*ip)->charge() > 0) {
        ++m_independentCounter[index_criteria] ;
        if(m_produceHistogram ) m_hIndCounter->fill(index_criteria,1);
        if (partPassed[index_part]){
          ++m_cumulativeCounter[index_criteria] ; 
          if(m_produceHistogram ) m_hCumCounter->fill(index_criteria,1);
        }
        
      }
      else {
        passed = false;
        partPassed[index_part] = false;
        
      }
    index_criteria++;

    }
    index_part++;
    
    //    if ( (*ip)->charge() < 0. ) passed = false;    
    if ( passed ) output.push_back( *ip );
  }


   
  return StatusCode::SUCCESS;
}


//=============================================================================
// Filter negative particles
//=============================================================================
StatusCode ParticleFilter::filterNegative( const ParticleVector& input,
                                   ParticleVector& output ){

  MsgStream          log( msgSvc(), name() );
  log << MSG::INFO << ">>> Criteria size " <<m_criteria.size()    << endreq;
  log << MSG::INFO << "input size " << input.size()   << endreq;

  m_independentCounter.clear();
  m_cumulativeCounter.clear();
  
  std::vector<IFilterCriterion*>::iterator ic;
  for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
    m_independentCounter.push_back(0.);
    m_cumulativeCounter.push_back(0.);
  }

  std::vector< bool > partPassed;
  
  ParticleVector::const_iterator ip;  
  int index_part = 0;
  
  for ( ip = input.begin(); ip != input.end(); ip++ ) {
    partPassed.push_back(true);
    
    std::vector<IFilterCriterion*>::iterator ic;
    bool passed = true;
    int index_criteria = 0;
    for( ic = m_criteria.begin(); ic != m_criteria.end(); ic++ ) { 
      if ((*ic)->isSatisfied(*ip) and (*ip)->charge() < 0) {
        ++m_independentCounter[index_criteria] ;
        if(m_produceHistogram )m_hIndCounter->fill(index_criteria,1);
        if (partPassed[index_part]){
          ++m_cumulativeCounter[index_criteria] ; 
          if(m_produceHistogram )m_hCumCounter->fill(index_criteria,1);
        }
        
      }
      else {
        passed = false;
        partPassed[index_part] = false;
        
      }
    index_criteria++;

    }
    index_part++;
    
    //    if ( (*ip)->charge() < 0. ) passed = false;    
    if ( passed ) output.push_back( *ip );
  }


   
  return StatusCode::SUCCESS;
}


//=============================================================================
// How many particles passed each criterium independently
//=============================================================================
 std::vector< int > ParticleFilter::independentCounters() 
   { 
     return m_independentCounter;
     
   }
 
//=============================================================================
// How many particles passed each criterium cumulatively
//=============================================================================
 std::vector< int > ParticleFilter::cumulativeCounters() 
   { 
     return m_cumulativeCounter;
     
   }
     



