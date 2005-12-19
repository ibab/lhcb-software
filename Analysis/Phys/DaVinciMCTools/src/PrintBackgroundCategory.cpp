// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// from DaVinci
#include "DaVinciMCTools/IBackgroundCategory.h"

// local
#include "PrintBackgroundCategory.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrintBackgroundCategory
//
// 2005-12-16 : Luis Fernandez
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrintBackgroundCategory>          s_factory ;
const        IAlgFactory& PrintBackgroundCategoryFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrintBackgroundCategory::PrintBackgroundCategory( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  , m_bkgCategory(0)
  , m_entries(0)
{

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

  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrintBackgroundCategory::execute() {

  debug() << "==> Execute" << endreq;

  ParticleVector parts = desktop()->particles();
  ParticleVector::iterator iparts;

  for(iparts = parts.begin(); iparts != parts.end(); ++iparts){
    IBackgroundCategory::categories cat = m_bkgCategory->category(*iparts);
    info() << "Result of BackgroundCategory is: " << cat << endreq;
    if (!increment_stats(cat)) return StatusCode::FAILURE ;
  }
  if(!parts.empty()) setFilterPassed(true);
  else{
    setFilterPassed(false);  
    debug() << "No particles found for BackgroundCategory" << endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
// stats
//=============================================================================
StatusCode PrintBackgroundCategory::increment_stats(IBackgroundCategory::categories cat) {
  bkgstats::iterator it = m_stats.find(cat);
  if (it==m_stats.end()){
    debug() << "Category " << cat << " is new" << endmsg ;
    m_stats[cat] = 1 ;
  } else {
    debug() << "Incrementing category " << cat << endmsg ;
    (m_stats[cat])++;
  }
  m_entries++;
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
  if (m_entries==0) return  StatusCode::SUCCESS;
  for ( bkgstats::iterator it = m_stats.begin() ; it != m_stats.end(); ++it){
    double f = 100.*it->second/m_entries;
    info() << "Background " << it->first << " seen " << it->second << " times ("
           << f << "%)" << endmsg ;
  }
  return  StatusCode::SUCCESS;
}
