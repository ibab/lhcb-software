// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlCnvSvc.cpp,v 1.7 2001-11-20 15:22:24 sponce Exp $

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
// Create an XML address
// -----------------------------------------------------------------------
StatusCode XmlCnvSvc::createAddress(unsigned char svc_type,
                                    const CLID& clid,
                                    const std::string* par, 
                                    const unsigned long* /*ip*/,
                                    IOpaqueAddress*& refpAddress) {
  if (svc_type == repSvcType()) {
    refpAddress = new GenericAddress (XML_StorageType,
                                      clid,
                                      par[0],
                                      par[1]);
    if (0 != refpAddress) {
      return StatusCode::SUCCESS;
    }
  }
  return StatusCode :: FAILURE;
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
  if (0 != m_parserSvc) {
    return m_parserSvc->parseString (source);
  }
  DOM_Document null_result;
  MsgStream log (msgSvc(), "XmlCnvSvc");
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
double XmlCnvSvc::eval (const std::string& expr, bool check) const {
  return eval (expr.c_str(), check);
}


// -----------------------------------------------------------------------
// Evaluate a numerical expresion
// -----------------------------------------------------------------------
double XmlCnvSvc::eval (const char* expr, bool check) const {
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
