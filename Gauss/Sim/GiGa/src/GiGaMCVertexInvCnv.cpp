#include <string>
#include <vector>
#include <algorithm>
#include <numeric>


#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"

#include "GiGa/GiGaTrajectory.h" 
#include "GiGa/GiGaTrajAddress.h" 

#include "LHCbEvent/MCParticle.h" 
#include "LHCbEvent/MCVertex.h" 

/// Geant4 includes
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// local 
#include "GiGaCnvFunctors.h"
#include "GiGaMCVertexInvCnv.h" 


////////////////////////////////////////////////////////////////////////////////////////////////////////
static const  CnvFactory<GiGaMCVertexInvCnv>                               s_GiGaMCVertexInvCnvFactory ;
const        ICnvFactory&                      GiGaMCVertexInvCnvFactory = s_GiGaMCVertexInvCnvFactory ;
/// constructor 
GiGaMCVertexInvCnv::GiGaMCVertexInvCnv( ISvcLocator* Locator ) 
  : GiGaCnv( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaTrajCnvSvc"    ); 
  setConverterName              ( "GiGaMCVertexInvCnv" );
  ///
  declareObject( "/Event/G4/MCVertices", objType() );
  ///
}; 
/// destructor 
GiGaMCVertexInvCnv::~GiGaMCVertexInvCnv(){}; 
/// Class ID
const CLID&         GiGaMCVertexInvCnv::classID     () { return ObjectVector<MCVertex>::classID(); }
/// StorageType 
const unsigned char GiGaMCVertexInvCnv::storageType () { return GiGaTraj_StorageType; }  
/// create Object 
StatusCode GiGaMCVertexInvCnv::createObj( IOpaqueAddress*  Address , DataObject*&  Object )
{
  ///
  Object = 0 ;
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                         ) ; }
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!" ) ; }
  ///
  Object        = new ObjectVector<MCVertex>();
  /// 
  StatusCode sc = updateObj( Address , Object );
  if( sc.isFailure() ) 
    { 
      delete Object ;
      Object =  0   ; 
      return Error("Could not update Object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};
///
/// fill Object's References  
StatusCode GiGaMCVertexInvCnv::fillObjRefs( IOpaqueAddress*  Address , DataObject*   Object  )
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCVertex>*"   ) ; }  
  ///       
  return updateObjRefs( Address , Object ); 
}; 
/// update Object 
StatusCode GiGaMCVertexInvCnv::updateObj( IOpaqueAddress*  Address , DataObject*     Object  ) 
{
  ///
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCVertex>*"   ) ; }  
  ///
  object->erase( object->begin() , object->end() ); 
  ///
  G4TrajectoryContainer* tc = 0 ; 
  ///
  try{ *gigaSvc() >> tc ; }
  catch( const GaudiException& Excpt ) { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( ... )                         { return Exception("UpdateObj: "         ) ; }  
  ///
  if( 0 == tc      ) { return Error("No G4TrajectoryContainer* object is found!"); } 
  ///
  for( unsigned int i = 0 ; i < tc->entries() ; ++i )
    {
      ///
      G4VTrajectory* tr = (*tc)[i];
      if( 0 == tr ) { Error("G4VTrajectory* points to NULL"                        ) ; break ; } 
      GiGaTrajectory*  gt = dynamic_cast<GiGaTrajectory*> ( tr ) ; 
      if( 0 == gt ) { Error("G4VTrajectory* could not be cast to GiGaTrajectory*"  ) ; break ; }
      for( GiGaTrajectory::CI it = gt->begin() ; gt->end() != it ; ++it )
	{
	  GiGaTrajectoryPoint* gp = *it ; 
          if( 0 == gp ) { Error("GiGaTrajectoryPoint* points to null!") ; continue; } 
	  MCVertex* mv = new MCVertex() ; 
          mv->setPosition    ( gp->GetPosition() ) ;
          mv->setTimeOfFlight( gp->GetTime    () ) ; 
          object->push_back( mv );
	} 
    } 
  ///
  {
    /// eliminate duplicates:
    ObjectVector<MCVertex>::iterator it;
    it = std::unique( object->begin() , object->end() , GiGaCnvFunctors::MCVerticesCompare() ) ;
    object->erase( it , object->end() ); 
    /// remove NULLs:
    it = std::remove( object->begin() , object->end() , (const MCVertex*) 0 ) ;
    object->erase( it , object->end() ); 
  }
  ///
  return StatusCode::SUCCESS;
};
/// update Object's References  
StatusCode GiGaMCVertexInvCnv::updateObjRefs( IOpaqueAddress*  Address , DataObject*   Object  )
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCVertex>*"   ) ; }  
  /// get trajectories 
  G4TrajectoryContainer* tc = 0 ; 
  try{ *gigaSvc() >> tc ; }
  catch( const GaudiException& Excpt ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( ... )                         { return Exception("UpdateObjRefs: "         ) ; }  
  if( 0 == tc      ) { return Error("No G4TrajectoryContainer* object is found!"); } 
  /// get converted MCParticles  
  const std::string ParticlesPath( Address->directory()->parent()->fullpath() + "/G4MCParticles" );
  SmartDataPtr< ObjectVector<MCParticle> > particles( evtSvc() , ParticlesPath );
  if( !particles ) { return Error("Could not locate Particles at="+ParticlesPath ); }
  long refID = object->addLink( ParticlesPath ,  particles );
  if( particles->size() != tc->entries() )
    { return Error("Size of G4TrajectoryContainer is not equal to size of "+ParticlesPath) ; }
  /// reset all existing relations
  {
    for( ObjectVector<MCVertex>::iterator it = object->begin() ; object->end() != it ; ++it )
      { 
        MCVertex* mcv = *it ; 
        if( 0 == mcv  ) { continue ; } 
        mcv->removeDaughterMCParticles(); 
        mcv->setMotherMCParticle( 0 ); 
      }
  }
  ///
  ObjectVector<MCVertex>::iterator iv = object->begin();
  for( unsigned int it = 0 ; it < tc->entries() ; ++it )
    {
      G4VTrajectory* tr = (*tc)[it];
      if( 0 == tr  ) { return Error("G4VTrajectory* points to NULL"                        ) ; } 
      GiGaTrajectory*  gt = dynamic_cast<GiGaTrajectory*> ( tr ) ; 
      if( 0 == gt  ) { return Error("G4VTrajectory* could not be cast to GiGaTrajectory*"  ) ; }
      MCParticle* mcp = (*particles)[it];
      if( 0 == mcp ) { return Error("MCParticle* points to NULL!"                          ) ; } 
      for( GiGaTrajectory::CI ip = gt->begin() ; gt->end() != ip ; ++ip )
	{
	  GiGaTrajectoryPoint* gp = *ip ; 
	  if( 0 == gp             ) { return Error("GiGaTrajectoryPoint* points to null!" ) ; }
	  iv = std::find_if( gt->begin() == ip ? object->begin() : iv     , 
			     object->end() , GiGaCnvFunctors::MCVertexToPointCompare( gp ) ); 
	  if      ( object->end() == iv ) {  return Error("MCVertex is not found!") ; }   
	  else if ( gt->begin  () == ip )           /// first vertex of the trajectory
	    {  (*iv)->addDaughterMCParticle( SmartRef<MCParticle> ( *iv  ,  refID , it , mcp ) ) ; }
	  else if ( !(*iv)->motherMCParticle() )    /// "decay" vertices 
	    { (*iv)->setMotherMCParticle   ( SmartRef<MCParticle> ( *iv  ,  refID , it , mcp ) ) ; }
          else { return Error("MotherMCParticle is already set!") ; }
	}
    } 
  ///
  return StatusCode::SUCCESS; 
}; 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////



















































