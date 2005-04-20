// $Id: Algorithm2ID.cpp,v 1.1 2005-04-20 14:08:39 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// local
#include "Algorithm2ID.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Algorithm2ID
//
// 2005-04-15 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<Algorithm2ID>          s_factory ;
const        IToolFactory& Algorithm2IDFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Algorithm2ID::Algorithm2ID( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IAlgorithm2ID>(this);
  declareProperty("AlgorithmNames", m_names);
}

//=============================================================================
// Initialize
//=============================================================================
StatusCode Algorithm2ID::initialize( ){
  StatusCode sc = GaudiTool::initialize();
  if (!sc) return sc;
  info() << "The following algorithms get an ID:" << endmsg ;
  int id = 0 ;
  for ( std::vector<std::string>::const_iterator is = m_names.begin();
        is != m_names.end(); ++is ){
    info() << id << " : " << *is << endmsg ;
    AlgNamePair anp(id,*is);
    m_pairs.push_back(anp);
    ++id ;
  }
  
  return sc;
}

//=============================================================================
// Initialize
//=============================================================================
int Algorithm2ID ::idFromName( const std::string& name ){
  for ( std::vector<AlgNamePair>::const_iterator ianp = m_pairs.begin();
        ianp != m_pairs.end(); ++ianp ){
    if ( name == ianp->getAlgName() ) return ianp->getID();
  }
  fatal() << "Algorithm " << name << " not known " << endmsg ;
  Exception("Algorithm "+name+" not known");
  return -999 ;
}
//=============================================================================
// Initialize
//=============================================================================
std::string Algorithm2ID ::nameFromId( const int& id ){
  for ( std::vector<AlgNamePair>::const_iterator ianp = m_pairs.begin();
        ianp != m_pairs.end(); ++ianp ){
    if ( id == ianp->getID() ) return ianp->getAlgName();
  }
  fatal() << "Algorithm ID " << id << " not known " << endmsg ;
  Exception("Algorithm ID not known");
  return "error" ;
}
