// $Id: GiGaMCParticleCnv.cpp,v 1.31 2006-02-06 16:05:59 jonrob Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.30  2006/01/31 10:34:15  gcorti
// adapt to changes in event model
//
// Revision 1.29  2005/04/01 11:41:04  gcorti
// adapt to changes in MCParticle
//
// Revision 1.28  2005/01/17 18:14:40  robbep
// Use generator energy instead of Geant4 energy for short lived particles.
//
// Revision 1.27  2004/05/03 13:50:19  gcorti
// set particle type for ions
//
// Revision 1.26  2004/04/07 15:47:55  gcorti
// signal info, extended collision, new vertex types
//
// Revision 1.25  2003/10/31 12:40:05  witoldp
// fixed units in GiGaHepMCCnv
//
// Revision 1.24  2003/07/14 15:26:36  witoldp
// some restructurisation to GiGaCollisionCnv
//
// Revision 1.23  2003/07/11 17:42:59  witoldp
// added collision converter
//
// Revision 1.22  2003/07/07 16:45:30  ranjard
// v12r3 - fix for gcc3.2
//
// Revision 1.21  2003/02/21 09:34:18  witoldp
// some cout`s removed
//
// Revision 1.20  2003/01/23 10:06:30  witoldp
// added handling of shortlived particles
//
// Revision 1.19  2002/12/07 21:13:48  ibelyaev
//  bug fix and small CPU performace optimization
//
//  ===========================================================================
#define GIGACNV_GIGAMCPARTICLECNV_CPP 1
// ============================================================================
/// STL
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <functional>
/// GaudiKernel
#include "GaudiKernel/CnvFactory.h"
#include "GaudiKernel/IAddressCreator.h"
#include "GaudiKernel/IOpaqueAddress.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
/// GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/GaudiException.h"
/// GiGa
#include "GiGa/IGiGaSvc.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
/// GiGaCnv
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
/// LHCbEvent
#include "Event/MCParticle.h"
#include "Event/HepMCEvent.h"
/// Geant4 includes
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
// local
#include "GiGaCnvFunctors.h"
#include "GiGaMCParticleCnv.h"
#include "Trajectory2Particle.h"

// ======================================================================
/** @file GiGaMcParticleCnv.cpp
 *
 *  implementation of class GiGaMCParticleCnv
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 */
// ======================================================================

// ======================================================================
// Factory stuff
// ======================================================================
static const  CnvFactory<GiGaMCParticleCnv>         s_Factory ;
const        ICnvFactory&GiGaMCParticleCnvFactory = s_Factory ;

// ======================================================================
/** Standard Constructor
 *  @param loc pointer to service locator
 */
// ======================================================================
GiGaMCParticleCnv::GiGaMCParticleCnv( ISvcLocator* Locator )
  : GiGaCnvBase( storageType() , classID() , Locator )
{
  ///
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Kine ) ;
  setConverterName              ( "GiGaMCPCnv"              ) ;
  ///
  declareObject( GiGaLeaf( LHCb::MCParticleLocation::Default , objType() ) );
  ///
};

// ======================================================================
// destructor
// ======================================================================
GiGaMCParticleCnv::~GiGaMCParticleCnv(){};

// ======================================================================
// Class ID
// ======================================================================
const CLID&         GiGaMCParticleCnv::classID     ()
{ return LHCb::MCParticles::classID(); }

// ======================================================================
// StorageType
// ======================================================================
const unsigned char GiGaMCParticleCnv::storageType ()
{ return GiGaKine_StorageType; }

// ======================================================================
/** initialize the converter
 *  @return status code
 */
// ======================================================================
StatusCode GiGaMCParticleCnv::initialize()
{
  /// initialize the base class
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() )
  { return Error("Could not initialze the base class!",sc);}
  /// check for nesessary services
  if( 0 == kineSvc() )
  { return Error("IGiGaKineCnvSvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
  ///
};

// ======================================================================
/** finalize  the converter
 *  @return status code
 */
// ======================================================================
StatusCode GiGaMCParticleCnv::finalize  ()
{
  /// clear the reference table
  if( 0 != kineSvc() ) { kineSvc()->table().clear();}
  ///
  return GiGaCnvBase::finalize();
};

// ======================================================================
/** create the Object
 *  @param address address
 *  @param object object itself
 *  @return status code
 */
// ======================================================================
StatusCode GiGaMCParticleCnv::createObj
( IOpaqueAddress*  address ,
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  )
  { return Error("IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new LHCb::MCParticles();
  ///
  StatusCode sc = updateObj( address , object );
  if( sc.isFailure() )
  {
    if( 0 != object ) { delete object ; object = 0 ; }
    return Error("Could not create and update Object" , sc );
  };
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** fill the object references
 *  @param address address
 *  @param object object itself
 *  @return status code
 */
// ============================================================================
StatusCode GiGaMCParticleCnv::fillObjRefs
( IOpaqueAddress*  address ,
  DataObject*      object  )
{
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL" );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"     );}
  LHCb::MCParticles* particles = dynamic_cast<LHCb::MCParticles*> ( object );
  if( 0 ==   particles ) { return Error(" DataObject* (of type '"       +
                                        GiGaUtil::ObjTypeName( object ) +
                                        "*') is not 'Particles*'!"      );}
  ///
  return updateObjRefs( address , object );
  ///
};

// ============================================================================
/** update the Object
 *  @param Address address
 *  @param Object object itself
 *  @return status code
 */
// ============================================================================
StatusCode GiGaMCParticleCnv::updateObj
( IOpaqueAddress*  address ,
  DataObject*      object  )
{
  ///
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL");}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"    );}
  LHCb::MCParticles* particles = dynamic_cast<LHCb::MCParticles*> ( object );
  if( 0 ==   particles ) { return Error(" DataObject*(of type '"      +
                                        GiGaUtil::ObjTypeName(object) +
                                        "*') is not 'Particles*'! "   );}
  // clear the object
  particles->clear();

  // clear the references between MCParticles and Geant4 TrackIDs
  GiGaKineRefTable& table = kineSvc()->table();
  table.clear();
  // retrieve the  trajectory container from GiGa Service
  G4TrajectoryContainer* trajectories = 0 ;
  try
  {
    // get trajectories from GiGa
    *gigaSvc() >> trajectories ;
    //
    if( 0 == trajectories )
    { return Error("No G4TrajectoryContainer* object is found!"); }
    // reserve elements on object container
    particles->reserve( trajectories->size() );
    // reserve elements on reference table
    table.resize( 4 * trajectories->size() );
    // create the conversion functor
    GiGaCnvFunctors::Trajectory2Particle Cnv( ppSvc() );
    // perform the conversion itself
    TrajectoryVector* tv = trajectories->GetVector();

    for(TrajectoryVector::const_iterator iTr = tv->begin(); tv->end() != iTr;
        ++iTr )
    {
      const GiGaTrajectory* trajectory = gigaTrajectory(*iTr );
      // convert the trajectory into particle and add it into container
      LHCb::MCParticle* mcp = Cnv( trajectory );

      // Check when pdg encoding is zero
      if( 0 == mcp->particleID().pid() ) {
        std::string message = "PDGEncoding does not exist, G4 name is ";
        message += trajectory->partDef()->GetParticleName();
        Warning( message, StatusCode::SUCCESS, 0 );
      }

      // insert the particle to container and fill the reference table
      const int index = trajectory->trackID() ;
      particles -> insert( mcp );
      table( index )  = GiGaKineRefTableEntry( mcp , index );
    }
  }
  catch( const GaudiException& Excp )
  {
    particles->clear(); table.clear();
    return Exception("updateObj: " , Excp ) ;
  }
  catch( const std::exception& Excp )
  {
    particles->clear(); table.clear();
    return Exception("updateObj: " , Excp ) ;
  }
  catch( ...  )
  {
    particles->clear(); table.clear();
    return Exception("updateObj: " ) ;
  }
  ///
  return StatusCode::SUCCESS;
};

// ======================================================================
/** update the object references
 *  @param address address
 *  @param object object itself
 *  @return status code
 */
// ======================================================================
StatusCode GiGaMCParticleCnv::updateObjRefs
( IOpaqueAddress*  address ,
  DataObject*      object  )
{
  if( 0 ==   address   ) { return Error(" IOpaqueAddress* points to NULL " );}
  if( 0 ==   object    ) { return Error(" DataObject* points to NULL"      );}
  LHCb::MCParticles* particles = dynamic_cast<LHCb::MCParticles*> ( object );
  if( 0 ==   particles ) { return Error(" DataObject*(of type '"      +
                                        GiGaUtil::ObjTypeName(object) +
                                        "*') is not 'Particles*'!"    );}
  // get trajectories
  G4TrajectoryContainer* trajectories = 0 ;
  try{ *gigaSvc() >> trajectories ; }
  catch( const GaudiException& Excpt  )
  { return Exception("UpdateObjRefs: " , Excpt ) ; }
  catch( const std::exception& Excpt  )
  { return Exception("UpdateObjRefs: " , Excpt ) ; }
  catch( ... )
  { return Exception("UpdateObjRefs: "         ) ; }
  if( 0 == trajectories      )
  { return Error("No G4TrajectoryContainer* object is found!"); }
  if( (size_t) trajectories->size() != (size_t) particles->size() )
  { return Error(" 'Particles' and G4Tajectories have different sizes!"); }
  // get the vertices:
  IRegistry* parent  =
    GiGaCnvUtils::parent( address->registry() , evtSvc() );
  if( 0 == parent ) { return Error( " Parent directory is not accessible!"); }
  const std::string verticesPath( parent->identifier() + "/Vertices" ) ;
  SmartDataPtr<LHCb::MCVertices> vertices( evtSvc() , verticesPath );
  if( !vertices )
  { return Error("Could not locate Vertices in '" + verticesPath + "'" ); }
  const long refID = object->linkMgr()->addLink( verticesPath ,  vertices );

  // clear all existing references
  std::transform( particles->begin                     () ,
                  particles->end                       () ,
                  particles->begin                     () ,
                  GiGaCnvFunctors::MCParticleResetRefs () ) ;
  //
  typedef SmartRef<LHCb::MCVertex>               Ref;
  typedef GiGaTrajectory::const_iterator   ITG;
  typedef TrajectoryVector::const_iterator ITC;

  LHCb::MCVertex miscVertex; /// misc
  LHCb::MCVertices::iterator iVertex = vertices -> begin();

  // new version by WP

  // loop over all vertices created by GiGaMCVertexCnv
  for(iVertex=vertices->begin();vertices->end()!=iVertex;++iVertex)
  {
    // check if there exist mother particle for the vertex
    // if yes add this vertex to EnvVertices of the mother
    if ((*iVertex)->mother())
    {
      Ref decay( (*iVertex)->mother(), refID, (*iVertex)->key(), *iVertex);
      LHCb::MCParticle * mcpart = const_cast<LHCb::MCParticle*>((*iVertex)->mother());
      if ( mcpart ) { mcpart->addToEndVertices( decay ); }
      else { Exception("UpdateObjRefs: Null MCParticle"); }
    }

    // look at the products of the vertex (outgoing particles)
    const SmartRefVector<LHCb::MCParticle>& daughters = (*iVertex)->products();
    SmartRefVector<LHCb::MCParticle>::const_iterator idau;
    for ( idau = daughters.begin(); idau != daughters.end(); ++idau )
    {
      // for each outgoing particle set this vertex to be OriginVertex
      Ref prod( (*idau), refID , (*iVertex)->key() , *iVertex  ) ;
      LHCb::MCParticle * mcpart =
        const_cast<LHCb::MCParticle*>((const LHCb::MCParticle*)*idau);
      if ( mcpart ) { mcpart->setOriginVertex( prod ); }
      else { Exception("UpdateObjRefs: Null MCParticle"); }
    }
  }

  return StatusCode::SUCCESS;
}

