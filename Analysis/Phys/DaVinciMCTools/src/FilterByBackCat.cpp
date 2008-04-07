// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "FilterByBackCat.h"

//-----------------------------------------------------------------------------
// Implementation file for class : FilterByBackCat
//
// 30/03/2008 : V. Gligorov
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( FilterByBackCat );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
FilterByBackCat::FilterByBackCat( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : FilterCriterionBase ( type, name , parent ) {

  // declare additional interface
  declareInterface<IFilterCriterion>(this);

  // declare properties
  declareProperty( "AllowedCategories", m_bkgCat);  
}

//=============================================================================
// initialize() method
//=============================================================================
StatusCode FilterByBackCat::initialize() 
{
  // initialise base class
  const StatusCode sc = FilterCriterionBase::initialize() ;
  if (!sc) return sc ;

  m_bkg = tool<IBackgroundCategory>( "BackgroundCategory", this );

  debug() << ">>>   FilterByBackCat::initialize() "<< endreq;
  debug() << ">>>   Cuts are " << endreq;
  debug() << ">>>   Allowed Categories  = ";
  for (m_bkgCatIterator = m_bkgCat.begin(); m_bkgCatIterator != m_bkgCat.end(); ++m_bkgCatIterator){
	debug() << (*m_bkgCatIterator) << " ";      
  }
  debug() << endreq;
  if ( m_bkgCat.size() > 0 ) setActive();
  
  return sc;
}

//=============================================================================
// Test if filter is satisfied
//=============================================================================
bool FilterByBackCat::testParticle( const LHCb::Particle* const & part ) 
{

  int thisPartCat;

  if( !part->isBasicParticle() ){

    thisPartCat  = (int)(m_bkg->category( part ));

  }

  bool ok = false;

  for (m_bkgCatIterator = m_bkgCat.begin(); m_bkgCatIterator != m_bkgCat.end(); ++m_bkgCatIterator){
  	if (thisPartCat == *m_bkgCatIterator) { 
		ok = true;
		break;
	}
  }

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << "BackCat = " << thisPartCat << endreq; 
  }
  return ok;
}

//=============================================================================
