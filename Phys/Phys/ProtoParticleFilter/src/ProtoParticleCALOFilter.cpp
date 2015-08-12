
//-----------------------------------------------------------------------------
/** @file ProtoParticleCALOFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleCALOFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "ProtoParticleCALOFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleCALOFilter
//
// 2006-05-03 : Chris Jones  Christoper.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleCALOFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleCALOFilter::ProtoParticleCALOFilter( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : ProtoParticleRICHFilter ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleCALOFilter::~ProtoParticleCALOFilter() { }

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleCALOFilter::createCut( const std::string & tag,
                                    const std::string & delim,
                                    const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ProtoParticleRICHFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;

  // Otherwise, cut data is CALO specific, so treat here

  // True to get a double from the value
  double cut_value = 0;
  if ( ! stringToDouble ( value, cut_value ) ) return NULL;
  
  // Create a new Cut object
  ProtoParticleSelection::SingleVariableCut * dllcut = new ProtoParticleSelection::SingleVariableCut();

  // set cut properties
  // Cut delimiter type
  dllcut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
  // cut value
  dllcut->setCutValue    ( cut_value                                     );
  // cut description
  dllcut->setDescription ( "Calo Cut : "+tag+" "+delim+" "+value         );

  // automatic setting for ANY possible calo tag
  bool ok = false;
  for ( int i = 300; i < 400; ++i ) // select the CALO range
  { 
    std::ostringstream var("");
    var << (LHCb::ProtoParticle::additionalInfo) i;
    if( std::string::npos != var.str().find("ERROR") )continue;
    std::string uTag( tag );
    std::string uVar( var.str() );
    std::transform( tag.begin() , tag.end() , uTag.begin () , ::toupper ) ;
    std::string v = var.str();
    std::transform( v.begin() , v.end() , uVar.begin () , ::toupper ) ;
    if( uTag == uVar)
    {
      ok = true;
      dllcut->setVariable( (LHCb::ProtoParticle::additionalInfo) i );
      break;
    }
  }

  if ( !ok )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Unknown tag " << tag << endreq;
    ++counter("Unknown ProtoParticleFilter tag "+tag);
    delete dllcut;
    dllcut = NULL;
  }
  
  if ( msgLevel(MSG::DEBUG) && dllcut )
  {
    debug() << "  -> Created new DLLCut : "
            << tag << " " << delim << " " << dllcut->cutValue() << endreq;
  }
  
  return dllcut;
}

//=============================================================================
