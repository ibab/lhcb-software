// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "BackgroundCategoryViaRelations.h"

//-----------------------------------------------------------------------------
// Implementation file for class : BackgroundCategoryViaRelations
//
// 2009-11-27 : Vladimir Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( BackgroundCategoryViaRelations );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
BackgroundCategoryViaRelations::BackgroundCategoryViaRelations( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : GaudiTool ( type, name , parent )
  , m_P2BCLocation(0)
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
BackgroundCategoryViaRelations::~BackgroundCategoryViaRelations() {};
//=============================================================================
IBackgroundCategory::categories BackgroundCategoryViaRelations::category(const LHCb::Particle* reconstructed_mother)
//In this implementation just a) checks if the relation table exists and b) returns the category of the requested particle
{

  m_catRelations.clear();

  for (std::vector<std::string>::const_iterator item = m_P2BCLocation.begin(); item!=m_P2BCLocation.end(); ++item) {
    const std::string& address = *item;
    if (exist<TableP2BC>(address) ) {
      verbose() << "Adding table " << address << endmsg;
      TableP2BC* table = get<TableP2BC>(address);
      m_catRelations.push_back(table);
    } else {
      Error ( " There is no valid data at '" + address + "'" ).ignore() ;
    }
  }

  //If the location provided is rubbish complain
  if (m_catRelations.size() == 0) {
    Error ( " There is no valid data at the locations provided, returning category -1" ).ignore() ;
    return Undefined;
  }

  //Else we are OK so find the particle and return its category
  for (TablesP2BC::const_iterator iTableP2BC = m_catRelations.begin();
         iTableP2BC != m_catRelations.end();
         ++iTableP2BC) {
      TableP2BC::Range range = (*iTableP2BC)->relations(reconstructed_mother);
      //For the moment we only allow one category per particle so if more than one category is
      //related return undefined, else just return the category we find 
      if (range.size() == 0 || range.size() > 1) continue; 
      else return range.begin()->to(); 
  }

  //If we have not found anything else to return yet we return undefined, 
  //the user probably supplied the tables for the wrogn particles
  return Undefined;
}
//=============================================================================
StatusCode BackgroundCategoryViaRelations::finalize(){

  StatusCode sc = GaudiTool::finalize();
  if (!sc) return sc;
  return StatusCode::SUCCESS;

}
//=============================================================================
StatusCode BackgroundCategoryViaRelations::initialize(){
  //Initialize and get the required tools

  StatusCode sc = GaudiTool::initialize();
  
  //Check that someone provided a location for the relations table
  //maybe set a default eventually but not for now
  if (m_P2BCLocation.size() == 0) return StatusCode::FAILURE;

  if (!sc) return sc;
  return StatusCode::SUCCESS;
}
