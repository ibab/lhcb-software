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
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
/// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa
#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaKineAddress.h"
#include "GiGa/GiGaTrajectoryPoint.h"
/// LHCbEvent 
#include "LHCbEvent/MCVertex.h" 
#include "LHCbEvent/MCParticle.h" 
/// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryContainer.hh"
// Local
#include "GiGaCnvFunctors.h"
#include "ObjTypeName.h"
#include "GiGaMCVertexCnv.h" 
///

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// factories ///////////////////////////////////////////////////////////////////////////////////////////////
static const  CnvFactory<GiGaMCVertexCnv>                               s_GiGaMCVertexCnvFactory ;
const        ICnvFactory&                      GiGaMCVertexCnvFactory = s_GiGaMCVertexCnvFactory ;
/// constructor /////////////////////////////////////////////////////////////////////////////////////////////
GiGaMCVertexCnv::GiGaMCVertexCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaKineCnvSvc"  ); 
  setConverterName              ( "GiGaMCVCnv"      );
  ///
  declareObject( "/Event/G4/MCVertices", objType() ); /// declare object name for G4->Gaudi conversion 
  ///
}; 
/// destructor ///////////////////////////////////////////////////////////////////////////////////////////////
GiGaMCVertexCnv::~GiGaMCVertexCnv(){}; 
/// Class ID /////////////////////////////////////////////////////////////////////////////////////////////////
const CLID&         GiGaMCVertexCnv::classID     () { return ObjectVector<MCVertex>::classID(); }
/// StorageType //////////////////////////////////////////////////////////////////////////////////////////////
const unsigned char GiGaMCVertexCnv::storageType () { return GiGaKine_StorageType; } 
/// create Object ////////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCVertexCnv::createObj( IOpaqueAddress*  Address , DataObject*&  Object )
{
  ///
  Object = 0 ;
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                         ) ; }
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress*(of type '"+ObjTypeName(Address)+"*') could not be cast into GiGaKineAddress!" ) ; }
  ///
  Object        = new ObjectVector<MCVertex>();
  /// 
  StatusCode sc = updateObj( Address , Object );
  if( sc.isFailure() ) 
    { 
      delete Object ;
      Object =  0   ; 
      return Error("Could not create Object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};
/// fill Object's References ///////////////////////////////////////////////////////////////////////////////  
StatusCode GiGaMCVertexCnv::fillObjRefs( IOpaqueAddress*  Address , DataObject*   Object  )
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress*(of type '"+ObjTypeName(Address)+"*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject*(of type '"+ObjTypeName(Object)+"*') could not be cast into ObjectVector<MCVertex>*"   ) ; }  
  ///       
  return updateObjRefs( Address , Object ); 
}; 
/// update Object /////////////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCVertexCnv::updateObj( IOpaqueAddress*  Address , DataObject*     Object  ) 
{
  ///
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress*(of type '"+ObjTypeName(Address)+"*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject*(of type '"+ObjTypeName(Object)+"*') could not be cast into ObjectVector<MCVertex>*"   ) ; }  
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
  if( 0 == tc ) { return Error("No G4TrajectoryContainer* object is found!"); } 
  ///
  for( unsigned int i = 0 ; i < tc->entries() ; ++i )
    {
      ///
      G4VTrajectory* tr = (*tc)[i];
      if( 0 == tr ) { return Error("G4VTrajectory* points to NULL"                        ) ; } 
      GiGaTrajectory*  gt = dynamic_cast<GiGaTrajectory*> ( tr ) ; 
      if( 0 == gt ) { return Error("G4VTrajectory*(of type '"+ObjTypeName(tr)+"*') could not be cast to GiGaTrajectory*"  ) ; }
      for( GiGaTrajectory::const_iterator it = gt->begin() ; gt->end() != it ; ++it )
        {
          GiGaTrajectoryPoint* gp = *it ; 
          if( 0 == gp ) { return Error("GiGaTrajectoryPoint* points to null!") ; } 
          MCVertex* mcv = new MCVertex() ; 
          mcv->setPosition    ( gp->GetPosition() ) ;
          mcv->setTimeOfFlight( gp->GetTime    () ) ; 
          object->push_back( mcv );
        } 
    } 
  ///
  {
    /// sort and eliminate duplicates
    GiGaCnvFunctors::MCVerticesLess  Less ;
    std::stable_sort( object->begin() , object->end() , Less );
    typedef ObjectVector<MCVertex>::iterator IT;
    IT      end = object->end () ; /// unsorted garbage iterator 
    for( IT it = object->begin() ;  end != it ; ++it )
      { 
        IT iL = it + 1; /// find the first element which is "bigger" - should be fast!
        IT iU = std::find_if( iL , end , std::bind1st( Less , *it ) ) ;
        if( end != iU ) { std::rotate( iL, iU , end ); end -= (iU-iL) ; }
      }
    object->erase( end , object->end() ) ; ///  unsorted remove garbage
  }
  ///
  return StatusCode::SUCCESS;
};
/// update Object's References /////////////////////////////////////////////////////////////////////////////// 
StatusCode GiGaMCVertexCnv::updateObjRefs( IOpaqueAddress*  Address , DataObject*   Object  )
{
  if( 0 ==  Address  ) { return Error("IOpaqueAddress* points to NULL!"                              ) ; }
  if( 0 ==  Object   ) { return Error("DataObject* points to NULL!"                                  ) ; }   
  GiGaKineAddress* address = dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) { return Error("IOpaqueAddress*(of type '"+ObjTypeName(Address)+"*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) { return Error("DataObject*(of type '"+ObjTypeName(Object)+"*') could not be cast into ObjectVector<MCVertex>*"   ) ; }  
  /// get trajectories 
  G4TrajectoryContainer* tc = 0 ; 
  try{ *gigaSvc() >> tc ; }
  catch( const GaudiException& Excpt ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( ... )                         { return Exception("UpdateObjRefs: "         ) ; }  
  if( 0 == tc      ) { return Error("No G4TrajectoryContainer* object is found!"); } 
  /// get converted MCParticles  
  const std::string ParticlesPath( Address->directory()->parent()->fullpath() + "/MCParticles" );
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
  MCVertex mV;
  GiGaCnvFunctors::MCVerticesLess  Less  ;
  GiGaCnvFunctors::MCVerticesEqual Equal ;
  for( unsigned int it = 0 ; it < tc->entries() ; ++it )
    {
      G4VTrajectory* tr = (*tc)[it];
      if( 0 == tr  ) { return Error("G4VTrajectory* points to NULL" ) ; } 
      GiGaTrajectory*  gt = dynamic_cast<GiGaTrajectory*> ( tr ) ; 
      if( 0 == gt  ) { return Error("G4VTrajectory*(of type '"+ObjTypeName(tr)+"*') could not be cast to GiGaTrajectory*"  ) ; }
      MCParticle* mcp = (*particles)[it];
      if( 0 == mcp ) { return Error("MCParticle* points to NULL!"   ) ; } 
      for( GiGaTrajectory::const_iterator ip = gt->begin() ; gt->end() != ip ; ++ip )
        {
          if( 0 == *ip  ) { return Error("GiGaTrajectoryPoint* points to null!" ) ; }  
          /// auxillary vertex                
          mV.setPosition    ( (*ip)->GetPosition() );
          mV.setTimeOfFlight( (*ip)->GetTime    () );
          /// look for vertex, special treatment for "first" vertex. should be fast 
          if( gt->begin() != ip ) { iv = std::find_if ( iv , object->end() , std::bind2nd( Equal , &mV ) ) ; } 
          else                    { iv = std::lower_bound( object->begin() , object->end() , &mV , Less  ) ; }
          if ( object->end() == iv || !Equal(&mV,*iv) ) 
            { return Error("appropriate MCVertex is not found!") ; }   
          else if ( gt->begin  () == ip        )  /// first vertex of the trajectory
            {  (*iv)->addDaughterMCParticle( SmartRef<MCParticle> ( *iv  ,  refID , it , mcp ) ) ; }
          else if ( !(*iv)->motherMCParticle() )  /// "decay" vertices 
            { (*iv)->setMotherMCParticle   ( SmartRef<MCParticle> ( *iv  ,  refID , it , mcp ) ) ; }
          else { return Error("MotherMCParticle is already set!") ; }
        }
    } 
  ///
  return StatusCode::SUCCESS; 
}; 
/// create representation /////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCVertexCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object        ) { return Error("CreateRep::DataObject* points to NULL"); } 
  ObjectVector<MCVertex>* obj = 0 ; 
  try        { obj = dynamic_cast<ObjectVector<MCVertex>*>( Object ) ; } 
  catch(...) { obj =                                               0 ; } 
  if( 0 == obj  ) { return Error("DataObject*(of type '"+ObjTypeName(Object)+"*') could not be cast into ObjectVector<MCVertex>*"   ) ; }  
  if( 0 == cnvSvc()      ) { return Error("CreateObj::Conversion SErvice is unavailable"); } 
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 0 ; 
  try        { addrCreator = dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; } 
  catch(...) { addrCreator =                                           0 ; } 
  if( 0 == addrCreator   ) { return Error("CreateRep::AddressCreator is not available"); } 
  StatusCode status = addrCreator->createAddress( repSvcType() , classID() , "GiGaKine" , "GiGaPrimaryEvent" , -1 , Address );   
  if( status.isFailure() ) { return Error("CreateRep::Error from Address Creator",status) ; }
  if( 0 == Address       ) { return Error("CreateRe::Invalid address is created") ; } 
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 
/// update representation /////////////////////////////////////////////////////////////////////////////////
StatusCode GiGaMCVertexCnv::updateRep( DataObject*     Object  , IOpaqueAddress*  Address ) 
{ 
  ///
  if( 0 == Object        ) { return Error("UpdateRep::DataObject*     points to NULL"); } 
  if( 0 == Address       ) { return Error("UpdateRep::IopaqueAddress* points to NULL"); } 
  ///
  ObjectVector<MCVertex>* vertices = 0 ; 
  try        { vertices = dynamic_cast<ObjectVector<MCVertex>*>( Object ) ; } 
  catch(...) { vertices =                                               0 ; } 
  if( 0 == vertices      ) { return Error("DataObject*(of type '"+ObjTypeName(Object)+"*') could not be cast into ObjectVector<MCVertex>*"   ) ; }  
  if( 0 == cnvSvc()      ) { return Error("UpdateRep::Conversion Service is unavailable"       ); } 
  if( 0 == gigaSvc()     ) { return Error("UpdateRep::GiGa Service is unavailable"             ); } 
  if( 0 == ppSvc()       ) { return Error("UpdateRep::ParticleProperty Service is unavailable" ); } 
  ///
  unsigned int nVertex=0; 
  /// loop over all vertices and "convert" them 
  ObjectVector<MCVertex>::const_iterator pVertex = vertices->begin(); 
  while( vertices->end()  != pVertex ) 
    {
      ///
      const MCVertex* vertex = *pVertex++ ; 
      /// 
      if( 0 == vertex                     )             { continue; } 
      ///
      /// find primary MCVertices (without origin MCparticle) 
      if( 0 != vertex->motherMCParticle() )             { continue; } 
      ///
      /// Is the vertex already used? see above concerning the NULL pointers 
      /// if( false == usedVertices.insert( vert ).second ) { continue; } 
      /// Trick - the vertex is not yet used, but we have marked it as "already used"!  :-)))
      ///
      /// empty vertex? 
      if( vertex->daughterMCParticles().empty() )      { continue; } 
      ///
      G4PrimaryVertex* Vertex = VertexFromMCVertex( vertex );
      ///
      { MsgStream log( msgSvc(),  name() ) ; log << MSG::VERBOSE << "UpdateRep::Add Vertex to GiGa" << endreq; }
      ///
      if( 0 != gigaSvc() ) { *gigaSvc() << Vertex ; ++nVertex ; } 
      else                 { return Error("CreateRep::IGiGaSvc* points to NULL!") ; }
      /// 
    }
  ///
  { MsgStream log( msgSvc(),  name() ) ; log << MSG::VERBOSE << "UpdateRep::end " << 
                                           nVertex << " primary vertices converted " << endreq; }
  ///
  return StatusCode::SUCCESS; 
  ///
}; 
////////////////////////////////////////////////////////////////////////////////////////////////////////
G4PrimaryVertex* GiGaMCVertexCnv::VertexFromMCVertex( const MCVertex*   vertex   ) 
{
  /// check for arguments 
  if( 0 == vertex                               )    { return 0 ; } 
  /// do not proceed empty verteces 
  if( 0 == vertex->daughterMCParticles().size() )   { return 0 ; } 
  ///
  G4PrimaryVertex* Vertex = 
    new G4PrimaryVertex( vertex->position().x() , 
                         vertex->position().y() , 
                         vertex->position().z() , 
                         vertex->timeOfFlight() ) ; 
  ///
  SmartRefVector<MCParticle>::const_iterator pParticle = 
    vertex->daughterMCParticles().begin(); 
  while( vertex->daughterMCParticles().end() != pParticle ) 
    {
      G4PrimaryParticle* Particle = ParticleFromMCParticle( *pParticle++ ); 
      if( 0 != Particle ) { Vertex->SetPrimary( Particle ); } 
    } 
  /// remove vertex if empty  
  if( 0 == Vertex->GetNumberOfParticle() ) { delete Vertex ; Vertex = 0 ; } 
  ///
  return Vertex; 
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////
G4PrimaryParticle* GiGaMCVertexCnv::ParticleFromMCParticle( const MCParticle* particle )
{
  /// check for arguments
  if( 0 == particle                  )    { return 0 ; }
  /// who are you? 
  G4ParticleDefinition* pid = ParticleDefinition( particle ); 
  if( 0 ==  pid                      )    { return 0 ; } 
  ///  
  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( pid                           , 
                           particle->fourMomentum().px() , 
                           particle->fourMomentum().py() , 
                           particle->fourMomentum().pz() ); 
  ///
  /// for Decay vertices one CURRENTLY should follow a little bit incorrect way
  /// NB - one loose the information about proper decay time!
  /// But it is a current property of Geant4! 
  /// This piece of code should be modified later
  ///
  SmartRefVector<MCVertex>::const_iterator pVertex = particle->decayMCVertices().begin(); 
  while( particle->decayMCVertices().end() != pVertex ) 
    { 
      const MCVertex* vertex = *pVertex++ ; 
      if( 0 == vertex )             { continue; }
      SmartRefVector<MCParticle>::const_iterator pParticle = 
        vertex->daughterMCParticles().begin(); 
      while( vertex->daughterMCParticles().end() != pParticle ) 
        {
          G4PrimaryParticle* p = ParticleFromMCParticle( *pParticle++ ) ;  /// recursion !!!
          if( 0 != p ) { Particle->SetDaughter( p ); } 
        } 
    }  
  ///
  return Particle;
}; 
///////////////////////////////////////////////////////////////////////////////////////////////////////
G4ParticleDefinition* GiGaMCVertexCnv::ParticleDefinition( const MCParticle* particle )
{
  /// check for argument
  if( 0 == particle ) { throw GiGaException( "Could not determine ParticleProperties of NULL pointer" );}
  const long ID = particle->particleID().id();  /// geant3 ID !! - UGLY!
  if( 0 == ppSvc()  ) { throw GiGaException( "ParticlePropertiesService is unavailable" ); } 
  /// who are you?
  ParticleProperty* PP = ppSvc()->find( ID ) ;  /// find by Geant3 ID - UGLY!
  if( 0 == PP       ) 
    {
      /// ugly code due to absent of ostringstream!
      const unsigned int buflen = 24; 
      char buf[buflen+1]; buf[buflen] = 0 ; buf[0] = 0 ; 
      std::strstream ost( buf, buflen ); 
      ost << ID ; 
      std::string cID = ost.str();  
      std::string message(name()+"ParticleProperty unavailable for Geant3ID=\t");
      message+=cID; 
      throw GiGaException( message );
    }
  ///
  int JetSetID = PP->jetsetID(); 
  G4ParticleTable* table = 0 ;
  table = G4ParticleTable::GetParticleTable(); 
  G4ParticleDefinition * PD = 0 ; 
  PD = table->FindParticle( JetSetID );                            /// locate by JetSetID 
  PD = ( 0 == PD ) ? table->FindParticle( PP->particle()  ) : PD ; /// locate by name 
  if( 0 == PD ) 
    { throw GiGaException( name()+":G4ParticleDefinition is unavailable for Particle=\t"+PP->particle()); }  
  ///
  return PD; 
};
///////////////////////////////////////////////////////////////////////////////////////////////////////












































































