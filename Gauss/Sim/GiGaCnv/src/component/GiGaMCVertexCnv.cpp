// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.7  2001/07/30 14:13:36  ibelyaev
/// update in GiGaCnvFunctors
///
/// Revision 1.6  2001/07/27 11:24:57  ibelyaev
/// bug fix in MCParticles<->MCVertices relations
///
/// Revision 1.5  2001/07/25 17:19:32  ibelyaev
/// all conversions now are moved from GiGa to GiGaCnv
///
/// Revision 1.4  2001/07/24 11:13:56  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
/// Revision 1.3  2001/07/15 20:45:11  ibelyaev
/// the package restructurisation
/// 
//  ===========================================================================
#define GIGACNV_GIGAMCVERTEXCNV_CPP 1 
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
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
/// GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaTrajectoryPoint.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
/// GiGaCnv
#include "GiGaCnv/GiGaKineAddress.h"
#include "GiGaCnv/GiGaKineRefTable.h"
/// LHCbEvent 
#include "LHCbEvent/MCVertex.h" 
#include "LHCbEvent/MCParticle.h" 
/// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryContainer.hh"
/// Local
#include "GiGaCnvFunctors.h"
#include "Vertex2Vertex.h"
#include "Point2Vertex.h"
///
#include "GiGaMCVertexCnv.h" 

// ============================================================================
/// factories 
// ============================================================================
static const  CnvFactory<GiGaMCVertexCnv>         s_Factory ;
const        ICnvFactory&GiGaMCVertexCnvFactory = s_Factory ;

// ============================================================================
/** standard constructor
 *  @param Loc pointer to service locator 
 */ 
// ============================================================================
GiGaMCVertexCnv::GiGaMCVertexCnv( ISvcLocator* Locator ) 
  : GiGaCnvBase( storageType() , classID() , Locator ) 
{
  ///
  setNameOfGiGaConversionService( "GiGaKineCnvSvc"  ); 
  setConverterName              ( "GiGaMCVCnv"      );
  ///
  /// declare object name for G4->Gaudi conversion 
  declareObject( "/Event/G4/MCVertices", objType() ); 
  ///
}; 

// ============================================================================
/// destructor 
// ============================================================================
GiGaMCVertexCnv::~GiGaMCVertexCnv(){}; 

// ============================================================================
/// Class ID 
// ============================================================================
const CLID&         GiGaMCVertexCnv::classID     () 
{ return ObjectVector<MCVertex>::classID(); }

// ============================================================================
/// StorageType 
// ============================================================================
const unsigned char GiGaMCVertexCnv::storageType ()
 { return GiGaKine_StorageType; }

// ============================================================================
/** initialize the converter
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMCVertexCnv::initialize()
{
  StatusCode sc = GiGaCnvBase::initialize();
  if( sc.isFailure() )
    { return Error("Could not initialize the base class!",sc); }
  ///
  if( 0 == gigaSvc() ) { return Error("IGiGaSvc* points to NULL!"       ) ; }
  if( 0 == cnvSvc () ) { return Error("IConversionSvc* points to NULL!" ) ; }  
  if( 0 == kineSvc() ) { return Error("IGiGaKineSnvSvc* points to NULL!") ; }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** finalize  the converter
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaMCVertexCnv::finalize() 
{ return GiGaCnvBase::finalize() ; };

// ============================================================================
/** create the Object
 *  @param  Address address of the object 
 *  @param  Object  reference to created object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::createObj( IOpaqueAddress*  Address , 
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
                   "*') could not be cast into GiGaKineAddress!" ) ; }
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

// ============================================================================
/** fill the object references 
 *  @param  Address address of the object 
 *  @param  Object  reference to the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::fillObjRefs( IOpaqueAddress*  Address , 
                                         DataObject*   Object  )
{
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!") ; }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!") ; } 
  GiGaKineAddress* address = 
    dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = 
    dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCVertex>*");}  
  ///       
  return updateObjRefs( Address , Object ); 
}; 

// ============================================================================
/** update the Object
 *  @param  Address address of the object 
 *  @param  Object  reference to updated object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateObj( IOpaqueAddress*  Address , 
                                       DataObject*     Object  ) 
{
  ///
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!") ; }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!") ; }   
  GiGaKineAddress* address = 
    dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = 
    dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCVertex>*") ; }  
  ///
  object->erase( object->begin() , object->end() ); 
  ///
  G4TrajectoryContainer* trajectories = 0 ; 
  ///
  try{ *gigaSvc() >> trajectories ; }
  catch( const GaudiException& Excpt ) 
    { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) 
    { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( ... )                         
    { return Exception("UpdateObj: "         ) ; }  
  ///
  if( 0 == trajectories ) 
    { return Error("No G4TrajectoryContainer* object is found!"); } 
  /// convert all trajectory points into MCVertex  objects
  object->reserve( trajectories->size() );
  {  
    /// create "converter"
    Point2Vertex Cnv;
    /// convert points into vertices 
    typedef G4TrajectoryContainer::const_iterator IT;
    for( IT iTr = trajectories->begin() ; 
         trajectories->end() != iTr ; ++iTr ) 
      {
        ///
        const G4VTrajectory* vt = *iTr ;
        if( 0 == vt ) { return Error("G4VTrajectory* points to NULL") ; } 
        const GiGaTrajectory* trajectory = 
          dynamic_cast<const GiGaTrajectory*> ( vt ) ; 
        if( 0 == trajectory ) 
          { return Error("G4VTrajectory*(of type '" + 
                         GiGaUtil::ObjTypeName(vt) + 
                         "*') could not be cast to GiGaTrajectory*"  ) ; }
        /// convert all trajectory points into MCVertices 
        std::transform( trajectory->begin() , trajectory->end() ,
                        std::back_inserter( *object ) , Cnv);
      } 
  }
  /// sort and eliminate duplicates
  {
    GiGaCnvFunctors::MCVerticesLess  Less ;
    std::stable_sort( object->begin() , object->end() , Less );
    
    /** due to close relations between ContainedObject 
     *  and its parent Container we could not use standard 
     *  algorithm std::unique. 
     *  Therefore the following ugly lines are used.  
     */
    typedef ObjectVector<MCVertex>::iterator IT;
    IT     end = object->end   () ; /// unsorted garbage iterator
    for( IT it = object->begin () ;  end != it ; ++it )
      { 
        /// find the first element which is "bigger" - should be fast!
        IT iL = it + 1; 
        IT iU = std::find_if( iL , end , std::bind1st( Less , *it ) ) ;
        if( end == iU ) { end = iL ; }
        else  { std::rotate( iL, iU , end ); end -= (iU-iL) ; }
      }
    object->erase( end , object->end() ) ; ///<  remove unsorted garbage 
  }
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** update the object references 
 *  @param  Address address of the object 
 *  @param  Object  reference to the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateObjRefs( IOpaqueAddress*  Address , 
                                           DataObject*   Object  )
{
  if( 0 ==  Address  ) 
    { return Error("IOpaqueAddress* points to NULL!") ; }
  if( 0 ==  Object   ) 
    { return Error("DataObject* points to NULL!") ; }   
  GiGaKineAddress* address = 
    dynamic_cast<GiGaKineAddress*> (Address);
  if( 0 ==  address  ) 
    { return Error("IOpaqueAddress*(of type '" + 
                   GiGaUtil::ObjTypeName(Address) + 
                   "*') could not be cast into GiGaKineAddress!" ) ; }
  ObjectVector<MCVertex>* object = 
    dynamic_cast<ObjectVector<MCVertex>*> ( Object ); 
  if( 0 ==  object   ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCVertex>*");}  
  /// get the trajectories from GiGa 
  G4TrajectoryContainer* trajectories = 0 ; 
  try{ *gigaSvc() >> trajectories ; }
  catch( const GaudiException& Excpt ) 
    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) 
    { return Exception("UpdateObjRefs: " , Excpt ) ; }  
  catch( ... )                         
    { return Exception("UpdateObjRefs: "         ) ; }  
  if( 0 == trajectories ) 
    { return Error("No G4TrajectoryContainer* object is found!"); } 
  /// get converted MCParticles  
  const std::string ParticlesPath( Address->
                                   directory()->parent()->fullpath() 
                                   + "/MCParticles" );
  SmartDataPtr< ObjectVector<MCParticle> > 
    particles( evtSvc() , ParticlesPath );
  if( !particles ) 
    { return Error("Could not locate Particles at="+ParticlesPath ); }
  const long refID = object->addLink( ParticlesPath ,  particles );
  if( particles->size() != trajectories->size() )
    { return Error("Size of G4TrajectoryContainer mismatch size of " + 
                   ParticlesPath) ; }
  /// reset all existing relations
  std::transform( object->begin() , object->end() , 
                  object->begin() , 
                  GiGaCnvFunctors::MCVertexResetRefs() );
  /// fill relations
  {
    typedef SmartRef<MCParticle> Ref;
    typedef G4TrajectoryContainer::const_iterator ITT ;
    typedef GiGaTrajectory::const_iterator        ITG ;
    typedef ObjectVector<MCParticle>::iterator    ITP ;
    typedef ObjectVector<MCVertex>::iterator      ITV ;
    /// auxillary values 
    MCVertex miscVertex;
    GiGaCnvFunctors::MCVerticesLess  Less  ; 
    GiGaCnvFunctors::MCVerticesEqual Equal ;
    GiGaKineRefTable& table = kineSvc()->table();
    
    ITV iVertex     = object       -> begin() ;
    ITT iTrajectory = trajectories -> begin() ;
    ITP iParticle   = particles    -> begin() ;
    for(  ; trajectories -> end() != iTrajectory , 
            particles    -> end() != iParticle   ; 
          ++iTrajectory , ++iParticle )
      {
	const G4VTrajectory* vt = *iTrajectory ;
	if( 0 == vt  ) { return Error("G4VTrajectory* points to NULL" ) ; } 
	const GiGaTrajectory*  trajectory  = 
          dynamic_cast<const GiGaTrajectory*> ( vt ) ; 
	if( 0 == trajectory  ) 
          { return Error("G4VTrajectory*(of type '" + 
                         GiGaUtil::ObjTypeName( vt ) + 
                         "*') could not be cast to GiGaTrajectory*" ) ; }
        /// own    MCParticle 
	MCParticle* particle  = *iParticle ; 
	if( 0 == particle ) { return Error("MCParticle* points to NULL!" ) ; } 
        /// index of particle 
        const int indxPart = iParticle - particles->begin();
        /// index of mother 
        GiGaKineRefTableEntry& entry = 
          table[ trajectory->parentID() ] ;
        /// index of mother particle (could be -1) 
        const int   iMother = entry.index    () ;
        /// mother MCParticle (could be NULL!)
        MCParticle* mother  = entry.particle () ;
        /// loop over trajectrory points (vertices)  
	for( ITG iPoint = trajectory->begin() ; 
             trajectory->end() != iPoint ; ++iPoint )
	  {
	    if( 0 == *iPoint  ) 
              { return Error("GiGaTrajectoryPoint* points to MULL!" ) ; }  
	    /// fill parameters of auxillary vertex                
	    miscVertex.setPosition    ( (*iPoint) -> GetPosition () );
	    miscVertex.setTimeOfFlight( (*iPoint) -> GetTime     () );
	    /// look for vertex, special treatment for "first" 
            /// vertex. should be quite fast 
	    iVertex = std::lower_bound( trajectory->begin() == iPoint ? 
                                        object->begin() : iVertex , 
                                        object->end() , &miscVertex , Less  ) ;
	    /// no vertex is found? 
            if( object->end() == iVertex || !Equal( &miscVertex , *iVertex ) )  
	      { return Error("appropriate MCVertex is not found !") ; }
            MCVertex* vertex = *iVertex ;
            /// is it the first vertex for track?
            if ( trajectory->begin() == iPoint ) 
	      {
                /// add daughter particle to the vertex 
                Ref dau( vertex , refID , indxPart , particle );
		vertex->addDaughterMCParticle( dau) ; 
                /// mother is known ?            
		if ( !vertex->motherMCParticle() 
                     && 0 != mother && 0 <= iMother )
		  { 
                    Ref moth( vertex , refID , iMother , mother ) ;
                    vertex->setMotherMCParticle ( moth ) ; 
                  }
	      }	       
            /// decay vertex 
            else if ( !vertex->motherMCParticle()  ) 
	      {
                Ref moth( vertex , refID , indxPart , particle );
                vertex->setMotherMCParticle ( moth ) ; 
              }
            /// corrupted data! 
	    else 
	      { return Error("MotherMCParticle is already set!") ; }
	    
	  } ///< end loop over points 
      } ///< end loop over trajectories/particles  
  } ///< end end of relations scope 
  ///
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** create the representation of the object 
 *  @param  Object  reference to the object 
 *  @param  Address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::createRep( DataObject*     Object  , 
                                       IOpaqueAddress*& Address ) 
{
  ///
  Address = 0 ; 
  ///
  if( 0 == Object ){ return Error("CreateRep::DataObject* points to NULL"); } 
  ObjectVector<MCVertex>* obj = 
    dynamic_cast<ObjectVector<MCVertex>*>( Object ) ; 
  if( 0 == obj  ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCVertex>*") ; }  
  /// create IOpaqueAddress
  IAddressCreator* addrCreator = 
    dynamic_cast<IAddressCreator*> ( cnvSvc() ) ; 
  if( 0 == addrCreator   ) 
    { return Error("CreateRep::AddressCreator is not available"); } 
  StatusCode status = addrCreator->createAddress( repSvcType() , 
                                                  classID   () , 
                                                  "GiGaKine"   , 
                                                  "GiGaPrimaryEvent" , 
                                                  -1 , Address );   
  if( status.isFailure() ) 
    { return Error("CreateRep::Error from Address Creator",status) ; }
  if( 0 == Address       ) 
    { return Error("CreateRe::Invalid address is created") ; } 
  ///
  return updateRep( Object , Address ) ; 
  /// 
}; 

// ============================================================================
/** update the representation of the object 
 *  @param  Object  reference to the object 
 *  @param  Address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateRep( DataObject*     Object  , 
                                       IOpaqueAddress*  Address ) 
{ 
  ///
  if( 0 == Object        ) 
    { return Error("UpdateRep::DataObject*     points to NULL"); } 
  if( 0 == Address       ) 
    { return Error("UpdateRep::IopaqueAddress* points to NULL"); } 
  ///
  ObjectVector<MCVertex>* vertices = 
    dynamic_cast<ObjectVector<MCVertex>*>( Object ) ;  
  if( 0 == vertices      ) 
    { return Error("DataObject*(of type '" + 
                   GiGaUtil::ObjTypeName(Object) + 
                   "*') could not be cast into ObjectVector<MCVertex>*") ; }  
  ///
  unsigned int nVertex=0; 
  /// create the vertex converter
  Vertex2Vertex Cnv( ppSvc() );
  /// loop over all vertices and "convert" them 
  typedef ObjectVector<MCVertex>::const_iterator IT;
  for( IT pVertex = vertices->begin() ; 
       vertices->end()  != pVertex ; ++pVertex ) 
    {
      const MCVertex* vertex = *pVertex ; 
      ///  skip artificial NULLS 
      if( 0 == vertex                     )             { continue; } 
      /// find primary MCVertices (without origin MCparticle) 
      if( 0 != vertex->motherMCParticle() )             { continue; } 
      /// skip empty vertices 
      if( vertex->daughterMCParticles().empty() )       { continue; } 
      /// perform the conversion 
      G4PrimaryVertex* Vertex =  Cnv( vertex );
      /// skip epmty 
      if( 0 == Vertex )                                 { continue; }
      ///
      { MsgStream log( msgSvc(),  name() ) ; 
      log << MSG::VERBOSE << "UpdateRep::Add Vertex to GiGa" << endreq; }
      /// add the vertex to GiGa !!!
      *gigaSvc() << Vertex ; 
      ++nVertex ;
      /// 
    }
  ///
  { MsgStream log( msgSvc(),  name() ) ; 
  log << MSG::DEBUG << "UpdateRep::end #" << 
    nVertex << " primary vertices converted " << endreq; }
  ///
  return StatusCode::SUCCESS; 
  ///
}; 

// ============================================================================











































































