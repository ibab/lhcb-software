//$Id: Condition.cpp,v 1.14 2008-02-22 12:12:12 marcocle Exp $
#include <string> 

#include "DetDesc/Condition.h"

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/IRegistry.h"

//---------------------------------------------------------------------------

/// Default constructor
Condition::Condition() : ParamValidDataObject() {}

//---------------------------------------------------------------------------

/// Copy constructor
Condition::Condition (const Condition& obj)
  : IValidity(), ParamValidDataObject (obj) {}

//---------------------------------------------------------------------------

/// Update using another instance of this class: deep copy all 
/// contents, except for the properties of a generic DataObject
void Condition::update ( ValidDataObject& obj )
{
  /// See the implementation of ParamValidDataObject::update
  /// for a real implementation.
  ParamValidDataObject::update ( obj );
}

//=========================================================================
//  Prepare an XML string representing the condition
//=========================================================================
std::string Condition::toXml(std::string name, bool header, int precision) const{
  std::ostringstream xml;
  if (header) {
    // XML header
    xml << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"
        << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">";
    // DDDB open
    xml << "<DDDB>";
    // condition open
  }
  xml << "<condition classID=\"" << this->clID() << "\" name=\"";
  
  if (name.empty()) {
    if (registry()){
      name = registry()->name();
    } else {
      name = "Condition";
    }
  }
  if ( name[0] == '/' ) {
    name = name.substr(1);
  }
  xml << name << "\">";

  std::vector<std::string> pars;
  std::vector<std::string>::const_iterator i;
  // loop over parameters
  pars = paramNames();
  for ( i = pars.begin(); i != pars.end(); ++i ){
    xml << paramToString(*i, precision);
  }
  
  // condition close
  xml << "</condition>";
  if (header) {
    // DDDB close
    xml << "</DDDB>";
  }
  
  return xml.str();
}
//---------------------------------------------------------------------------

/// Destructor
Condition::~Condition() {}

//----------------------------------------------------------------------------

/// initialize
StatusCode Condition::initialize() {
  // this is a default implementation that does nothing.
  // it is up to the user to override this in a child of Condition
  return StatusCode::SUCCESS;
}

//----------------------------------------------------------------------------
