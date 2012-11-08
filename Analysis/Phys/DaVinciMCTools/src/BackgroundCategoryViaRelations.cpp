// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "BackgroundCategoryViaRelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BackgroundCategoryViaRelations
//
// 2009-11-27 : Vladimir Gligorov
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BackgroundCategoryViaRelations::
BackgroundCategoryViaRelations( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  IBackgroundCategory::m_cat[-1]   = "Undefined";
  IBackgroundCategory::m_cat[0]    = "Signal";
  IBackgroundCategory::m_cat[10]   = "QuasiSignal";
  IBackgroundCategory::m_cat[20]   = "FullyRecoPhysBkg";
  IBackgroundCategory::m_cat[30]   = "Reflection";
  IBackgroundCategory::m_cat[40]   = "PartRecoPhysBkg";
  IBackgroundCategory::m_cat[50]   = "LowMassBkg";
  IBackgroundCategory::m_cat[60]   = "Ghost";
  IBackgroundCategory::m_cat[63]   = "Clone";
  IBackgroundCategory::m_cat[66]   = "Hierarchy";
  IBackgroundCategory::m_cat[70]   = "FromPV";
  IBackgroundCategory::m_cat[80]   = "AllFromSamePV";
  IBackgroundCategory::m_cat[100]  = "FromDifferentPV";
  IBackgroundCategory::m_cat[110]  = "bbar";
  IBackgroundCategory::m_cat[120]  = "ccbar";
  IBackgroundCategory::m_cat[130]  = "uds";
  IBackgroundCategory::m_cat[1000] = "LastGlobal";

  declareInterface<IBackgroundCategory>(this);
  declareProperty("inputTable", m_P2BCLocation);
}

//=============================================================================
// Destructor
//=============================================================================
BackgroundCategoryViaRelations::~BackgroundCategoryViaRelations() {}

//=============================================================================
IBackgroundCategory::categories
BackgroundCategoryViaRelations::category( const LHCb::Particle* reconstructed_mother,
                                          const LHCb::Particle* headP )
//In this implementation just a) checks if the relation table exists and b) returns the category of the requested particle
{

  m_catRelations.clear();

  // locations in JOs
  std::vector<std::string> locs = m_P2BCLocation;

  // add particle dependent locations
  if ( headP && headP != reconstructed_mother ) 
  {
    locs.push_back( b2cLocation(headP->parent()) );
  }
  if ( reconstructed_mother ) 
  {
    locs.push_back( b2cLocation(reconstructed_mother->parent()) );
  }

  std::sort( locs.begin(),locs.end() );
  locs.erase( std::unique(locs.begin(),locs.end()), locs.end() );

  for ( std::vector<std::string>::const_iterator item = locs.begin(); 
        item != locs.end(); ++item )
  {
    const std::string & address = *item;
    TableP2BC * table = getIfExists<TableP2BC>(address);
    if ( table ) 
    {
      if ( msgLevel(MSG::VERBOSE) )
        verbose() << "Adding table " << address << endmsg;
      m_catRelations.push_back(table);
    }
  }
  
  // If the location provided is rubbish complain
  if ( m_catRelations.empty() )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "There is no valid data at the locations provided, returning category Undefined"
              << endmsg;
    //Warning( "There is no valid data at the locations provided, returning category Undefined",
    //         StatusCode::SUCCESS, 0 ).ignore() ;
    return Undefined;
  }
  
  // Else we are OK so find the particle and return its category
  for ( TablesP2BC::const_iterator iTableP2BC = m_catRelations.begin();
        iTableP2BC != m_catRelations.end(); ++iTableP2BC ) 
  {
    TableP2BC::Range range = (*iTableP2BC)->relations(reconstructed_mother);
    //For the moment we only allow one category per particle so if more than one category is
    //related return undefined, else just return the category we find
    if ( range.empty() || range.size() > 1 ) 
    {
      continue;
    }
    else
    {
      return (IBackgroundCategory::categories) range.begin() -> to();
    }
  }
  
  //If we have not found anything else to return yet we return undefined,
  //the user probably supplied the tables for the wrogn particles
  return Undefined;
}

//=============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( BackgroundCategoryViaRelations )

//=============================================================================
