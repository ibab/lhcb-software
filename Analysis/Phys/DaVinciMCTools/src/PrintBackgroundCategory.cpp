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
  }
  if(!parts.empty()) setFilterPassed(true);
  else{
    setFilterPassed(false);  
    debug() << "No particles found for BackgroundCategory" << endreq;
  }

  return StatusCode::SUCCESS;
};


//=============================================================================
