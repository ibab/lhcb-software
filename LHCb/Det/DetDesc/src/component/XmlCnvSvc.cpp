// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlCnvSvc.cpp,v 1.11 2002-05-02 12:05:50 sponce Exp $

// Include Files
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>

#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SvcFactory.h"
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IConverter.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GenericAddress.h"

#include "XmlCnvSvc.h"
#include "XmlParserSvc.h"


// -----------------------------------------------------------------------
// Instantiation of a static factory class used by clients to create
// instances of this service
// -----------------------------------------------------------------------
static SvcFactory<XmlCnvSvc> xmlcnvsvc_factory;
const ISvcFactory& XmlCnvSvcFactory = xmlcnvsvc_factory;


// -----------------------------------------------------------------------
// Standard Constructor
// -----------------------------------------------------------------------
XmlCnvSvc::XmlCnvSvc (const std::string& name, ISvcLocator* svc) :
  ConversionSvc(name, svc, XML_StorageType) {
  
  // gets the AllowGenericConversion property value
  declareProperty ("AllowGenericConversion", m_genericConversion = false);

  // gets the DtdLocation property value
  declareProperty ("DtdLocation", m_dtdLocation = "");
}


// -----------------------------------------------------------------------
// Standard Destructor
// -----------------------------------------------------------------------
XmlCnvSvc::~XmlCnvSvc() {
}


// -----------------------------------------------------------------------
// Initialize the service.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::initialize() {
  // Before anything we have to initialize grand mother
  StatusCode status = ConversionSvc::initialize();

  // Service MUST be initialized BEFORE!
  MsgStream log (msgSvc(), "XmlCnvSvc");

  if (!status.isSuccess()) {
    return status;  
  }

  // Initialize the xerces system
  try  {
    XMLPlatformUtils::Initialize();
  } catch (const XMLException& toCatch) {
    char *message = XMLString::transcode (toCatch.getMessage());
    log << MSG::FATAL << "Error during initialization! :\n"
        << message << endreq;
    status = StatusCode::FAILURE;
    delete [] message;
  } 
  
  // creation of a parser service
  status = serviceLocator()->service("XmlParserSvc", m_parserSvc, true);
  if (status.isFailure()) {
    return status;
  }
  
  //setProperties();

  // Initialize numerical expressions parser with the standard math functions
  // and the system of units used by Gaudi (Geant4)
  m_xp.setStdMath();
  m_xp.setSystemOfUnits (1.e+3,
                         1./1.60217733e-25,
                         1.e+9,
                         1./1.60217733e-10,
                         1.0,
                         1.0,
                         1.0);
  return status;
}


// -----------------------------------------------------------------------
// Stop the service.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::finalize() {
  m_parserSvc->release();
  m_parserSvc = 0;
  return ConversionSvc::finalize();
}


// -----------------------------------------------------------------------
// Query interface
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::queryInterface(const IID& riid, void** ppvInterface) {
  if (IID_IXmlSvc.versionMatch(riid))  {
    *ppvInterface = (IXmlSvc*)this;
  } else {
    // Interface is not directly availible: try out a base class
    return ConversionSvc::queryInterface(riid, ppvInterface);
  }
  addRef();
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Create an XML address.
// Converters interpret XML addresses according to the following rules:
// par[0] is an XML file name or an XML string;
// par[1] is the name of the requested element in the XML data;
// ipar[0] is 0 or 1 for files and strings respectively.
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::createAddress(unsigned char svc_type,
                                    const CLID& clid,
                                    const std::string* par, 
                                    const unsigned long* ipar,
                                    IOpaqueAddress*& refpAddress) 
{
  // First check that requested address is of type XML_StorageType
  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Create an XML address" << endreq;
  if( XML_StorageType != svc_type ) {
    log << MSG::ERROR 
	<< "Cannot create addresses of type " << (int)svc_type 
	<< " which is different from " << (int)XML_StorageType 
	<< endreq;
    return StatusCode::FAILURE;
  }

  // Par[0] is an XML file name or an XML string.
  std::string source = par[0];

  // Par[1] is the name of the requested XML element.
  std::string entryName = par[1];

  // Ipar[0] is 0 for XML files and 1 for XML strings.
  unsigned int isString;

  // To avoid the need of two separate address creators with two different
  // storage types for files and strings, this method also creates XML string 
  // addresses when par[0] begins by "<?xml": in this case ipar[0] is ignored.
  unsigned int pos = source.find_first_not_of(" ");
  if( 0 < pos && pos < source.length() ) source.erase( 0, pos );
  if( source.find("<?xml") == 0 ) {
    isString = 1;
    log << MSG::VERBOSE 
	<< "XML source beginning by \"<?xml\" is interpreted"
	<< " as an XML string" << endreq;
  } else {
    isString = ipar[0];
    if( isString == 0 ) {
      log << MSG::VERBOSE << "XML source is an XML file name" << endreq;
    } else if( isString == 1 ) { 
      log << MSG::VERBOSE << "XML source is an XML string" << endreq;
    } else {
      log << MSG::ERROR 
	  << "Cannot create address: invalid ipar[0] value = "
	  << ipar[0] << endreq;
      return StatusCode::FAILURE;
    }
  }
  
  // Now create the address
  refpAddress = new GenericAddress( XML_StorageType,
				    clid,
				    source,
				    entryName,
				    isString );
  return StatusCode::SUCCESS;
}


// -----------------------------------------------------------------------
// Parses an Xml file and provides the DOM tree representing it
// -----------------------------------------------------------------------
DOM_Document XmlCnvSvc::parse (const char* fileName) {
  if (0 != m_parserSvc) {
    return m_parserSvc->parse(fileName);
  }
  DOM_Document null_result;
  MsgStream log (msgSvc(), "XmlCnvSvc");
  log << MSG::DEBUG << "null result returned in parse" << endreq;
  return null_result;
}


// -----------------------------------------------------------------------
// Parses an Xml file and provides the DOM tree representing it
// -----------------------------------------------------------------------
DOM_Document XmlCnvSvc::parseString (std::string source) {
  MsgStream log (msgSvc(), "XmlCnvSvc");

  // First prepend the proper DTD path where appropriate
  // Only one "relpath/file.dtd" or 'relpath/file.dtd' is expected in string
  if( m_dtdLocation != "" ) {
    unsigned int dtdPos = source.find( ".dtd" );
    if( dtdPos < source.length() ) {
      log << MSG::VERBOSE 
          << "Set correct DTD location in the string to be parsed" << endreq;
      unsigned int quotePos;
      if( source[dtdPos+4] == '\'' ) {
        quotePos = source.substr(0,dtdPos).rfind("\'");
        source.insert( quotePos+1, m_dtdLocation+"/" );
        log << MSG::VERBOSE << "DTD literal is now: " 
            << source.substr(quotePos,dtdPos+6-quotePos+m_dtdLocation.length())
            << endreq;
      } else if ( source[dtdPos+4] == '\"' ) {
        quotePos = source.substr(0,dtdPos).rfind("\"");
        source.insert( quotePos+1, m_dtdLocation+"/" );
        log << MSG::VERBOSE << "DTD literal is now: " 
            << source.substr(quotePos,dtdPos+6-quotePos+m_dtdLocation.length())
            << endreq;
      } else {
        log << MSG::VERBOSE
            << "Bad DTD literal in the string to be parsed: do nothing" 
            << endreq;
      }
    }
  }

  // Then feed the string to the XML parser
  if (0 != m_parserSvc) {
    return m_parserSvc->parseString (source);
  }
  DOM_Document null_result;
  log << MSG::DEBUG << "null result returned in parseString" << endreq;
  return null_result;

}


// -----------------------------------------------------------------------
// clears the cache of previously parsed xml files
// -----------------------------------------------------------------------
void XmlCnvSvc::clearCache() {
  if (0 != m_parserSvc) {
    m_parserSvc->clearCache();
    return;
  }
}


// -----------------------------------------------------------------------
// Evaluate a numerical expresion
// -----------------------------------------------------------------------
double XmlCnvSvc::eval (const std::string& expr, bool check) {
  return eval (expr.c_str(), check);
}


// -----------------------------------------------------------------------
// Evaluate a numerical expresion
// -----------------------------------------------------------------------
double XmlCnvSvc::eval (const char* expr, bool check) {
  MsgStream log (msgSvc(), name());

  // Check if it is needed to be a dimention number
  if (check) {
    // find the last '*' and check that there is a unit name after it
    std::string e(expr);
    unsigned int pos = e.find_last_of('*');
    // set the unit to what come after the '*' or the complete expresion
    // std::string unit = (pos == -1 ) ? e : e.substr(pos + 1);
    std::string unit = ( pos == std::string::npos ) ? e : e.substr(pos + 1);
    // remove leading blanks
    pos = unit.find_first_not_of(' ');
    // remove leading '-' if one
    if (unit[pos] == '-') pos += 1;
    unit = unit.substr(pos);
    // check if what is left is not empty and alphabetic character
    if (unit.size() == 0 || !isalpha(unit[0])) {
      log << MSG::WARNING << "Expression requires units ["
          << expr << "]" << endreq;
    }
  }
  // Call the CLHEP Evaluator
  double value = m_xp.evaluate( expr );
  std::string errtxt;
  switch (m_xp.status()) {
  case HepTool::Evaluator::OK :
    return value;
  case HepTool::Evaluator::ERROR_NOT_A_NAME :
    errtxt = "NOT_A_NAME";
    break;
  case HepTool::Evaluator::ERROR_SYNTAX_ERROR :
    errtxt = "SYNTAX_ERROR";
    break;
  case HepTool::Evaluator::ERROR_UNPAIRED_PARENTHESIS :
    errtxt = "UNPAIRED_PARENTHESIS";
    break;
  case HepTool::Evaluator::ERROR_UNEXPECTED_SYMBOL :
    errtxt = "UNEXPECTED_SYMBOL";
    break;
  case HepTool::Evaluator::ERROR_UNKNOWN_VARIABLE :
    errtxt = "UNKNOWN_VARIABLE";
    break;
  case HepTool::Evaluator::ERROR_UNKNOWN_FUNCTION :
    errtxt = "UNKNOWN_FUNCTION";
    break;
  case HepTool::Evaluator::ERROR_EMPTY_PARAMETER :
    errtxt = "EMPTY_PARAMETER";
    break;
  case HepTool::Evaluator::ERROR_CALCULATION_ERROR :
    errtxt = "CALCULATION_ERROR";
    break;
  default :
    errtxt = "UNKNOWN_ERROR";
    break;
  }
  log << MSG::ERROR << "Expresion evaluation error: " << errtxt << endreq;
  log << MSG::ERROR << "[" << expr << "]" << endreq;
  log << MSG::ERROR << " ";
  for (int i = 0; i < m_xp.error_position(); i++) {
    log << " "; log << "^" << endreq;
  }
  return 0;
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const std::string& name,
                              const std::string& value) {
  return addParameter (name.c_str(), value.c_str());
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const char* name, const char* expr) {
  m_xp.setVariable (name, expr);
  if (m_xp.status() == HepTool::Evaluator::OK) return true;
  else return false;
}


// -----------------------------------------------------------------------
// addParameter into expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::addParameter (const char* name, double value) {
  m_xp.setVariable (name, value);
  if (m_xp.status() == HepTool::Evaluator::OK) {
    return true;
  } else {
    return false;
  }
}


// -----------------------------------------------------------------------
// removeParameter from expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::removeParameter (const std::string& name) {
  return removeParameter (name.c_str());
}


// -----------------------------------------------------------------------
// removeParameter from expresion parser
// -----------------------------------------------------------------------
bool XmlCnvSvc::removeParameter (const char* name) {
  m_xp.removeVariable (name);
  if (m_xp.status() == HepTool::Evaluator::OK) {
    return true;
  } else {
    return false;
  }
}
