

#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IGiGaSvc.h" 
#include "GaudiKernel/IParticlePropertySvc.h"

#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/ParticleProperty.h"


#include "GiGa/GiGaException.h" 


#include "LHCbEvent/MCVertex.h" 
#include "LHCbEvent/MCParticle.h" 


///
/// Geant4 includes
///
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"


// local 
#include "GiGaMCVertexCnv.h" 


static const  CnvFactory<GiGaMCVertexCnv>                               s_GiGaMCVertexCnvFactory ;
const        ICnvFactory&                      GiGaMCVertexCnvFactory = s_GiGaMCVertexCnvFactory ;


/// constructor 
GiGaMCVertexCnv::GiGaMCVertexCnv( ISvcLocator* Locator ) 
  : GiGaCnv( storageType() , classID() , Locator ) 
{
  setNameOfGiGaConversionService( "GiGaKineCnvSvc"  ); 
  setConverterName              ( "GiGaMCVertexCnv" );
}; 
/// destructor 
GiGaMCVertexCnv::~GiGaMCVertexCnv(){}; 
/// Class ID
const CLID&         GiGaMCVertexCnv::classID     () { return ObjectVector<MCVertex>::classID(); }
/// StorageType 
const unsigned char GiGaMCVertexCnv::storageType () { return GiGaKine_StorageType; } 
/// 
StatusCode GiGaMCVertexCnv::createRep( DataObject*     Object  , IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object        ) { return Error("CreateRep::DataObject* points to NULL"); } 
  ObjectVector<MCVertex>* obj = 0 ; 
  try        { obj = dynamic_cast<ObjectVector<MCVertex>*>( Object ) ; } 
  catch(...) { obj =                                               0 ; } 
  if( 0 == obj  ) { return Error("CreateRep::Bad cast to ObjectVector<MCVertex>"); }
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
/// Update representation 
StatusCode GiGaMCVertexCnv::updateRep( DataObject*     Object  , IOpaqueAddress*  Address ) 
{ 
  ///
  { MsgStream log( msgSvc(),  name() ) ; log << MSG::VERBOSE << "UpdateRep::start" << endreq; }
  ///
  if( 0 == Object        ) { return Error("UpdateRep::DataObject*     points to NULL"); } 
  if( 0 == Address       ) { return Error("UpdateRep::IopaqueAddress* points to NULL"); } 
  ///
  ObjectVector<MCVertex>* vertices = 0 ; 
  try        { vertices = dynamic_cast<ObjectVector<MCVertex>*>( Object ) ; } 
  catch(...) { vertices =                                               0 ; } 
  if( 0 == vertices      ) { return Error("UpdateRep::Bad cast to ObjectVector<MCVertex>"      ); }
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


///
///
///
G4PrimaryVertex* GiGaMCVertexCnv::VertexFromMCVertex( const MCVertex*   vertex   ) 
{
  ///
  /// check for arguments 
  if( 0 == vertex                               )    { return 0 ; } 
  ///
  /// do not proceed empty verteces 
  if( 0 == vertex->daughterMCParticles().size() )   { return 0 ; } 
  ///
  G4PrimaryVertex* Vertex = 
    new G4PrimaryVertex( vertex->position().x() , 
			 vertex->position().y() , 
			 vertex->position().z() , 
			 vertex->timeOfFlight() ) ; 
  ///
  ///
  SmartRefVector<MCParticle>::const_iterator pParticle = 
    vertex->daughterMCParticles().begin(); 
  while( vertex->daughterMCParticles().end() != pParticle ) 
    {
      ///
      G4PrimaryParticle* Particle = ParticleFromMCParticle( *pParticle++ ); 
      ///
      if( 0 != Particle ) { Vertex->SetPrimary( Particle ); } 
    } 
  ///
  /// remove vertex if empty  
  if( 0 == Vertex->GetNumberOfParticle() ) { delete Vertex ; Vertex = 0 ; } 
  ///
  return Vertex; 
  ///
};

///
///
/// 

G4PrimaryParticle* GiGaMCVertexCnv::ParticleFromMCParticle( const MCParticle* particle )
{
  ///
  /// check for arguments
  ///
  if( 0 == particle                  )    { return 0 ; }
  ///

  ///
  /// who are you? 
  ///
  G4ParticleDefinition* pid = ParticleDefinition( particle ); 
  ///
  if( 0 ==  pid                      )    { return 0 ; } 
  ///
  
  G4PrimaryParticle* Particle = 
    new G4PrimaryParticle( pid                           , 
			   particle->fourMomentum().px() , 
			   particle->fourMomentum().py() , 
			   particle->fourMomentum().pz() ); 
  ///
  /// for Decay vertices one CURRENTLY shoudl follow a little bit incorrect way
  /// NB - one loose the informtion about proper decay time!
  /// But it is a current property of Geant4! 
  /// This piece of code should be modified later
  ///
  SmartRefVector<MCVertex>::const_iterator pVertex = particle->decayMCVertices().begin(); 
  while( particle->decayMCVertices().end() != pVertex ) 
    { 
      ///
      const MCVertex* vertex = *pVertex++ ; 
      ///
      if( 0 == vertex )             { continue; }
      ///
      SmartRefVector<MCParticle>::const_iterator pParticle = 
	vertex->daughterMCParticles().begin(); 
      while( vertex->daughterMCParticles().end() != pParticle ) 
	{
	  ///
	  G4PrimaryParticle* p = ParticleFromMCParticle( *pParticle++ ) ;  /// recursion !!!
	  ///
	  if( 0 != p ) { Particle->SetDaughter( p ); } 
	  ///
	} 
    }  
  ///
  ///
  return Particle;
  /// 
} 


G4ParticleDefinition* GiGaMCVertexCnv::ParticleDefinition( const MCParticle* particle )
{
  ///
  /// check for argument
  if( 0 == particle ) { throw GiGaException( "Could not determine ParticleProperties of NULL pointer" );}
  ///
  const long ID = particle->particleID().id(); 
  ///
  if( 0 == ppSvc()  ) { throw GiGaException( "ParticlePropertiesService is unavailable" ); } 
  ///
  ParticleProperty* PP = ppSvc()->find( ID ) ;  /// find by Geant3 ID - ugly!!!
  ///
  if( 0 == PP       ) 
    {
      /// ugly code due to absent of ostringstream!
      const unsigned int buflen = 24; 
      char buf[buflen+1]; buf[buflen] = 0 ; buf[0] = 0 ; 
      std::strstream ost( buf, buflen ); 
      ost << ID ; 
      std::string cID = ost.str();  
      /// 
      std::string message("ParticlePropertiy unavailable for Geant3ID=\t");
      message+=cID; 
      throw GiGaException( message );
      ///
    }
  ///
  int JetSetID = PP->jetsetID(); 
  ///
  G4ParticleTable* table = 0 ;
  table = G4ParticleTable::GetParticleTable(); 
  ///
  G4ParticleDefinition * PD = 0 ; 
  PD = table->FindParticle( JetSetID );
  ///
  if( 0 == PD ) 
    {
      /// ugly code due to absent of ostringstream!
      const unsigned int buflen = 24; 
      char buf[buflen+1]; buf[buflen] = 0 ; buf[0] = 0 ; 
      std::strstream ost( buf, buflen ); 
      ost << JetSetID ; 
      std::string cID = ost.str();  
      /// 
      std::string message("G4ParticleDefinition is unavailable for JetSetID=\t");
      message+=cID; 
      throw GiGaException( message );
      ///
    }  
  ///
  return PD; 
  ///
};












































































