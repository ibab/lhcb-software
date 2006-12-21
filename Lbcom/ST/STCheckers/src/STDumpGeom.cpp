// $Id: STDumpGeom.cpp,v 1.3 2006-12-21 17:54:48 jvantilb Exp $

// Gaudi
#include "GaudiKernel/AlgFactory.h"

// xml geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTSector.h"

// local
#include "STDumpGeom.h"

DECLARE_ALGORITHM_FACTORY( STDumpGeom );

//--------------------------------------------------------------------
//
//  STDumpGeom : Prints out the full geometry tree
//
//--------------------------------------------------------------------

STDumpGeom::STDumpGeom( const std::string& name, 
                        ISvcLocator* pSvcLocator ) :
  GaudiAlgorithm(name, pSvcLocator)
{
  // constructer
  declareProperty("DetType",  m_detType  = "TT"  );
  declareProperty("FullDetail", m_fullDetail = false );
}

STDumpGeom::~STDumpGeom()
{
  // destructer
}

StatusCode STDumpGeom::initialize()
{
  // initialize  
  StatusCode sc = GaudiAlgorithm::initialize();
  if (sc.isFailure()) return Error("Failed to initialize", sc);

  // Print out the full tree
  DeSTDetector* det =getDet<DeSTDetector>(DeSTDetLocation::location(m_detType));
  if (det != 0 ){
    std::cout << "Printing out the full geometry tree of\n"
              << det->name() << std::endl;
    children("", det);
  }

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
