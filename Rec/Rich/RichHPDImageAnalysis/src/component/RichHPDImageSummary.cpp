
// Local
#include "RichHPDImageSummary.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichHPDImageSummary
//
// 2010-03-16 : Thomas BLAKE
//-----------------------------------------------------------------------------

using namespace Rich::HPDImage;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================

Summary::Summary( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : SummaryBase ( name , pSvcLocator ) { }

//=============================================================================
// Destructor
//=============================================================================

Summary::~Summary() { }

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( Summary )

//=============================================================================
