// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from DaVinci
#include "DaVinciMCTools/IBackgroundCategory.h"
#include "DaVinciMCTools/IDebugTool.h"
// local
#include "PrintBackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintBackgroundCategory
//
// 2005-12-16 : Luis Fernandez & Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrintBackgroundCategory>          s_factory ;
const        IAlgFactory& PrintBackgroundCategoryFactory = s_factory ;


PrintBackgroundCategory::bkgStat& PrintBackgroundCategory::bkgStat::operator++()
{
  ++countCndate;
  if( !tmpFlag ) ++countEvent;
  tmpFlag = true;
  return *this;
}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintBackgroundCategory::PrintBackgroundCategory( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_bkgCategory(0)
  , m_debug(0)
  , m_PrintTree(false)
  , m_DumpMCEvent(false)
  , m_acceptedevents(0)
  , m_candidates(0)
  , m_flipResult(false)
{
  // WARNING!
  // temp. should be defined in the BackgroundCategory tool.
  // In the mean time, every change in IBackgroundCategory enum should be made here as well
  m_cat[-1]   = "Undefined";
  m_cat[0]    = "Signal";
  m_cat[10]   = "QuasiSignal";
  m_cat[20]   = "FullyRecoPhysBkg";
  m_cat[30]   = "Reflection";
  m_cat[40]   = "PartRecoPhysBkg";
  m_cat[50]   = "LowMassBkg";
  m_cat[60]   = "Ghost";
  m_cat[70]   = "FromPV";
  m_cat[80]   = "AllFromSamePV";
  m_cat[100]  = "FromDifferentPV";
  m_cat[110]  = "bbar";
  m_cat[120]  = "ccbar";
  m_cat[130]  = "uds";
  m_cat[1000] = "LastGlobal";
  //end temp.

  // building the map object
  std::map<int, std::string>::iterator p;
  bkgStat tmp;
  for( p=m_cat.begin(); p!=m_cat.end(); ++p ){
    tmp.name = p->second;
    m_stats[p->first] = tmp;
  }

  declareProperty( "PrintTree",   m_PrintTree = false );
  declareProperty( "DumpMCEvent", m_DumpMCEvent = false );
  declareProperty( "FlipResult",  m_flipResult = false );
  declareProperty( "FilterCategoryNames",  m_stringCat );
  declareProperty( "FilterCategoryTypes",  m_intCat );

}
//=============================================================================
// Destructor
//=============================================================================
PrintBackgroundCategory::~PrintBackgroundCategory() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode PrintBackgroundCategory::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize();
  if (!sc) return sc;
  
  debug() << "==> Initialize" << endreq;

  m_bkgCategory = tool<IBackgroundCategory>("BackgroundCategory", this);
  if(!m_bkgCategory){
    fatal() << "Unable to retrieve BackgroundCategory tool" << endreq;
    return StatusCode::FAILURE;
  }  

  if(m_PrintTree || m_DumpMCEvent){
    m_debug = tool<IDebugTool>( "DebugTool", this );
    if( !m_debug ) {
    fatal() << "Unable to retrieve Debug tool" << endreq;
    return StatusCode::FAILURE;
    }
  }

  // Getting the filtered categories.
  bkgstats::iterator bi;
  std::map<std::string,int>::iterator rbi;
  std::vector<std::string>::iterator si;
  std::vector<int>::iterator ii;
  int counter = 0;
  
  std::map<std::string,int> revMap; // Building a reverse map with all categories.
  for( bi = m_stats.begin(); bi!=m_stats.end(); ++bi ){
    revMap[bi->second.name] = bi->first;
  }
  
  for( si = m_stringCat.begin(); si!= m_stringCat.end(); ++si ){
    std::string name = *si;
    rbi = revMap.find(name);
    if( rbi==revMap.end() ){
      fatal() << "Unknown background catergory '" << name << "'." << endreq;
      return StatusCode::FAILURE;
    }
    bi = m_stats.find( rbi->second );
    if( bi==m_stats.end() ){
      fatal() << "Unknown background type " << rbi->second << endreq;
      return StatusCode::FAILURE;
    }
    bi->second.filterIt = 1;
    ++counter;
  }
  for( ii = m_intCat.begin(); ii!=m_intCat.end(); ++ii ){
    bi = m_stats.find( *ii );
    if( bi==m_stats.end() ){
      fatal() <<"Unknown background type " << *ii << endreq;
      return StatusCode::FAILURE;
    }
    if( bi->second.filterIt > 0 )
      info() << "Redundant call on category '" << bi->second.name <<"'." 
             << endreq;
    bi->second.filterIt = 1;
    ++counter;
  }

  // Looping on all the categories. If no category has been defined,
  // setting the result to 1 && flipResult. If at least 1 cat is
  // defined, setting it to 0. Categories set by the user are
  // only modified by the FlipResult property.
  bool anyDefined = true;
  if( m_intCat.empty() && m_stringCat.empty() ) anyDefined = false;

  for( bi=m_stats.begin(); bi!=m_stats.end(); ++bi ){
    int& val = bi->second.filterIt;
    if( anyDefined ) val = m_flipResult ? !val : val;
    else val = m_flipResult ? 0 : 1;
  }
  
  // summary of the actual settings.
  info() << "PrintBackgroundCategory initialized with following categories:" 
         << endreq;
  if( m_flipResult ) 
    info() << "flipResult allready taken into account." << endreq;
  
  for ( bi = m_stats.begin() ; bi != m_stats.end(); ++bi){
    info() << "category " << bi->first << " (" << bi->second.name 
           << "). Filter it: " << bool(bi->second.filterIt) << endreq;
  }
  
  return StatusCode::SUCCESS;
}


//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintBackgroundCategory::execute() {

  debug() << "==> Execute" << endreq;

  ParticleVector parts = desktop()->particles();
  ParticleVector::iterator iparts;

  bool accepted = false;

  for( bkgstats::iterator it = m_stats.begin(); it!=m_stats.end(); ++it ){
    it->second.reset();
  }
  
  for(iparts = parts.begin(); iparts != parts.end(); ++iparts){
    ++m_candidates;
    IBackgroundCategory::categories cat = m_bkgCategory->category(*iparts);
    if( m_stats[cat].filterIt == 0 ){ // reject the event.
      info() << "Found a candidate of category " << cat << " (" 
             << m_stats[cat].name << ")." << endreq;
    }
    else{ //accept the event.
      info() << "Result of BackgroundCategory is: " << cat
              << " (" << m_stats[cat].name << ")." << endreq;
      if (!increment_stats(cat)) return StatusCode::FAILURE ;
      accepted = true; // one a least is accepted.
      
      // Print the reconstructed tree
      if(m_PrintTree){
        m_debug->printTree(*iparts);
      } // m_PrintTree
      
    }
  }

  if( accepted ){
    debug() << "will keep the event." << endreq;
    ++m_acceptedevents;  
  }
  else  debug() << "will reject the event." << endreq;

  if( parts.empty() ){
    accepted = true; // this behavior could be changed in futur release though...
    debug() << "No particles found for BackgroundCategory, still accepting the event." << endreq;

  } else {
    // Dump whole MC event
    if( accepted && m_DumpMCEvent){
      MCParticles* kmcparts = get<MCParticles>(MCParticleLocation::Default);
      if(!kmcparts){
        fatal() << "Unable to find MC particles at '"
                << MCParticleLocation::Default << "'" << endreq;
        return StatusCode::FAILURE;
      }
      std::vector<MCParticle*> mcparts(kmcparts->begin(), kmcparts->end());
      info() << "printEventAsTree(mcparts)" << std::flush << endreq;
      m_debug->printEventAsTree(mcparts);
    } // m_DumpMCEvent
  }
  
  if( accepted ) setFilterPassed(true);
  else           setFilterPassed(false);

  return StatusCode::SUCCESS;
};

//=============================================================================
// stats
//=============================================================================
StatusCode PrintBackgroundCategory::increment_stats(IBackgroundCategory::categories cat) {
  debug() << "Incrementing category " << cat << endreq ;
  ++(m_stats[cat]);
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode PrintBackgroundCategory::finalize() {
  return print_stats();
}
//=============================================================================
// print stats
//=============================================================================
StatusCode PrintBackgroundCategory::print_stats() {
  bkgstats::iterator it;
  
  info() << "Background Summary:";
  if ( !m_acceptedevents ){
    info() << " no event." << endreq;
    return  StatusCode::SUCCESS;
  }
  
  info() << " total " << m_candidates << " candidates in " 
         << m_acceptedevents << " accepted events." << endreq;
  for ( it = m_stats.begin() ; it != m_stats.end(); ++it){
    if( it->second.countEvent ){
      info() << "Background " << it->first << " (" << it->second.name 
             << ") has " << it->second.countCndate << " candidates in " 
             << it->second.countEvent << " events accepted by this category." << endreq;
    }
  }
  return  StatusCode::SUCCESS;
}
