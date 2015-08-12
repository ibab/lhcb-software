// $Id: TisTos.cpp,v 1.1 2010-07-21 21:22:16 tskwarni Exp $
// Include files 
#include <functional>
#include <sstream>

// local
#include "TisTos/TisTos.h"

#include "Kernel/CaloCellCode.h"

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TisTos
//
//     lowest layer of various TisTos tools classified collection of trigger hits
//     with respect to (offline) signal
//
// 2010-07-07 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// DECLARE_TOOL_FACTORY( TisTos );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TisTos::TisTos( const std::string& type,
                const std::string& name,
                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ITisTos>(this);

  declareProperty("NoHitTypes", m_noHitTypes = false);
  m_nHitTypes = nHitTypes;

  declareProperty("TOSFracVelo", m_TOSFrac[kVelo] = 0.7 );
  declareProperty("TOSFracOTIT", m_TOSFrac[kOTIT] = 0.7 );
  //  declareProperty("TOSFracTT",   m_TOSFrac[kTT]   = 0.7 );
  declareProperty("TOSFracTT",   m_TOSFrac[kTT]   = 0.0 );  // TT off by default (8/31/12)
  //  declareProperty("TOSFracMuon", m_TOSFrac[kMuon] = 0.6 ); // low threshold to avoid problems 
  declareProperty("TOSFracMuon", m_TOSFrac[kMuon] = 0.0001 );  // with Hlt1 muon lines in some data (8/31/12)  
  declareProperty("TOSFracEcal", m_TOSFrac[kEcal] = 0.01); 
  declareProperty("TOSFracHcal", m_TOSFrac[kHcal] = 0.01);

  declareProperty("TISFracVelo", m_TISFrac[kVelo] = 0.01 );
  declareProperty("TISFracOTIT", m_TISFrac[kOTIT] = 0.01 );
  declareProperty("TISFracTT",   m_TISFrac[kTT]   = 0.01 );
  //  declareProperty("TISFracMuon", m_TISFrac[kMuon] = 0.01 ); // see above
  declareProperty("TISFracMuon", m_TISFrac[kMuon] = 0.0001 ); 
  declareProperty("TISFracEcal", m_TISFrac[kEcal] = 0.0099 ); 
  declareProperty("TISFracHcal", m_TISFrac[kHcal] = 0.0099 );

}


//=============================================================================
// Destructor
//=============================================================================
TisTos::~TisTos() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TisTos::initialize() {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  
  if ( msgLevel(MSG::VERBOSE) ){
    verbose() << " TIS/TOS cuts at initialize " << endmsg;
    verbose() << " NoHitTypes " << m_noHitTypes << endmsg;    
    if( m_noHitTypes ){
      verbose() << " TOSFrac " <<  m_TOSFrac[kAllHits] << " TISFracVelo " << m_TISFrac[kAllHits] << endmsg;
    } else {
      verbose() << " TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << endmsg;
      verbose() << " TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << endmsg;    
      verbose() << " TOSFracTT   " <<  m_TOSFrac[kTT]   << " TISFracTT   " << m_TISFrac[kTT] << endmsg;    
      verbose() << " TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << endmsg;    
      verbose() << " TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << endmsg;
      verbose() << " TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << endmsg;
    }
  }

  if ( msgLevel(MSG::DEBUG) )
  {
    if( ! m_noHitTypes ){
      
      if( m_TOSFrac[kVelo]<=0.0 ){
        debug()<< "TOSFracVelo " <<  m_TOSFrac[kVelo] << " TISFracVelo " << m_TISFrac[kVelo] << " thus Velo hits not used " << endmsg;
      }
      if( m_TOSFrac[kOTIT]<=0.0  && m_nHitTypes>1 ){
        debug()<< "TOSFracOTIT " <<  m_TOSFrac[kOTIT] << " TISFracOTIT " << m_TISFrac[kOTIT] << " thus OTIT hits not used " << endmsg;
      }
      if( m_TOSFrac[kTT]<=0.0 && m_nHitTypes>2 ){
        debug()<< "TOSFracTT   " <<  m_TOSFrac[kTT] <<   " TISFracTT   " << m_TISFrac[kTT] << " thus TT hits not used " << endmsg;
      }
      if( m_TOSFrac[kMuon]<=0.0 && m_nHitTypes>3 ){
        debug()<< "TOSFracMuon " <<  m_TOSFrac[kMuon] << " TISFracMuon " << m_TISFrac[kMuon] << " thus Muon hits not used " << endmsg;
      }
      if( m_TOSFrac[kEcal]<=0.0  && m_nHitTypes>4 ){
        debug()<< "TOSFracEcal " <<  m_TOSFrac[kEcal] << " TISFracEcal " << m_TISFrac[kEcal] << " thus Ecal hits not used " << endmsg;
      }
      if( m_TOSFrac[kHcal]<=0.0  && m_nHitTypes>5 ){
        debug()<< "TOSFracHcal " <<  m_TOSFrac[kHcal] << " TISFracHcal " << m_TISFrac[kHcal] << " thus Hcal hits not used " << endmsg;
      }
    }
  }
 
  setSignal();

  return StatusCode::SUCCESS;

}

// ==================== internal classification of hit type for matching 
// see HitType enum
// =====================================================================
unsigned int TisTos::hitMatchType(const LHCbID & id)
{  
    if( id.isVelo() ){
      return kVelo;
    } else if ( id.isOT() || id.isIT() ){
      return kOTIT;
    } else if( id.isTT() ){
      return kTT;
    } else if ( id.isMuon() ) {
      return kMuon;
    } else if ( id.isCalo() ) {
      if( CaloCellCode::CaloNameFromNum( id.caloID().calo() ) == "Hcal" ){
        return kHcal;
      } else if (  CaloCellCode::CaloNameFromNum( id.caloID().calo() ) == "Ecal" ){
        return kEcal;
      }
    }
    return kNotUsed;
}


//=============================================================================
// -------------- Signal inputs -----------------------------------------------
//=============================================================================

// erase previous input ---------------------------------------------------------
bool TisTos::setSignal( )
{
  bool modified( m_Signal.size() );  
  m_Signal.clear();
  return modified;  
}
   

// (unsorted)  hit list input ---------------------------------------------------------------
bool TisTos::addHitsToSignal( const std::vector<LHCb::LHCbID> & hitlist )
{

  return addSortedHitsToSignal( sortedHits(hitlist) );

}

// sorted hit list input ---------------------------------------------------------------
bool TisTos::addSortedHitsToSignal( const std::vector<LHCb::LHCbID> & hitlist )
{

  bool modified(false);
  // addopted from Track code
  std::vector<LHCbID> & signalHits = m_Signal;
  std::vector<LHCbID> merged( signalHits.size() + hitlist.size() );
  std::vector<LHCbID>::iterator endmerged = std::set_union(signalHits.begin(), signalHits.end(),
                                                           hitlist.begin(), hitlist.end(),
                                                           merged.begin() );
  if( (unsigned int)(endmerged-merged.begin()) != signalHits.size() ){
    modified = true;        
    if( endmerged != merged.end() ){
      merged.erase( endmerged,merged.end() );
    }
    signalHits.swap(merged);
  }
  return modified;
}

//=============================================================================
// -------------- outputs:
//=============================================================================



// analyze trigger-signal relation, calculate overlap statistics 
//    interrupt if specified search condition met/failed, return condition value
unsigned int TisTos::analyze(const std::vector<LHCb::LHCbID> & triggerSortedHits,
                             unsigned int searchCond,
                             unsigned int* nHitsAll, unsigned int* nHitsMatched, double* overlap,
			     unsigned int & valid ) const
{
  
  valid =0;
  std::vector<LHCbID>::const_iterator first1 = triggerSortedHits.begin();
  std::vector<LHCbID>::const_iterator last1  = triggerSortedHits.end();
  std::vector<LHCbID>::const_iterator first2 = m_Signal.begin();
  std::vector<LHCbID>::const_iterator last2  = m_Signal.end();

  if( searchCond && !(m_Signal.size()||triggerSortedHits.size())  )return 0;

  valid =1;

  // ----------------------------------
  // no hit types used (not a default!)
  // ----------------------------------
  if( m_noHitTypes ){
    unsigned int rc=0;
    while (first1 != last1 && first2 != last2)
      if (*first1 < *first2)
        ++first1;
      else if (*first2 < *first1)
        ++first2;
      else {
        ++first1;
        ++first2;
        ++rc ;
      }
    *nHitsAll = triggerSortedHits.size();
    *nHitsMatched= rc;
    if( *nHitsAll ){
      *overlap = double(*nHitsMatched)/double(*nHitsAll);
      if( searchCond == kSearchForTOS ){
        if( *overlap < *m_TOSFrac )
          return 0;
        else
          return 1;
      } else if ( searchCond == kSearchForTUS ){
        if( *overlap > *m_TISFrac )
          return 1;
        else
          return 0;
      } else if ( searchCond == kSearchForTIS ){
        if( *overlap > *m_TISFrac )
          return 0;
        else
          return 1;
      } else {
        bool TOS(true),TIS(true),TPS(false);
        if( *overlap > *m_TISFrac ){
          TIS = false;
          if( *overlap < *m_TOSFrac ){
            TPS = true;
            TOS = false;
          }
        } else {
          TOS = false;
        }        
        return TisTosTob(TPS,TOS,TIS).value();
      }
    } else {
      // no hits on trigger object
      *overlap = 0.0;
      valid =0;
      return 0;
    }
  }

      
  // ------------------------------------------------------
  // using HitTypes
  // ------------------------------------------------------

  // calculate fraction of Trigger hits found in Signal for each hitType separately  

  // only need this if caller is interested in full analysis and not determining conditions   
  if( !searchCond ){
    unsigned int* iHitsAll=nHitsAll;  
    unsigned int* iHitsMatched=nHitsMatched;  
    double* iOverlap =overlap;  
    for( unsigned int hitType=0; hitType!=m_nHitTypes; ++hitType ){
      *iHitsAll++ =0;
      *iHitsMatched++ =0;
      *iOverlap++ = 0.0;    
    }
  }

  bool TOS(true),TIS(true),TPS(false);  
  bool trivial(true);



#define DECISION( jHitsAll , jHitsMatched, jHitType )      \
   if ( jHitType != kNotUsed ){                            \
    unsigned int* iHitsAll = &(nHitsAll[jHitType]);          \
    unsigned int* iHitsMatched = &(nHitsMatched[jHitType]);  \
    double* iOverlap = &(overlap[jHitType]);                 \
    *iHitsAll = jHitsAll; *iHitsMatched=jHitsMatched;      \
    if( *iHitsAll && (m_TOSFrac[jHitType]>0) ){            \
      trivial = false;                                     \
      *iOverlap = double(*iHitsMatched)/double(*iHitsAll); \
      if( searchCond == kSearchForTOS ){                   \
        if( *iOverlap < m_TOSFrac[jHitType] )return 0;     \
      } else if ( searchCond == kSearchForTUS ){           \
        if( *iOverlap > m_TISFrac[jHitType] )return 1;     \
      } else if ( searchCond == kSearchForTIS ){           \
        if( *iOverlap > m_TISFrac[jHitType] )return 0;     \
      } else {                                             \
        if( *iOverlap <= m_TISFrac[jHitType] ){            \
          TOS = false;                                     \
        } else {                                           \
          TIS = false;                                     \
          if ( *iOverlap < m_TOSFrac[jHitType]  ){         \
            TOS = false;                                   \
            TPS = true;                                    \
            if( searchCond )break;                         \
          }                                                \
        }                                                  \
        if( !(TOS||TIS) )                                  \
        {                                                  \
          TPS = true;                                      \
          if( searchCond )break;                           \
        }                                                  \
      }                                                    \
    } else {                                               \
      *iOverlap = 0.0;                                     \
    }                                                      \
  }                                                        \
  
  



  unsigned int previousHitsType=kNotUsed;  
  unsigned int previousHitsAll=0;
  unsigned int previousHitsMatched=0;

  do {
    if( first1 != last1 ){
      if( first2 != last2 ){
        if( *first2 < *first1 ){
          ++first2;
        } else {
          unsigned int hitType=hitMatchType(*first1);
          if( hitType!= previousHitsType ){
            DECISION( previousHitsAll,  previousHitsMatched, previousHitsType )
            previousHitsType = hitType;
            previousHitsAll=0;
            previousHitsMatched=0;
          }
          ++previousHitsAll;
          if( *first1++ == *first2 ){
            ++first2;
            ++previousHitsMatched;
          }
        }
      } else {
        unsigned int hitType=hitMatchType(*first1++);
        if( hitType!= previousHitsType ){
          DECISION( previousHitsAll,  previousHitsMatched, previousHitsType )
          previousHitsType = hitType;
          previousHitsAll=0;
          previousHitsMatched=0;
        }
        ++previousHitsAll;      
      }
    } else {
      DECISION( previousHitsAll,  previousHitsMatched, previousHitsType )
      break;      
    }
  } while(true);

  if( trivial ){
    valid = 0;
    return 0;
  }
  if( searchCond == kSearchForTOS )return 1;
  if( searchCond == kSearchForTUS )return 0;
  if( searchCond == kSearchForTIS )return 1;
  return TisTosTob(TPS,TOS,TIS).value();
}



// full classification
unsigned int TisTos::tisTosSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits, unsigned int & valid) const
{
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  return analyze( triggerSortedHits, kSearchForAll, nHitsAll,nHitsMatched,overlap,valid);
}
 

// fast search for TOS
bool TisTos::tosSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits, unsigned int & valid) const
{
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  return analyze( triggerSortedHits, kSearchForTOS, nHitsAll,nHitsMatched,overlap,valid);
}

// fast search for TIS
bool TisTos::tisSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits, unsigned int & valid) const
{
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  return analyze( triggerSortedHits, kSearchForTIS, nHitsAll,nHitsMatched,overlap,valid);
}

// fast search for TUS (Trigger Used Signal = TOS or TPS)
bool TisTos::tusSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits, unsigned int & valid) const
{
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  return analyze( triggerSortedHits, kSearchForTUS, nHitsAll,nHitsMatched,overlap,valid);
}



// ------------ auxiliary output:  list of LHCbIDs corresponding to present signal
std::vector<LHCb::LHCbID> TisTos::signal() const
{
  std::vector<LHCb::LHCbID> flatHitList;
  flatHitList.insert(flatHitList.end(),m_Signal.begin(),m_Signal.end());
  return flatHitList;
}

// Control calls ------------------------------


bool TisTos::setNoHitTypes(bool onOff)
{
  if( m_noHitTypes != onOff ){
    if ( msgLevel(MSG::VERBOSE) ) 
      verbose() << " Changed NoHitTypes to " << onOff << endmsg;
    m_noHitTypes = onOff;
    if( onOff )
      m_nHitTypes =1;
    else
      m_nHitTypes =nHitTypes;    
    return true;
  }
  return false;
    
}

bool TisTos::setTOSFrac(unsigned int hitType,double tosFrac)
{
  if( hitType<nHitTypes ){
    m_TOSFrac[hitType] = tosFrac;
    if ( msgLevel(MSG::VERBOSE) ) 
      verbose() << " TOSFrac changed for HitType " << hitTypeName(hitType) << " to " << tosFrac << endmsg;
    return true;    
  } else {
    Error(" illegal HitType in setTOSFrac  - ignored ", StatusCode::SUCCESS, 10 );
    return false;    
  }
}

bool TisTos::setTISFrac(unsigned int hitType,double tisFrac)
{
  if( hitType<nHitTypes ){
    m_TISFrac[hitType] = tisFrac;
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << " TISFrac changed for HitType " << hitTypeName(hitType) << " to " << tisFrac << endmsg;
      // intentionally print this warning only in verbose mode
      if( tisFrac> m_TOSFrac[hitType] ){
        Warning(" You set TISFrac above TOSFrac. This can upset TisTos logic - be worried. ",
                StatusCode::SUCCESS, 10).ignore();      
      }
    }
    return true;    
  } else {
    Error(" illegal HitType in setTISFrac  - ignored ", StatusCode::SUCCESS, 10 ).ignore();
    return false;    
  }
}


bool TisTos::setTOSFrac(std::vector<double> tosFrac)
{
  for(unsigned hitType=0; hitType<nHitTypes; ++hitType){
    if( hitType>= tosFrac.size() )break;
    m_TOSFrac[hitType] = tosFrac[hitType];
    if ( msgLevel(MSG::VERBOSE) ) 
      verbose() << " TOSFrac changed for HitType " << hitTypeName(hitType) << " to " << m_TOSFrac[hitType] << endmsg;   
  }
  return true; 
}

bool TisTos::setTISFrac(std::vector<double> tisFrac)
{
  for(unsigned hitType=0; hitType<nHitTypes; ++hitType){
    if( hitType>= tisFrac.size() )break;
    m_TISFrac[hitType] = tisFrac[hitType];
    if ( msgLevel(MSG::VERBOSE) ) 
    {
      verbose() << " TISFrac changed for HitType " << hitTypeName(hitType) << " to " << m_TISFrac[hitType] << endmsg;   
      // intentionally print this warning only in verbose mode
      if( m_TISFrac[hitType] > m_TOSFrac[hitType] ){
        Warning(" You set TISFrac above TOSFrac. This can upset TisTos logic - be worried. ",
                StatusCode::SUCCESS, 10).ignore();      
      }
    }    
  }
  return true; 
}

double TisTos::getTOSFrac(unsigned int hitType) const 
{
  if( hitType<nHitTypes )return m_TOSFrac[hitType];
  return 0.0;
}


double TisTos::getTISFrac(unsigned int hitType) const
{
  if( hitType<nHitTypes )return m_TISFrac[hitType];
  return 0.0;
}

std::vector<double> TisTos::getTOSFrac() const 
{
  std::vector<double> res;
  for(unsigned hitType=0; hitType<nHitTypes; ++hitType){ res.push_back(m_TOSFrac[hitType]);}
  return res;
}
  
std::vector<double> TisTos::getTISFrac() const 
{
  std::vector<double> res;
  for(unsigned hitType=0; hitType<nHitTypes; ++hitType){ res.push_back(m_TISFrac[hitType]);}
  return res;
}
  

unsigned int TisTos::analyzeSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits,
                         std::vector<unsigned int> & nTrigger ,
                         std::vector<double> & fractionInSignal,
				       unsigned int & valid ) const
{
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  unsigned int res=analyze( triggerSortedHits, kSearchNone, nHitsAll,nHitsMatched,overlap,valid);
  for( unsigned int hitType=0; hitType!=m_nHitTypes; ++hitType ){
    nTrigger.push_back( nHitsAll[hitType] );
    fractionInSignal.push_back( overlap[hitType] );
  }
  return res;  
}

std::string TisTos::analysisReportSortedHits(const std::vector<LHCb::LHCbID> & triggerSortedHits) const
{
  std::ostringstream report;
  unsigned int nHitsAll[nHitTypes],nHitsMatched[nHitTypes]; 
  double overlap[nHitTypes];
  unsigned int valid;
  TisTosTob res(analyze( triggerSortedHits, kSearchNone, nHitsAll,nHitsMatched,overlap,valid));
  report << " TIS= " << res.tis() << " TOS= " << res.tos() << " TPS= " << res.tps() << " valid= " << valid << " ";
  for( unsigned int hitType=0; hitType!=m_nHitTypes; ++hitType ){
    report << hitTypeName(hitType) << " " <<  nHitsAll[hitType] << " " 
           << std::setprecision(4) << overlap[hitType] << " ";
  }
  return report.str();
}

