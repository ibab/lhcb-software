/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlLVolumeCnv.cpp,v 1.2 2001-01-22 09:55:41 ibelyaev Exp $

#include "DetDesc/XmlLVolumeCnv.h"
#include "DetDesc/XmlCnvException.h"

/// Include files
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/GenericAddress.h"
#include "GaudiKernel/GenericLink.h"

#include "GaudiKernel/ICnvManager.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IDataDirectory.h"

#include "GaudiKernel/MsgStream.h"

#include "GaudiKernel/RegistryEntry.h"

#include "DetDesc/XmlCnvAttrList.h"
#include "DetDesc/XmlAddress.h"
#include "DetDesc/XmlCnvSvc.h"
#include "DetDesc/XmlExprParser.h"

#include "DetDesc/Isotope.h"
#include "DetDesc/Element.h"
#include "DetDesc/Mixture.h"

#include "DetDesc/LVolume.h"
#include "DetDesc/PVolume.h"

#include "DetDesc/Solids.h"

/// Forward and extern declarations

extern const    CLID&   CLID_Isotope;

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: XmlLVolumeCnv.cpp,v 1.2 2001-01-22 09:55:41 ibelyaev Exp $";

// Instantiation of a static factory class used by clients to create
// instances of this service
static CnvFactory<XmlLVolumeCnv> s_factoryLVolume;
const ICnvFactory& XmlLVolumeCnvFactory = s_factoryLVolume;


// Create the transient representation of an object.
StatusCode XmlLVolumeCnv::createObj( IOpaqueAddress* pAddress,
                                     DataObject*& refpObject)              {

  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  
  StatusCode sc = StatusCode::SUCCESS;

  GenericAddress* addr;
  
  // Test and store hint
  if( 0 != pAddress )                                                      {
    try                                                                    {
      addr = dynamic_cast< XmlAddress* >( pAddress );
    }
    catch( ... )                                                           {
      return StatusCode::FAILURE;
    }
  }
  
  m_tagRead             = false;
  m_transContext        = "";
  m_insideBoolean       = false;
  m_insideParameterized = false;
  m_materialName        = "";
  m_volName             = "";
  m_solid               = 0;
  m_bsName              = "";

  m_pvstore.clear();
  m_ppvstore.clear();
  m_bstore.clear();

  m_objRcpt = addr;
  
  sc = initParser();
  if( sc.isFailure() )                                                     {
    log << MSG::FATAL         << "XML Parser init failed, can't convert "
        << addr->objectName() << "!" << endreq;
    finiParser();
    return sc;
  }
  
  sc = parse( addr->dbName().c_str() );
  
  if( sc.isFailure() )                                                     {
    finiParser();
    return StatusCode::FAILURE;
  }
  
  sc = finiParser();
  if( sc.isFailure() )                                                     {
    log << MSG::FATAL         << "XML Parser init failed, can't convert "
        << addr->objectName() << "!" << endreq;
    return sc;
  }

  refpObject = m_dataObj;
  
  return sc;
}

// Update the transient object from the other representation.
StatusCode XmlLVolumeCnv::updateObj(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                    )                  
{
  return StatusCode::SUCCESS;
}

// Convert the transient object to the requested representation
StatusCode XmlLVolumeCnv::createRep(
                                     DataObject*      //pObject
                                    ,IOpaqueAddress*& //refpAddress
                                   )         
{
  return StatusCode::SUCCESS;
}

// Update the converted representation of a transient object.
StatusCode XmlLVolumeCnv::updateRep(
                                     IOpaqueAddress* //pAddress
                                    ,DataObject*     //pObject
                                   )                  
{
  return StatusCode::SUCCESS;
}

// Constructor
XmlLVolumeCnv::XmlLVolumeCnv( ISvcLocator* svc )
: XmlGenericCnv( svc, CLID_LogicalVolume ),
  m_tagRead(false), m_insideBoolean(false), m_insideParameterized(false)   {
  // Register myself as the recevier of ASCII XML SAX events
  set8BitDocHandler( *this );
  m_transContext = "";
  m_materialName = "";
  m_volName = "";
  m_solid = 0;
  m_bsName = "";
  m_pvstore.clear();
  m_ppvstore.clear();
  m_bstore.clear();
}

XmlLVolumeCnv::SolidItem sFound;

void XmlLVolumeCnv::startElement( const char* const name,
                                 XmlCnvAttributeList& attributes)          {

  MsgStream log(msgSvc(), "XmlLVolumeCnv" );

  // Make an instance of the numerical expression parser
  XmlExprParser xep( msgSvc() );

  std::string tagName( name );
  
  /// Solid textified dimensions got from the solids' tag attributes
  std::string sizeX,      sizeY,  sizeZ;
  std::string sizeX1,     sizeX2, sizeY1, sizeY2;
  std::string oRadPz,     oRadMz, iRadPz, iRadMz, startPhi, deltaPhi;
  std::string oRad,       iRad;
  std::string startTheta, deltaTheta;
  
  /// Transformation type attribute value
  std::string tType;
  
  log << MSG::DEBUG << "<" << tagName << " ";
  
  for( unsigned int i = 0; i < attributes.getLength(); i++ )               {
    log << MSG::DEBUG
        << attributes.getName(i)  << "=" 
        << attributes.getValue(i) << " "
        << attributes.getType(i)  << " ";
  }
  log << ">" << endreq;
  
  PVitem    pv;
  ParamPV   parampv;
  
  if( "logvol" == tagName )                                                {
    
    // Basename of the transient object ID
    unsigned int sPos = m_objRcpt->objectName().find_last_of('/');
    std::string baseName = m_objRcpt->objectName().substr( sPos + 1 );
    
    if( baseName == attributes.getValue( "name" ) )                        {
      
      // We're converter for this concrete XML tag
      // We need to create our transient representation
      // according the required class ID
      
      m_tagRead = true;

      // We have to decode class ID of the referred object
      const CLID& clsID = (unsigned long)atol( attributes.getValue( "classID" ).c_str() );
      
      // Remember material name
      m_materialName  = "/dd/Materials/";
      m_materialName += attributes.getValue( "material" );
      
      m_volName = baseName;
      
      // Create the corresponding transient version of the logical volume
      if( CLID_LogicalVolume == clsID )                                    {
        // We move this to endElement() callback because we need to
        // collect XML data for LVolume() constructor
        //m_dataObj = new LVolume();
      }
      else                                                                 {
        log << MSG::ERROR       << "Found invalid class ID " << clsID
            << " for material " << m_objRcpt->objectName()   << endreq;
        // May be en exception should be thrown now
      }      
    }
    else                                                                   {
      // This should never happen!
      log << MSG::WARNING                  << "Got unexpected material tag: "
          << attributes.getValue( "name" ) << endreq;
    }
  }
  else if( "physvol" == tagName )                                          {

    if( m_insideParameterized ) {
      ParamPV& ppv = m_ppvstore.back();
      // We need to record the starting physical volume
      ppv.m_initialPos.m_pvName  = attributes.getValue( "name" );
      ppv.m_initialPos.m_lvName  = attributes.getValue( "logvol" );
    }
    else                        {
      // Ordinary physical volume
      //
      // The following code does not work if we allow to place logical volumes
      // somewhere else than into the /dd/Geometry catalog
      //pv.m_lvName  = "/dd/Geometry/";
      // 
      // Remember name
      pv.m_pvName  = attributes.getValue( "name" );
      pv.m_lvName  = attributes.getValue( "logvol" );
      m_pvstore.push_back( pv );
    }

    setTransContext( tagName );
  }
  else if( "paramphysvol" == tagName )                                     {
    // Remember the number of copies
    ///
    int nn = (int) xep.eval( attributes.getValue("number"), false );
    if( nn <= 0 ) 
      {
      ///
      /// error situation 
      ///
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      throw XmlCnvException( " paramphysvolume:: number of copies must be integer positive ! ", stcod );
      }
    ///
    parampv.m_number = nn; 
    m_ppvstore.push_back( parampv );
    
    // Indicate to the others that paramphysvol tag is the current context
    m_insideParameterized = true;
    
    setTransContext( tagName );

  }
  // Solids
  else if( "box" == tagName )                                              {
    sizeX = attributes.getValue( "sizeX" );
    sizeY = attributes.getValue( "sizeY" );
    sizeZ = attributes.getValue( "sizeZ" );
    
    if( sizeX.empty() ) { sizeX = "0.0"; }
    if( sizeY.empty() ) { sizeY = "0.0"; }
    if( sizeZ.empty() ) { sizeZ = "0.0"; }
    
    m_solid = new SolidBox( attributes.getValue( "name" ),
                            xep.eval( sizeX.c_str() )/2.,
                            xep.eval( sizeY.c_str() )/2.,
                            xep.eval( sizeZ.c_str() )/2.
                          );

    //doBoolean( sFound, attributes );
    setTransContext( tagName );
  }
  else if( "trd" == tagName )                                              {
    sizeX1 = attributes.getValue( "sizeX1" );
    sizeX2 = attributes.getValue( "sizeX2" );
    sizeY1 = attributes.getValue( "sizeY1" );
    sizeY2 = attributes.getValue( "sizeY2" );
    sizeZ  = attributes.getValue( "sizeZ"  );

    if( sizeX1.empty() ) { sizeX1 = "0.0"; }
    if( sizeX2.empty() ) { sizeX2 = "0.0"; }
    if( sizeY1.empty() ) { sizeY1 = "0.0"; }
    if( sizeY2.empty() ) { sizeY2 = "0.0"; }
    if( sizeZ.empty()  ) { sizeZ  = "0.0"; }

    m_solid = new SolidTrd( attributes.getValue( "name" ),
                            xep.eval( sizeZ.c_str()  )/2.,
                            xep.eval( sizeX1.c_str() )/2.,
                            xep.eval( sizeY1.c_str() )/2.,
                            xep.eval( sizeX2.c_str() )/2.,
                            xep.eval( sizeY2.c_str() )/2.
                          );
    //doBoolean( sFound, attributes );
    setTransContext( tagName );
  }
  else if( "cons" == tagName )                                             {
    sizeZ    = attributes.getValue( "sizeZ"         );
    oRadPz   = attributes.getValue( "outerRadiusPZ" );
    oRadMz   = attributes.getValue( "outerRadiusMZ" );
    iRadPz   = attributes.getValue( "innerRadiusPZ" );
    iRadMz   = attributes.getValue( "innerRadiusMZ" );
    startPhi = attributes.getValue( "startPhiAngle" );
    deltaPhi = attributes.getValue( "deltaPhiAngle" );

    if( oRadPz.empty()   ) { oRadPz   = "0.0"; }
    if( oRadMz.empty()   ) { oRadMz   = "0.0"; }
    if( sizeZ.empty()    ) { sizeZ    = "0.0"; }

    ///
    /// both angles could be omitted simultaneously ! 
    /// 
    if( startPhi.empty() != deltaPhi.empty() ) 
      { 
      ///
      /// error ! exception ? 
      ///
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      if( !startPhi.empty() ) {
        throw XmlCnvException( " cons :: only startPhi is provided! ", stcod );
      } 
      if( !deltaPhi.empty() ) {
        throw XmlCnvException( " cons :: only deltaPhi is provided! ", stcod );
      } 
      } 
    
    m_solid = new SolidCons( attributes.getValue( "name" ),
                             xep.eval( sizeZ.c_str()    )/2.,
                             xep.eval( oRadMz.c_str()   ),
                             xep.eval( oRadPz.c_str()   ),
                             iRadMz.empty()   ?   0.0          : xep.eval( iRadMz   ),   
                             iRadPz.empty()   ?   0.0          : xep.eval( iRadPz   ),   
                             startPhi.empty() ?   0.0          : xep.eval( startPhi ),
                             deltaPhi.empty() ? 360.0 * degree : xep.eval( deltaPhi )  
			     );
    //doBoolean( sFound, attributes );
    setTransContext( tagName );
  }
  else if( "tubs" == tagName )                                             {
    sizeZ    = attributes.getValue( "sizeZ"         );
    oRad     = attributes.getValue( "outerRadius"   );
    iRad     = attributes.getValue( "innerRadius"   );
    startPhi = attributes.getValue( "startPhiAngle" );
    deltaPhi = attributes.getValue( "deltaPhiAngle" );

    // log << MSG::INFO << "startPhi " << xep.eval(startPhi)/degree << endreq;
    // log << MSG::INFO << "deltaPhi " << xep.eval(deltaPhi)/degree << endreq;
    
    if( oRad.empty()     ) { oRad     = "0.0"; }
    if( sizeZ.empty()    ) { sizeZ    = "0.0"; }

    ///
    /// both angles could be omitted simultaneously ! 
    /// 
    if( startPhi.empty() != deltaPhi.empty() ) { 
      ///
      /// error ! exception ? 
      ///
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      if( !startPhi.empty() ) {
        throw XmlCnvException( " tubs :: only startPhi is provided! ", stcod );
      } 
      if( !deltaPhi.empty() ) {
        throw XmlCnvException( " tubs :: only deltaPhi is provided! ", stcod );
      } 
    } 

    m_solid = new SolidTubs( attributes.getValue( "name" ),
                             xep.eval( sizeZ.c_str()    )/2.,
                             xep.eval( oRad.c_str()     ),
                             iRad.empty()     ?   0.0          : xep.eval( iRad     ),
                             startPhi.empty() ?   0.0          : xep.eval( startPhi ),
                             deltaPhi.empty() ? 360.0 * degree : xep.eval( deltaPhi )  
			     );
    //doBoolean( sFound, attributes );
    setTransContext( tagName );
  }
  else if( "sphere" == tagName )                                           {
    oRad       = attributes.getValue( "outerRadius"     );
    iRad       = attributes.getValue( "innerRadius"     );
    startPhi   = attributes.getValue( "startPhiAngle"   );
    deltaPhi   = attributes.getValue( "deltaPhiAngle"   );
    startTheta = attributes.getValue( "startThetaAngle" );
    deltaTheta = attributes.getValue( "deltaThetaAngle" );

    if( oRad.empty()       ) { oRad       = "0.0"; }
    ///
    ///
    /// 
    ///
    /// both angles could be omitted simultaneously ! 
    /// 
    if( startPhi.empty() != deltaPhi.empty() ) { 
      ///
      /// error ! exception ? 
      ///
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      if( !startPhi.empty() ) {
        throw XmlCnvException( " sphere :: only startPhi is provided! ", stcod );
      } 
      if( !deltaPhi.empty() ) {
        throw XmlCnvException( " sphere :: only deltaPhi is provided! ", stcod );
      } 
    } 

    if( startTheta.empty() != deltaTheta.empty() ) { 
      ///
      /// error ! exception ? 
      ///
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      if( !startTheta.empty() ) {
        throw XmlCnvException( " sphere :: only startTheta is provided! ", stcod );
      } 
      if( !deltaTheta.empty() ) {
        throw XmlCnvException( " sphere :: only deltaTheta is provided! ", stcod );
      } 
    } 

    m_solid = new SolidSphere( attributes.getValue( "name" ),
                               xep.eval( oRad.c_str()       ),
                               iRad.empty()       ?   0.0          : xep.eval( iRad       ),
			       startPhi.empty()   ?   0.0          : xep.eval( startPhi   ),
			       deltaPhi.empty()   ? 360.0 * degree : xep.eval( deltaPhi   ),  
			       startTheta.empty() ?   0.0          : xep.eval( startTheta ),
			       deltaTheta.empty() ? 180.0 * degree : xep.eval( deltaTheta )  
			       );
    //doBoolean( sFound, attributes );
    setTransContext( tagName );
  }
  else if( "union" == tagName || "subtraction" == tagName || "intersection" == tagName ){
    m_insideBoolean = true;
    m_bsName = attributes.getValue( "name" );
  }
  // End of Solids
  else if( tagName == "posXYZ" )                                           {
    if( transContext().empty() )                                          {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );  
      throw XmlCnvException( " posXYZ in an empty transformation context", stcod );
    }
    else                                                                   {
      log << MSG::DEBUG << "posXYZ tag in the context of "
                        << transContext() << endreq;
  
      if( m_insideParameterized )                                    {
        // Parameterized physical volume initial position
        // or step for transformation
        ParamPV& ppv = m_ppvstore.back();

        if( transContext() == "physvol" )                       {
          // Initial position
          ppv.m_initialPos.m_translation = doTranslation(
                                                     attributes.getValue( "x" ),
                                                     attributes.getValue( "y" ),
                                                     attributes.getValue( "z" )
                                                    );
        }
        else                                                    {
          // Step for transformation
          ppv.m_stepTranslation          = doTranslation(
                                                     attributes.getValue( "x" ),
                                                     attributes.getValue( "y" ),
                                                     attributes.getValue( "z" )
                                                    );
        }
      }
      else if( transContext() == "physvol" )                         {
        // Position of a ordinary physical volume
        PVitem &pv = m_pvstore.back();
        pv.m_translation = doTranslation( attributes.getValue( "x" ),
                                          attributes.getValue( "y" ),
                                          attributes.getValue( "z" )
                                        );
      }
      else                                                           {
        // Position of a solid ( Hope... )
        if( m_insideBoolean == true )              {
          doBoolTranslation( sFound, attributes );
        }
        else {
          log << MSG::WARNING
              << "posXYZ for a solid makes sense only inside boolean solid"
              << endreq;
        }
      }
    }
  }
  else if( tagName == "posRPhiZ" )                                           {
    if( transContext().empty() )                                          {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );  
      throw XmlCnvException( " posRPhiZ in an empty transformation context", stcod );
    }
    else                                                                   {
      log << MSG::DEBUG << "posRPhiZ tag in the context of "
                       << transContext() << endreq;

      if( m_insideParameterized )                                    {
        // Parameterized physical volume initial position
        // or step for transformation

        ParamPV& ppv = m_ppvstore.back();

        if( transContext() == "physvol" )                       {
          // Initial position
          ppv.m_initialPos.m_translation = doRPhiZTranslation(
                                                     attributes.getValue( "r" ),
                                                     attributes.getValue( "phi" ),
                                                     attributes.getValue( "z" )
                                                    );
        }
        else                                                    {
          // Step for transformation
          ppv.m_stepTranslation = doRPhiZTranslation(
                                                     attributes.getValue( "r" ),
                                                     attributes.getValue( "phi" ),
                                                     attributes.getValue( "z" )
                                                    );
        }
      }
      else if( transContext() == "physvol" )                              {
        // Posistion of a physical volume
        PVitem &pv = m_pvstore.back();
        pv.m_translation = doRPhiZTranslation( attributes.getValue( "r" ),
                                               attributes.getValue( "phi" ),
                                               attributes.getValue( "z" )
                                             );
      }
      else                                              {
        // Position of a solid ( Hope... )
        if( m_insideBoolean == true )              {
          doBoolRPhiZTranslation( sFound, attributes );
        }
        else {
          log << MSG::WARNING
              << "posRPhiZ for a solid makes sense only inside boolean solid"
              << endreq;
        }
      }
    }
  }
  else if( tagName == "posRThPhi" )                                        {
    if( transContext().empty() )                                          {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );  
      throw XmlCnvException( " posRThPhi in an empty transformation context", stcod );
    }
    else                                                                   {
      log << MSG::DEBUG << "posRThPhi tag in the context of "
                       << transContext() << endreq;

      if( m_insideParameterized )                                    {
        // Parameterized physical volume initial position
        // or step for transformation

        ParamPV& ppv = m_ppvstore.back();

        if( transContext() == "physvol" )                       {
          // Initial position
          ppv.m_initialPos.m_translation = doRThPhiTranslation(
                                                     attributes.getValue( "r" ),
                                                     attributes.getValue( "theta" ),
                                                     attributes.getValue( "phi" )
                                                    );
        }
        else                                                    {
          // Step for transformation
          ppv.m_stepTranslation = doRThPhiTranslation(
                                                     attributes.getValue( "r" ),
                                                     attributes.getValue( "theta" ),
                                                     attributes.getValue( "phi" )
                                                    );
        }
      }
      else if( transContext() == "physvol" )                              {
        // Posistion of a physical volume
        PVitem &pv = m_pvstore.back();
        pv.m_translation = doRThPhiTranslation( attributes.getValue( "r" ),
                                                attributes.getValue( "theta" ),
                                                attributes.getValue( "phi" )
                                              );
      }
      else                                              {
        // Position of a solid ( Hope... )
        if( m_insideBoolean == true )              {
          doBoolRThPhiTranslation( sFound, attributes );
        }
        else {
          log << MSG::WARNING
              << "posRThPhi for a solid makes sense only inside boolean solid"
              << endreq;
        }
      }
    }
  }
  else if( tagName == "rotXYZ" )                                           {
    if( transContext().empty() )       {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );  
      throw XmlCnvException( " rotXYZ in an empty transformation context", stcod );
    }
    else                                                                   {
      log << MSG::DEBUG << "rotXYZ tag in the context of "
                       << transContext() << endreq;

      if( m_insideParameterized )                                    {
        // Parameterized physical volume initial position
        // or step for transformation
        
        ParamPV& ppv = m_ppvstore.back();

        if( transContext() == "physvol" )                       {
          // Initial position
          ppv.m_initialPos.m_rotation    = doRotation(
                                                       attributes.getValue( "rotX" ),
                                                       attributes.getValue( "rotY" ),
                                                       attributes.getValue( "rotZ" )
                                                      );
        }
        else                                                    {
          // Step for transformation
          ppv.m_stepRotation    = doRotation( attributes.getValue( "rotX" ),
                                                     attributes.getValue( "rotY" ),
                                                     attributes.getValue( "rotZ" )
                                                   );
        }
      }
      else if( transContext() == "physvol" )                               {
        // Rotation of a physical volume
        PVitem &pv = m_pvstore.back();
        pv.m_rotation    = doRotation( attributes.getValue( "rotX" ),
                                       attributes.getValue( "rotY" ),
                                       attributes.getValue( "rotZ" )
                                     );
      }
      else                                          {
        // Rotation of a solid ( Hope... )
        if( m_insideBoolean == true )           {
          doBoolRotation( sFound, attributes );
        }
        else {
          log << MSG::WARNING
              << "rotXYZ for a solid makes sense only inside boolean solid"
              << endreq;
        }
      }
    }
  }
  else if( tagName == "rotAxis" )                                          {
    if( transContext().empty() )       {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );  
      throw XmlCnvException( " rotAxis in an empty transformation context", stcod );
    }
    else                                                                   {
      log << MSG::DEBUG << "rotAxis tag in the context of "
                       << transContext() << endreq;

      if( m_insideParameterized )                                    {
        // Parameterized physical volume initial position
        // or step for transformation

        ParamPV& ppv = m_ppvstore.back();
        
        if( transContext() == "physvol" )                       {
          // Initial position
          ppv.m_initialPos.m_rotation = doAxisRotation(
                                                       attributes.getValue( "axTheta" ),
                                                       attributes.getValue( "axPhi" ),
                                                       attributes.getValue( "angle" )
                                                      );
        }
        else                                                    {
          // Step for transformation
          ppv.m_stepRotation = doAxisRotation(
                                                       attributes.getValue( "axTheta" ),
                                                       attributes.getValue( "axPhi" ),
                                                       attributes.getValue( "angle" )
                                                      );
        }
      }
      else if( transContext() == "physvol" )                               {
        // Rotation of a physical volume
        PVitem &pv = m_pvstore.back();
        pv.m_rotation    = doAxisRotation( attributes.getValue( "axTheta" ),
                                           attributes.getValue( "axPhi" ),
                                           attributes.getValue( "angle" )
                                         );
      }
      else                                          {
        // Rotation of a solid ( Hope... )
        if( m_insideBoolean == true )           {
          doBoolAxisRotation( sFound, attributes );
        }
        else {
          log << MSG::WARNING
              << "rotAxis for a solid makes sense only inside boolean solid"
              << endreq;
        }
      }
    }
  }
  else                                                                     {
    // Something goes wrong, does it?
    log << MSG::WARNING
        << "This tag makes no sense to LVolume: " << tagName << endreq;
  }
}

void XmlLVolumeCnv::characters(
                                const char* const  //chars
                               ,const unsigned int //length
                              )                
{
}

void XmlLVolumeCnv::endElement( const char* const name )                   {
  
  MsgStream log(msgSvc(), "XmlLVolumeCnv" );
  
  log << MSG::DEBUG << "</" << name << ">" << endreq;

  std::string tagName = name;

  if( true == m_tagRead && "logvol" == tagName )                           {
    m_tagRead = false;
    LVolume* vol = new LVolume( m_volName, m_solid, m_materialName,
                                dataProvider(), msgSvc());
    
    //  Add all of the physical volumes
    std::vector<PVitem>::iterator pvit;
    for( pvit = m_pvstore.begin(); pvit != m_pvstore.end(); pvit++ )        {
        vol->createPVolume( (*pvit).m_pvName,
                            (*pvit).m_lvName,
                            (*pvit).m_translation,
                            (*pvit).m_rotation );
    }

    // Add all of the parametric physical volumes
    std::vector<ParamPV>::iterator ppvit;
    for( ppvit = m_ppvstore.begin(); ppvit != m_ppvstore.end(); ppvit++ )  {
      vol->createMultiPVolume( (*ppvit).m_initialPos.m_pvName,
                               (*ppvit).m_initialPos.m_lvName,
                               (*ppvit).m_number,
                               (*ppvit).m_initialPos.m_translation,
                               (*ppvit).m_initialPos.m_rotation,
                               (*ppvit).m_stepTranslation,
                               (*ppvit).m_stepRotation
                             );
    }

    m_dataObj = vol;

    m_pvstore.clear();
    m_ppvstore.clear();
  }
  else if( "union" == tagName ) {
    SolidUnion* sol = new SolidUnion( m_bsName, m_bstore[0].m_solid );
    
    for( unsigned int i = 1; i < m_bstore.size(); i++ ) {
      sol->unite( m_bstore[i].m_solid       , 
		  m_bstore[i].m_translation , 
		  m_bstore[i].m_rotation    );
    }
    
    m_solid = sol;
    m_insideBoolean = false;
    m_bstore.clear();
  }
  else if( "subtraction" == tagName ) {
    SolidSubtraction* sol = new SolidSubtraction( m_bsName, m_bstore[0].m_solid );
    
    for( unsigned int i = 1; i < m_bstore.size(); i++ ) {
      sol->subtract( m_bstore[i].m_solid       , 
		     m_bstore[i].m_translation ,  
		     m_bstore[i].m_rotation    );
    }

    m_solid = sol;
    m_insideBoolean = false;
    m_bstore.clear();
  }
  else if( "intersection" == tagName ) {
    SolidIntersection* sol = new SolidIntersection( m_bsName, m_bstore[0].m_solid );
    
    for( unsigned int i = 1; i < m_bstore.size(); i++ ) {
      sol->intersect( m_bstore[i].m_solid       ,
		      m_bstore[i].m_translation , 
		      m_bstore[i].m_rotation    );
    }
    
    m_solid = sol;
    m_insideBoolean = false;
    m_bstore.clear();
  }
  else if( tagName == "physvol" ) {
    if( !m_insideParameterized ) { setTransContext( ""             ); } 
    else                         { setTransContext( "paramphysvol" ); } 
  }
  else if( tagName == "paramphysvol" ) {
    m_insideParameterized = false;
    setTransContext("");
  }
  else if( tagName == "box" || tagName == "tubs" || tagName == "cons" ||
           tagName == "trd" || tagName == "sphere" )                       {

    if( m_insideBoolean == true ) {
      sFound.m_solid = m_solid;
      m_bstore.push_back( sFound );
    }

    setTransContext("");
  }
  else {
    // This end of tag we ignore
  }
}

// Implementation of the obligatory method to provide a hint to generic XML
// converter about the XML tag we want to be notified about
const char* XmlLVolumeCnv::tag() const                                      {
  return "logvol";
}

HepPoint3D XmlLVolumeCnv::doRPhiZTranslation( std::string r,
                                              std::string phi,
                                              std::string z )               {
  
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doRPhiZTranslation" );

  XmlExprParser xep( msgSvc() );

  HepPoint3D ret;
  
  if( !r.empty() && !phi.empty() && !z.empty() )                            {

    // We must construct a unit vector here!!!!!!
    HepPoint3D tmp(1,1,1);
    ret = tmp;

    if( 0 <= xep.eval( r ) )                         {
      ret.setPerp( xep.eval( r ) );
    }
    else                                               {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      throw XmlCnvException( " doRPhiZTranslation: r must be non-negative value!", stcod );
    }
    
    ret.setPhi(  xep.eval( phi ) );
    ret.setZ( xep.eval( z ) );

  }

  return ret;
}

HepPoint3D XmlLVolumeCnv::doRThPhiTranslation( std::string r,
                                                std::string theta,
                                                std::string phi )          {
  
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doRThPhiTranslation" );

  XmlExprParser xep( msgSvc() );

  HepPoint3D ret;

  if( !r.empty() && !theta.empty() && !phi.empty() )                       {

    // We must construct a unit vector here!!!!!!
    HepPoint3D tmp(1,1,1);
    ret = tmp;

    if( 0 <= xep.eval( r ) )                         {
      ret.setMag( xep.eval( r ) );
    }
    else                                               {
      StatusCode stcod;
      stcod.setCode( CORRUPTED_DATA );
      throw XmlCnvException( " doRThPhiTranslation: r must be non-negative value!", stcod );
    }

    ret.setTheta(  xep.eval( theta ) );
    ret.setPhi(    xep.eval( phi ) );

  }

  // Debug printout of translation vector
  //std::cout << "doTranslation: " << ret << std::endl;

  return ret;
}

HepPoint3D XmlLVolumeCnv::doTranslation( std::string x,
                                         std::string y,
                                         std::string z )                    {
  
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doTranslation" );

  XmlExprParser xep( msgSvc() );

  HepPoint3D ret;
  
  if( !x.empty() && !y.empty() && !z.empty() )                              {

    double l_x = xep.eval( x ); 
    double l_y = xep.eval( y ); 
    double l_z = xep.eval( z ); 

    ret.setX( l_x );
    ret.setY( l_y );
    ret.setZ( l_z );

    /* This suddenly doesn't work, different calling convention???
    ret.setX( xep.eval( x ) );
    ret.setY( xep.eval( y ) );
    ret.setZ( xep.eval( z ) );
    */
  }

  // Debug printout of translation vector
  //std::cout << "doTranslation: " << ret << std::endl;

  return ret;
}

HepRotation XmlLVolumeCnv::doRotation( std::string rx,
                                       std::string ry,
                                       std::string rz )                    {

  MsgStream log(msgSvc(), "XmlLVolumeCnv::doRotation" );

  XmlExprParser xep( msgSvc() );
  
  HepRotation rot;

  if( !rx.empty() && !ry.empty() && !rz.empty() )                          {
    rot =       HepRotateX3D( xep.eval( rx ) ).getRotation();
    rot = rot * HepRotateY3D( xep.eval( ry ) ).getRotation();
    rot = rot * HepRotateZ3D( xep.eval( rz ) ).getRotation();
    rot = rot.inverse();
  }

  return rot;
}

HepRotation XmlLVolumeCnv::doAxisRotation( std::string axtheta,
                                           std::string axphi,
                                           std::string angle )            {
  
  MsgStream log(msgSvc(), "XmlLVolumeCnv::doAxisRotation" );
  
  XmlExprParser xep( msgSvc() );
  
  HepRotation rot;
  
  if( !axtheta.empty() && !axphi.empty() && !angle.empty() )               {
    double axt  = xep.eval( axtheta );
    double axp  = xep.eval( axphi   );
    double angl = xep.eval( angle   );
    
    if( axt < 0 || axt > 180 * degree ) 
      { 
	StatusCode stcod;
	stcod.setCode( CORRUPTED_DATA );
	throw XmlCnvException( " doAxisRotation: axTheta must be inside 0*&degree; and 180*&degree; ! ", stcod );
      }
    
    if( axp < 0 || axp > 360 * degree ) 
      { 
	StatusCode stcod;
	stcod.setCode( CORRUPTED_DATA );
	throw XmlCnvException( " doAxisRotation: axPhi   must be inside 0*&degree; and 360*&degree; ! ", stcod );
      }

    // Construction of vector with input of theta and phi
    // v = Hep3Vector( sin(theta)*cos(phi) , sin(theta)*sin(phi) , cos(theta) ) 
    rot = HepRotate3D( angl,
                       Hep3Vector( sin(axt)*cos(axp),
                                   sin(axt)*sin(axp),
                                   cos(axt) )
                     ).getRotation();

    rot = rot.inverse();
  }
    
  return rot;
}
    

HepTransform3D* XmlLVolumeCnv::doTransformation( std::string x,
                                                std::string y,
                                                std::string z,
                                                std::string rx,
                                                std::string ry,
                                                std::string rz
                                                )                          {
  return new HepTransform3D( doRotation( rx, ry, rz ), doTranslation( x, y, z ) );
}

void XmlLVolumeCnv::doBoolean( SolidItem&  siRef,
                               XmlCnvAttributeList& attributes )           {
  
  std::string tType = attributes.getValue( "type" );
  
  if( true == m_insideBoolean )                                            {
    if( tType.empty() || "0" == tType )                                    {
      // We can't ignore transformation attributes
      //      siRef.m_transform = doTransformation( attributes.getValue( "x" ),
      //                                     attributes.getValue( "y" ),
      //                                    attributes.getValue( "z" ),
      //                                    attributes.getValue( "rotX" ),
      //                                    attributes.getValue( "rotY" ),
      //                                    attributes.getValue( "rotZ" )
      //                                  );
      siRef.m_translation = doTranslation( attributes.getValue( "x"    ),
					                                 attributes.getValue( "y"    ),
					                                 attributes.getValue( "z"    ) );
      
      siRef.m_rotation    = doRotation( attributes.getValue( "rotX" ),
					                              attributes.getValue( "rotY" ),
				                                attributes.getValue( "rotZ" ) );
      siRef.m_solid = m_solid;
      m_bstore.push_back( siRef );
    }
  }  
}

void XmlLVolumeCnv::doBoolTranslation( SolidItem&  siRef,
                                       XmlCnvAttributeList& attributes )   {
  if( true == m_insideBoolean )                                            {
    siRef.m_translation = doTranslation( attributes.getValue( "x" ),
                                         attributes.getValue( "y" ),
                                         attributes.getValue( "z" ) );
    
    //siRef.m_solid = m_solid;
    //m_bstore.push_back( siRef );
  }  
}

void XmlLVolumeCnv::doBoolRPhiZTranslation( SolidItem&  siRef,
                                       XmlCnvAttributeList& attributes )   {
  if( true == m_insideBoolean )                                            {
    siRef.m_translation = doRPhiZTranslation( attributes.getValue( "r" ),
                                              attributes.getValue( "phi" ),
                                              attributes.getValue( "z" ) );
    
    //siRef.m_solid = m_solid;
    //m_bstore.push_back( siRef );
  }  
}

void XmlLVolumeCnv::doBoolRThPhiTranslation( SolidItem&  siRef,
                                       XmlCnvAttributeList& attributes )   {
  if( true == m_insideBoolean )                                            {
    siRef.m_translation = doRThPhiTranslation( attributes.getValue( "r" ),
                                               attributes.getValue( "theta" ),
                                               attributes.getValue( "phi" ) );
    
    //siRef.m_solid = m_solid;
    //m_bstore.push_back( siRef );
  }  
}

void XmlLVolumeCnv::doBoolRotation( SolidItem&  siRef,
                                    XmlCnvAttributeList& attributes )     {
  if( true == m_insideBoolean )                                           {
    siRef.m_rotation    = doRotation( attributes.getValue( "rotX" ) ,
                                      attributes.getValue( "rotY" ) ,
                                      attributes.getValue( "rotZ" ) )  ;
    //siRef.m_solid = m_solid;
    //m_bstore.push_back( siRef );
  }  
}

void XmlLVolumeCnv::doBoolAxisRotation( SolidItem&  siRef,
                                    XmlCnvAttributeList& attributes )     {
  if( true == m_insideBoolean )                                           {
    siRef.m_rotation    = doAxisRotation( attributes.getValue( "axTheta" ),
                                          attributes.getValue( "axPhi" ),
                                          attributes.getValue( "angle" )
                                        );
  }  
}
