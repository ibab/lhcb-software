
//-----------------------------------------------------------------------------
/** @file RichMarkovRingFinderAlg.cpp
 *
 *  Header file for algorithm : RichMarkovRingFinderAlg
 *
 *  CVS Log :-
 *  $Id: RichMarkovRingFinderAlg.cpp,v 1.19 2005-08-09 13:17:42 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2005-08-09
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "RichMarkovRingFinderAlg.h"

// Declaration of the Algorithm Factory
static const  AlgFactory<RichMarkovRingFinderAlg>          s_factory ;
const        IAlgFactory& RichMarkovRingFinderAlgFactory = s_factory ; 

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichMarkovRingFinderAlg::RichMarkovRingFinderAlg( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : RichRecMoniAlgBase ( name , pSvcLocator )
{

}
//=============================================================================
// Destructor
//=============================================================================
RichMarkovRingFinderAlg::~RichMarkovRingFinderAlg() {}; 

//=============================================================================
// Initialization
//=============================================================================
StatusCode RichMarkovRingFinderAlg::initialize() 
{
  const StatusCode sc = RichRecMoniAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode RichMarkovRingFinderAlg::execute() 
{

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode RichMarkovRingFinderAlg::finalize() 
{

  return RichRecMoniAlgBase::finalize(); 
}

//=============================================================================
