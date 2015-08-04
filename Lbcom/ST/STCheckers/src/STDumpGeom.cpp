// xml geometry
#include "STDet/DeSTDetector.h"


// local
#include "STDumpGeom.h"

DECLARE_ALGORITHM_FACTORY( STDumpGeom )

//--------------------------------------------------------------------
//
//  STDumpGeom : Prints out the full geometry tree
//
//--------------------------------------------------------------------

STDumpGeom::STDumpGeom( const std::string& name, 
                        ISvcLocator* pSvcLocator ) :
  ST::AlgBase(name, pSvcLocator)
{
  // constructer
  declareProperty("FullDetail", m_fullDetail = false );
  setForcedInit();
}

STDumpGeom::~STDumpGeom()
{
  // destructer
}

StatusCode STDumpGeom::initialize()
{
  // initialize  
  StatusCode sc = ST::AlgBase::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Print out the full tree
  std::cout << "Printing out the full geometry tree of\n"
            << tracker()->name() << std::endl;
  children("", tracker());
  
  return StatusCode::SUCCESS;
}

StatusCode STDumpGeom::execute()
{
  // execute once per event
  return StatusCode::SUCCESS;
}

void STDumpGeom::children(std::string indent, DetectorElement* parent)
{
  if( m_fullDetail ) {
    // Print the full details of this DetectorElement
    std::cout << parent << std::endl;
  } else {
    // Print out only the DetectorElement's name
    std::cout << indent << parent->DataObject::name() << std::endl; 
  }

  indent += "  ";
  IDetectorElement::IDEContainer::const_iterator iChild = parent->childBegin();
  for ( ; iChild != parent->childEnd(); ++iChild ) {
    DetectorElement* tElem = dynamic_cast<DetectorElement*>(*iChild);
    children(indent, tElem);
  }  
}
