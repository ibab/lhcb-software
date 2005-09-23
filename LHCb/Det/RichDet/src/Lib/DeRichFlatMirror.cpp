
/** @file DeRichFlatMirror.cpp
 *
 *  Implementation file for detector description class : DeRichFlatMirror
 *
 *  $Id: DeRichFlatMirror.cpp,v 1.12 2005-09-23 15:27:28 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */

#define DERICHFLATMIRROR_CPP

// Include files
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/RegistryEntry.h"

// CLHEP files
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Geometry/Transform3D.h"
#include "CLHEP/Vector/Rotation.h"
#include "CLHEP/Geometry/Normal3D.h"

/// Detector description classes
#include "DetDesc/IGeometryInfo.h"
#include "DetDesc/SolidBox.h"
#include "DetDesc/Surface.h"

#include "RichDet/DeRichFlatMirror.h"

//----------------------------------------------------------------------------

const CLID& CLID_DeRichFlatMirror = 12031;  // User defined

// Standard Constructor
DeRichFlatMirror::DeRichFlatMirror()
  : m_alignmentConstantX(0.0), m_alignmentConstantY(0.0), m_mirrorNumber(-1)
{}

// Standard Destructor
DeRichFlatMirror::~DeRichFlatMirror() {}

// Retrieve Pointer to class defininition structure
const CLID& DeRichFlatMirror::classID() {
  return CLID_DeRichFlatMirror;
}


StatusCode DeRichFlatMirror::initialize()
{
  MsgStream msg( msgSvc(), "DeRichFlatMirror" );
  msg << MSG::DEBUG << "Initializing flat mirror" << endreq;
  //this->printOut(msg);

  // find if this mirror is in Rich1 or Rich2
  const std::string::size_type pos = name().find("Rich");
  if ( std::string::npos != pos ) {
    m_name = name().substr(pos);
    std::string richNum = m_name.substr(4,1);
    if ( richNum == "1" )
      m_rich = Rich::Rich1;
    else
      if ( richNum == "2")
        m_rich = Rich::Rich2;
      else {
        msg << MSG::FATAL<<"Could not identify Rich (1/2=="<<richNum<<" )"<<endmsg;
        return StatusCode::FAILURE;
      }
  } else {
    m_name = "DeRichSphMirror_NO_NAME";
    msg << MSG::FATAL << "A spherical mirror without a number!" << endmsg;
    return StatusCode::FAILURE;
  }

  m_solid = geometry()->lvolume()->solid();

  m_alignmentConstantX = param<double>( "AlignmentConstantX" );
  m_alignmentConstantY = param<double>( "AlignmentConstantY" );

  const HepRotateX3D alignX(-m_alignmentConstantY);
  const HepRotateY3D alignY(m_alignmentConstantX);

  // get the z half length, to find the centre on the mirror surface
  // where reflection will take place
  const ISolid* mysolid = geometry()->lvolume()->solid();
  const SolidBox* myBox = dynamic_cast<const SolidBox*>(mysolid);
  const double centreOnSurfaceZ = myBox->zHalfLength();

  HepPoint3D localCentre(0.0, 0.0, centreOnSurfaceZ);
  // in global coordinates
  m_mirrorCentre = geometry()->toGlobal(localCentre);

  HepNormal3D localNormal(0.0, 0.0, 1.0);

  // Rotate local normal dependig on the alignment constants
  // Rotating around X changes Y, while leaving X unchanged
  HepNormal3D alignedLocalNormal = localNormal;
  alignedLocalNormal.transform(alignY);
  alignedLocalNormal.transform(alignX);

  // go back to the global coord system.
  const HepTransform3D localToGlobal = geometry()->matrixInv();
  m_normalVector = alignedLocalNormal.transform(localToGlobal);

  // create the mirror plane
  m_mirrorPlane = HepPlane3D(m_normalVector, m_mirrorCentre);
  msg << MSG::DEBUG << "mirrorPlane " << m_mirrorPlane << endmsg;
  
  // extract mirror number from detector element name
  const std::string::size_type pos2 = name().find(':');
  if( std::string::npos == pos2 ) {
    msg << MSG::FATAL << "A mirror without a number!" << endmsg;
    return StatusCode::FAILURE;
  }
  m_mirrorNumber = atoi( name().substr(pos2 + 1).c_str() );
  const std::string  mirNumString = name().substr(pos2+1);

  msg << MSG::DEBUG << "Mirror #" << m_mirrorNumber
      << " Centre (on reflective surface) " << m_mirrorCentre  << endmsg;
  msg << MSG::DEBUG <<"Normal vector " << m_normalVector << endmsg;

  // find surface properties
  std::string surfLocation, flatMirrorName, surfName;
  if ( m_rich == Rich::Rich1 ) {
    surfLocation = "/dd/Geometry/Rich1/Rich1Surfaces";
    flatMirrorName = "Mirror2";
    surfName = ":"+mirNumString;
  }
  else{
    surfLocation = "/dd/Geometry/Rich2/Rich2Surfaces";
    flatMirrorName = "FlatMirror";
    surfName = "Seg"+mirNumString;
  }

  bool foundSurface( false );
  SmartDataPtr<DataObject> rich2SurfCat(dataSvc(),surfLocation);
  DataSvcHelpers::RegistryEntry *rich2Reg = dynamic_cast<DataSvcHelpers::RegistryEntry *>
    (rich2SurfCat->registry());
  IRegistry* storeReg = 0;

  // find the surface in the registry
  for (DataSvcHelpers::RegistryEntry::Iterator child = rich2Reg->begin();
       (child != rich2Reg->end() && !foundSurface); ++child){
    // child is a const_iterator of vector<IRegistry*>
    const std::string::size_type pos3 = (*child)->name().find(flatMirrorName);
    if ( std::string::npos != pos3 ) {
      const std::string::size_type pos4 = (*child)->name().find(surfName);
      if ( std::string::npos != pos4 ) {
        storeReg = (*child);
        foundSurface = true;
      }
    }
  }

  bool foundRefl( false );
  // get the surface, get the tabulated properties and find REFLECTIVITY
  if ( foundSurface ) {
    SmartDataPtr<DataObject> obj (dataSvc(), storeReg->identifier());
    DataObject* pObj = obj;
    msg << MSG::DEBUG << "Dynamic cast to surface " << obj->name() << endmsg;
    Surface* surf = dynamic_cast<Surface*> (pObj);
    const Surface::Tables surfTabProp = surf->tabulatedProperties();
    for (Surface::Tables::const_iterator table_iter = surfTabProp.begin();
         table_iter != surfTabProp.end(); ++table_iter) {
      if ( (*table_iter)->type() == "REFLECTIVITY" ) {
        m_reflectivity = (*table_iter);
        foundRefl = true;
        break;
      }
    }
    if ( !foundRefl ) {
      msg << MSG::DEBUG <<"Could not find REFLECTIVITY "<< surf->name() << endmsg;
      return StatusCode::FAILURE;
    }
  }
  else {
    msg << MSG::DEBUG <<"Could not find surface for mirror "<< myName() << endmsg;
    return StatusCode::FAILURE;
  }

  msg << MSG::DEBUG << "Reflectivity is from TabProp "
      << m_reflectivity->name() << endmsg;

  msg << MSG::DEBUG <<"Finished initialisation for DeRichFlatMirror"<< endmsg;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  intersection with solid
//=========================================================================

StatusCode DeRichFlatMirror::intersects( const HepPoint3D& globalP,
                                         const HepVector3D& globalV) const
{
  HepVector3D vLocal = globalV;
  vLocal.transform( geometry()->matrix() );

  ISolid::Ticks ticks;
  const unsigned int noTicks =
    m_solid->intersectionTicks(geometry()->toLocal(globalP), vLocal, ticks);

  return ( 0 == noTicks ? StatusCode::FAILURE : StatusCode::SUCCESS );
}
