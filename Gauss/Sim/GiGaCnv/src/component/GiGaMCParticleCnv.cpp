/// ===========================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
/// ===========================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.3  2001/07/15 20:45:11  ibelyaev
/// the package restructurisation
/// 
//  ===========================================================================
#define GIGACNV_GIGAMCPARTICLECNV_CPP 1 
/// ===========================================================================
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
/// GaudiKernle
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa 
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaTrajectory.h" 
#include "GiGa/GiGaUtil.h"
/// GiGaCnv  
#include "GiGaCnv/GiGaKineAddress.h" 
#include "GiGaCnv/GiGaKineRefTable.h" 
/// LHCbEvent
#include "LHCbEvent/MCParticle.h" 
/// Geant4 includes
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
// local 
#include "GiGaCnvFunctors.h"
#include "GiGaMCParticleCnv.h" 
#include "Trajectory2Particle.h"

/** implementation of class GiGaMCParticleCnv 
 *  @author Vanya Belyaev 
 */

/// =====================================================================
/// Factory stuff 
/// =====================================================================
static const  CnvFactory<GiGaMCParticleCnv>         s_Factory ;
const        ICnvFactory&GiGaMCParticleCnvFactory = s_Factory ;

/// =====================================================================
/** Standard Constructor
 *  @param loc pointer to service locator 
 */
/// =====================================================================
GiGaMCParticleCnv::GiGaMCParticleCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaKineCnvSvc" ); 
  setConverterName              ( "GiGaMCPCnv"     );
  ///
  declareObject( "/Event/G4/MCParticles", objType() );
  ///
}; 

/// =====================================================================
/// destructor 
/// =====================================================================
GiGaMCParticleCnv::~GiGaMCParticleCnv(){}; 

/// =====================================================================
/// Class ID
/// =====================================================================
const CLID&         GiGaMCParticleCnv::classID     () 
{ return ObjectVector<MCParticle>::classID(); }

/// =====================================================================
/// StorageType 
/// =====================================================================
const unsigned char GiGaMCParticleCnv::storageType () 
{ return GiGaKine_StorageType; }  

/// =====================================================================
/** initialize the converter 
 *  @return status code
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::initialize()
{
  /// initialize the base class 
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialze the base class!",sc);}
  /// check for nesessary services 
  if( 0 == kineSvc() ) 
    { return Error("IGiGaKineCnvSvc* points to NULL!");}
  if( 0 == ppSvc  () )
    { return Error("IParticlePropertySvc* points to NULL!");}
  ///
  return StatusCode::SUCCESS;
  ///
};

/// =====================================================================
/** finalize  the converter 
 *  @return status code
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::finalize  () 
{
  /// clear the reference table 
  if( 0 != kineSvc() ) { kineSvc()->table().clear();}
  ///
  return GiGaCnvBase::finalize();
};

/// =====================================================================
/** create the Object 
 *  @param Address address 
 *  @param Object object itself 
 *  @return status code 
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::createObj( IOpaqueAddress*  Address ,
                                         DataObject*&  Object )
{
  ///
  Object = 0 ;
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!" ) ; }
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "') could not be cast into GiGaKineAddress!" ) ; }
  ///
  Object        = new ObjectVector<MCParticle>();
  /// 
  StatusCode sc = updateObj( Address , Object );
  if( sc.isFailure() ) 
    { 
      delete Object ;
      Object =  0   ; 
      return Error("Could not create and update Object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};

/// =====================================================================
/** fill the object references 
 *  @param Address address 
 *  @param Object object itself 
 *  @return status code 
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::fillObjRefs( IOpaqueAddress*  Address , 
                                           DataObject*     Object  ) 
{
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!"); }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!") ; }   
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCParticle>* object = 
    dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCParticle>*");}  
  ///
  return updateObjRefs( Address , Object );
  ///
};

/// =====================================================================
/** update the Object 
 *  @param Address address 
 *  @param Object object itself 
 *  @return status code 
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::updateObj( IOpaqueAddress*  Address , 
                                         DataObject*     Object  ) 
{
  ///
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!") ; }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!" ) ; }   
  GiGaKineAddress* address = 
    dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCParticle>* object = 
    dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCParticle>*");}  
  /// clear the object 
  object->erase( object->begin() , object->end() ); 
  /// clear the references between MCParticles and Geant4 TrackIDs
  GiGaKineRefTable& table = kineSvc()->table();
  table.clear();
  /// retrieve the  trajectory container from GiGa Service 
  G4TrajectoryContainer* trajectories = 0 ; 
  try
    { 
      /// get trajectories from GiGa 
      *gigaSvc() >> trajectories ; 
      ///
      if( 0 == trajectories ) 
        { return Error("No G4TrajectoryContainer* object is found!"); } 
      /// reserve elements on object container 
      object->reserve( trajectories->size() );
      /// reserve elements on reference table 
      table.resize( 4 * trajectories->size() );
      /// create the conversion functor 
      Trajectory2Particle Cnv( ppSvc() );
      /// perform the conversion itself  
      for( G4TrajectoryContainer::const_iterator iTr = trajectories->begin() ;
           trajectories->end() != iTr ; ++iTr ) 
        {
          const GiGaTrajectory* trajectory = ( 0 == *iTr ) ? 0 : 
            dynamic_cast<const GiGaTrajectory*> (*iTr );
          MCParticle* mcp = Cnv( trajectory );
          object->push_back( mcp );
          /// fill the reference table 
          table[ trajectory->trackID() ] = 
            GiGaKineRefTableEntry( mcp , object->size() - 1 ) ;
        } 
    }
  catch( const GaudiException& Excp )
    {      
      object->erase( object->begin() , object->end() ); 
      table.clear();
      return Exception("updateObj: " , Excp ) ;
    }  
  catch( const std::exception& Excp )
    {      
      object->erase( object->begin() , object->end() ); 
      table.clear();
      return Exception("updateObj: " , Excp ) ;  
    }
  catch( ...  )
    {      
      object->erase( object->begin() , object->end() ); 
      table.clear();
      return Exception("updateObj: " ) ;  
    }
  ///
  return StatusCode::SUCCESS;
};

/// =====================================================================
/** update the object references 
 *  @param Address address 
 *  @param Object object itself 
 *  @return status code 
 */
/// =====================================================================
StatusCode GiGaMCParticleCnv::updateObjRefs( IOpaqueAddress*  Address , 
                                             DataObject*     Object  ) 
{
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!" ) ; }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!") ; }   
  GiGaKineAddress* address = 
    dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCParticle>* object = 
    dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCParticle>*");}  
  /// get trajectories 
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
  if( trajectories->size() != object->size() ) 
    { return Error("MCParticles and G4Tajectory has different sizes!"); } 
  /// get all MCVertices:
  const std::string VerticesPath( Address->directory()->parent()->fullpath() + 
                                  "/MCVertices" );
  SmartDataPtr<ObjectVector<MCVertex> > vertices( evtSvc() , VerticesPath );
  if( !vertices ) 
    { return Error("Could not locate Vertices at="+VerticesPath ); }
  const long refID = object->addLink( VerticesPath ,  vertices );
  /// clear all existing references 
  std::transform( object->begin() , object->end() ,
                   object->begin() , 
                   GiGaCnvFunctors::MCParticleResetRefs() ) ;
  ///
  typedef SmartRef<MCVertex> Ref;
  typedef GiGaTrajectory::const_iterator ITG;
  MCVertex miscVertex; /// misc 
  GiGaCnvFunctors::MCVerticesLess  Less; 
  GiGaCnvFunctors::MCVerticesEqual Equal;
  ObjectVector<MCVertex>::iterator      iVertex     = vertices     -> begin() ;
  G4TrajectoryContainer::const_iterator iTrajectory = trajectories -> begin() ;
  ObjectVector<MCParticle>::iterator    iParticle   = object       -> begin() ; 
  for( ; trajectories->end() != iTrajectory , 
         object->end() != iParticle ; 
       ++iTrajectory , ++iParticle  )
    {
      const G4VTrajectory* vt = *iTrajectory ;
      if( 0 == vt       ) { return Error("G4VTrajectory* points to NULL" ) ; } 
      MCParticle* particle    = *iParticle ;
      if( 0 == particle ) { return Error("MCParticle* points to NULL!"   ) ; } 
      const GiGaTrajectory*  trajectory = 
        dynamic_cast<const GiGaTrajectory*> ( vt ) ; 
      if( 0 == trajectory ) 
        { return Error("G4VTrajectory*(of type '" + 
                       GiGaUtil::ObjTypeName(vt)+ 
                       "*') could not be cast to GiGaTrajectory*");}
      for( ITG iPoint = trajectory->begin() ; 
           trajectory->end() != iPoint ; ++iPoint )
        {
          if( 0 == *iPoint ) 
            { return Error("GiGaTrajectoryPoint* points to null!" ) ; } 
          /// auxillary vertex 
          miscVertex.setPosition    ( (*iPoint)->GetPosition() );
          miscVertex.setTimeOfFlight( (*iPoint)->GetTime    () );
          /// look for vertex 
	  iVertex = 
            std::lower_bound( trajectory->begin() == iPoint ? 
                              vertices->begin() : iVertex , 
                              vertices->end() , &miscVertex , Less  ) ;
          /// index of the vertex  
          const unsigned int ivIndex = iVertex - vertices->begin() ;
	  /// vertex is not found! 
          if ( vertices->end() == iVertex || !Equal( &miscVertex , *iVertex ) ) 
            {  return Error(" appropriate MCVertex is not found!") ; }
          MCVertex* Vertex = *iVertex ;             
          ///  it is not first vertex of the trajectory
          if ( trajectory->begin  () != iPoint )           
            { 
              Ref decay( particle , refID , ivIndex , Vertex) ;
              particle->addDecayMCVertex ( decay ) ; 
            } 
          /// first vertex and origin is not yet set   
          else if ( !particle->originMCVertex() )    
            {  
              Ref origin( particle , refID , ivIndex , Vertex );
              particle->setOriginMCVertex( origin ) ;
            } 
          else { return Error("OriginMCVertex is already set!") ; }
        }
    } 
  ///
  return StatusCode::SUCCESS;
  ///
};

/// =====================================================================
















































