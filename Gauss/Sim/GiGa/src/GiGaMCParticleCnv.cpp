

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

/// Geant4 includes
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"

// local 
#include "GiGaCnvFunctors.h"
#include "GiGaMCParticleCnv.h" 

////////////////////////////////////////////////////////////////////////////////////////////////////////
static const  CnvFactory<GiGaMCParticleCnv>                               s_GiGaMCParticleCnvFactory ;
const        ICnvFactory&                      GiGaMCParticleCnvFactory = s_GiGaMCParticleCnvFactory ;
/// constructor 
GiGaMCParticleCnv::GiGaMCParticleCnv( ISvcLocator* Locator ) 
  : GiGaCnv( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaTrajCnvSvc"    ); 
  setConverterName              ( "GiGaMCParticleCnv" );
  ///
  declareObject( "/Event/G4/MCParticles", objType() );
  ///
}; 
/// destructor 
GiGaMCParticleCnv::~GiGaMCParticleCnv(){}; 
/// Class ID
const CLID&         GiGaMCParticleCnv::classID     () { return ObjectVector<MCParticle>::classID(); }
/// StorageType 
const unsigned char GiGaMCParticleCnv::storageType () { return GiGaTraj_StorageType; }  
/// create Object 
StatusCode GiGaMCParticleCnv::createObj( IOpaqueAddress*  Address , DataObject*&  Object )
{
  ///
  Object = 0 ;
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                         ) ; }
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!" ) ; }
  ///
  Object        = new ObjectVector<MCParticle>();
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
/// fill references /////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCParticleCnv::fillObjRefs( IOpaqueAddress*  Address , DataObject*     Object  ) 
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCParticle>* object = dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCParticle>*" ) ; }  
  ///
  return updateObjRefs( Address , Object );
  ///
};
/// update Object /////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCParticleCnv::updateObj( IOpaqueAddress*  Address , DataObject*     Object  ) 
{
  ///
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCParticle>* object = dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCParticle>*" ) ; }  
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
      MCParticle* mp = mcParticle( gt );
      if( 0 == mp ) { Error("GiGaTrajectory could not be converted to MCParticle!" ) ; break ; } 
      object->push_back( mp ) ; 
    } 
  ///
  if( tc->entries() != object->size() )
    {
      object->erase( object->begin() , object->end() ); 
      return Error("Nat all trajectories are converted");
    } 
  ///
  return StatusCode::SUCCESS;
};
/// update  references  //////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCParticleCnv::updateObjRefs( IOpaqueAddress*  Address , DataObject*     Object  ) 
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaTrajAddress* address = dynamic_cast<GiGaTrajAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress* could nto be cast into GiGaTrajAddress!"      ) ; }  
  ObjectVector<MCParticle>* object = dynamic_cast<ObjectVector<MCParticle>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject* could nto be cast into ObjectVector<MCParticle>*" ) ; }  
  /// get trajectories 
  G4TrajectoryContainer* tc = 0 ; 
  try{ *gigaSvc() >> tc ; }
  catch( const GaudiException& Excpt  ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( const std::exception& Excpt  ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( ... )                          { return Exception("UpdateObjRefs: "         ) ; }  
  if( 0 == tc      ) { return Error("No G4TrajectoryContainer* object is found!"); } 
  if( tc->entries() != object->size() ) { return Error("MCParticles and G4Tajectory has different sizes!"); } 
  /// get MCVertices:
  const std::string VerticesPath( Address->directory()->parent()->fullpath() + "/G4MCVertices" );
  SmartDataPtr< ObjectVector<MCVertex> > vertices( evtSvc() , VerticesPath );
  if( !vertices ) { return Error("Could not locate Vertices at="+VerticesPath ); }
  long refID = object->addLink( VerticesPath ,  vertices );
  /// clear all existing references 
  {
    for( ObjectVector<MCParticle>::iterator i = object->begin() ; object->end() != i ; ++i )
      { 
	MCParticle* mcp = *i ; 
	if( 0 == mcp  ) { continue ; } 
	mcp->setOriginMCVertex( 0 ); 
	mcp->removeDecayMCVertices() ; 
      }
  }
  ///
  ObjectVector<MCVertex>::iterator iv = vertices->begin();
  for( unsigned int it = 0 ; it < tc->entries() ; ++it )
    {
      G4VTrajectory* tr = (*tc)[it];
      if( 0 == tr ) { return Error("G4VTrajectory* points to NULL"                        ) ; } 
      GiGaTrajectory*  gt = dynamic_cast<GiGaTrajectory*> ( tr ) ; 
      if( 0 == gt ) { return Error("G4VTrajectory* could not be cast to GiGaTrajectory*"  ) ; }
      MCParticle* mp = *(object->begin()+it);
      if( 0 == mp ) { return Error("MCParticle* points to NULL!"                          ) ; } 
      for( GiGaTrajectory::CI ip = gt->begin() ; gt->end() != ip ; ++ip )
	{
	  GiGaTrajectoryPoint* gp = *ip ; 
	  if( 0 == gp             ) { return Error("GiGaTrajectoryPoint* points to null!" ) ; } 
	  iv = std::find_if( gt->begin() == ip ? vertices->begin() : iv     ,
			     vertices->end() , GiGaCnvFunctors::MCVertexToPointCompare( gp ) );
	  if      ( vertices->end() == iv ) {  return Error("MCVertex is not found!") ; }   
	  else if ( gt->begin  () != ip )           ///  it is not first vertex of the trajectory
	    {  mp->addDecayMCVertex ( SmartRef<MCVertex> ( mp , refID , iv - vertices->begin() , *iv ) ) ; } 
	  else if ( !mp->originMCVertex() )         /// first vertex and origin is not yet set   
	    {  mp->setOriginMCVertex( SmartRef<MCVertex> ( mp , refID , iv - vertices->begin() , *iv ) ) ; } 
          else { return Error("OriginMCVertex is already set!") ; }
	}
    } 
  ///
  return StatusCode::SUCCESS;
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
MCParticle* GiGaMCParticleCnv::mcParticle( const GiGaTrajectory* tr )
{
  ///
  if( 0 == tr )   { Error("GiGaTrajectory* points to NULL!") ; return 0 ; }
  ///
  const G4ParticleDefinition* pd = tr->partDef();
  if( 0 == pd )   { Error("G4ParticleDefinition* is not available for GiGaTrajectory!"); return 0 ; }
  ///q
  ParticleProperty* pp = ppSvc()->findByStdHepID( pd->GetPDGEncoding () ) ; 
  if( 0 == pp )   { pp = ppSvc()->find          ( pd->GetParticleName() ) ; } 
  if( 0 == pp )   { Error("ParticleProperty* could not be located for "+pd->GetParticleName() ) ; return 0 ; }
  ///
  MCParticle* mcp = new MCParticle();
  mcp->setFourMomentum( tr->fourMomentum()          );
  mcp->setParticleID  ( ParticleID( pp->geantID() ) );
  ///
  return mcp ;
  ///
};




















































