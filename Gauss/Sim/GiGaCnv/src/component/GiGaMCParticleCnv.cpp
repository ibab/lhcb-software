// $Id: GiGaMCParticleCnv.cpp,v 1.18 2002-12-07 14:36:27 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.17  2002/07/17 08:29:49  ranjard
// v7r0 - adapted to Geant4 4.1
//
// Revision 1.16  2002/07/02 15:15:44  ibelyaev
//  fix the bugs
//
// Revision 1.15  2002/05/20 13:36:16  ibelyaev
//  add conversion of primary vertices
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
/// GiGa 
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaTrajectory.h" 
#include "GiGa/GiGaUtil.h"
/// GiGaCnv  
#include "GiGaCnv/GiGaKineRefTable.h" 
#include "GiGaCnv/GiGaCnvUtils.h" 
/// LHCbEvent
#include "Event/MCParticle.h" 
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
  declareObject( GiGaLeaf( MCParticleLocation::Default , objType() ) );
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
{ return MCParticles::classID(); }

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
  object        = new MCParticles();
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
  MCParticles* particles = dynamic_cast<MCParticles*> ( object ); 
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
  MCParticles* particles = dynamic_cast<MCParticles*> ( object ); 
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
          const GiGaTrajectory* trajectory = 
            ( 0 == *iTr ) ? (const GiGaTrajectory*) 0 : 
            dynamic_cast<const GiGaTrajectory*> (*iTr );
          // convert the trajectory into particle and add it into container 
          MCParticle* mcp = Cnv( trajectory );
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
  MCParticles* particles = dynamic_cast<MCParticles*> ( object ); 
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
  SmartDataPtr<MCVertices> vertices( evtSvc() , verticesPath );
  if( !vertices ) 
    { return Error("Could not locate Vertices in '" + verticesPath + "'" ); }
  const long refID = object->linkMgr()->addLink( verticesPath ,  vertices );
  
  // get the references between MCParticles and Geant4 TrackIDs
  GiGaKineRefTable& table = kineSvc()->table();
  
  // clear all existing references 
  std::transform( particles->begin                     () , 
                  particles->end                       () ,
                  particles->begin                     () , 
                  GiGaCnvFunctors::MCParticleResetRefs () ) ;
  //
  typedef SmartRef<MCVertex>               Ref;
  typedef GiGaTrajectory::const_iterator   ITG;
  typedef TrajectoryVector::const_iterator ITC;
  MCVertex miscVertex; /// misc 
  GiGaCnvFunctors::MCVerticesLess  Less  ; 
  GiGaCnvFunctors::MCVerticesEqual Equal ;
  MCVertices::iterator iVertex     = vertices     -> begin();
  TrajectoryVector* tv = trajectories->GetVector();
  for( ITC iTrajectory = tv->begin(); tv->end() != iTrajectory ; ++iTrajectory )
    {
      const G4VTrajectory* vt = *iTrajectory ;
      if( 0 == vt       ) { return Error("G4VTrajectory* points to NULL" ) ; } 
      const GiGaTrajectory*  trajectory = 
        dynamic_cast<const GiGaTrajectory*> ( vt ) ; 
      if( 0 == trajectory ) 
        { return Error("G4VTrajectory*(of type '" + 
                       GiGaUtil::ObjTypeName(vt)+ 
                       "*') could not be cast to GiGaTrajectory*");}
      MCParticle* particle = table( trajectory->trackID() ).particle() ;
      if( 0 == particle ) { return Error("MCParticle* points to NULL!"   ) ; } 
      for( ITG iPoint = trajectory->begin() ; 
           trajectory->end() != iPoint ; ++iPoint )
        {
          if( 0 == *iPoint ) 
            { return Error("GiGaTrajectoryPoint* points to NULL!" ) ; } 
          // auxillary vertex 
          miscVertex.setPosition    ( (*iPoint)->GetPosition() );
          miscVertex.setTimeOfFlight( (*iPoint)->GetTime    () );
          // look for vertex 
          iVertex = 
            std::lower_bound( trajectory -> begin () == iPoint             ? 
                              vertices   -> begin () : iVertex             , 
                              vertices   -> end   () , &miscVertex , Less  ) ;
          // vertex is not found! 
          if ( vertices->end() == iVertex || !Equal( &miscVertex , *iVertex ) ) 
            {  return Error(" appropriate MCVertex is not found!") ; }
          MCVertex* Vertex = *iVertex ;             
          if( 0 == Vertex ) { return Error("MCVertex* points to NULL!") ; }
          //  it is not the first vertex of the trajectory
          if ( trajectory->begin  () != iPoint )           
            { 
              Ref decay( particle , refID , Vertex->key() , Vertex  ) ;
              particle->addToEndVertices ( decay ) ; 
            } 
          // the first vertex and origin is not yet set   
          else if ( !particle->originVertex() )    
            {  
              Ref origin( particle , refID , Vertex->key() , Vertex ) ;
              particle->setOriginVertex( origin ) ;
            } 
          else { return Error("'OriginVertex' is already set!") ; }
        }
    } 
  ///
  return StatusCode::SUCCESS;
  ///
};

// ======================================================================
// End 
// ======================================================================
















































