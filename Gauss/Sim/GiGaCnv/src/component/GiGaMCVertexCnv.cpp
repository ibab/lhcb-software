// $Id: GiGaMCVertexCnv.cpp,v 1.20 2003-01-23 10:06:30 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.19  2002/12/07 21:13:49  ibelyaev
//  bug fix and small CPU performace optimization
//
//  ===========================================================================
#define GIGACNV_GIGAMCVERTEXCNV_CPP 1 
// ============================================================================
// STL 
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <numeric>
#include <functional>
// GaudiKernel
#include "GaudiKernel/CnvFactory.h" 
#include "GaudiKernel/IAddressCreator.h" 
#include "GaudiKernel/IOpaqueAddress.h" 
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IRegistry.h"
// GaudiKernel
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/LinkManager.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ParticleProperty.h"
// GiGa
#include "GiGa/IGiGaSvc.h" 
#include "GiGa/GiGaException.h" 
#include "GiGa/GiGaTrajectoryPoint.h"
#include "GiGa/GiGaTrajectory.h"
#include "GiGa/GiGaUtil.h"
// GiGaCnv
#include "GiGaCnv/GiGaKineRefTable.h"
#include "GiGaCnv/GiGaCnvUtils.h"
// LHCbEvent 
#include "Event/MCVertex.h" 
#include "Event/MCParticle.h" 
// Geant4 includes
#include "G4PrimaryParticle.hh"
#include "G4PrimaryVertex.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "G4TrajectoryContainer.hh"
#include "G4Event.hh"
#include "G4PrimaryParticle.hh"
// Local
#include "GiGaCnvFunctors.h"
#include "Vertex2Vertex.h"
#include "PrimaryVertex2Vertex.h"
#include "Point2Vertex.h"
//
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
  , m_leaf( "" , classID() )
{
  //
  setNameOfGiGaConversionService( IGiGaCnvSvcLocation::Kine ) ; 
  setConverterName              ( "GiGaMCVCnv"              ) ;
  //
  //  declare object name for G4->Gaudi conversion 
  declareObject( GiGaLeaf( MCVertexLocation::Default , objType() ) ); 
  //
}; 

// ============================================================================
// destructor 
// ============================================================================
GiGaMCVertexCnv::~GiGaMCVertexCnv()
{
  m_onepointIDs.clear();
}; 

// ============================================================================
// Class ID 
// ============================================================================
const CLID&         GiGaMCVertexCnv::classID     () 
{ return MCVertices::classID(); }

// ============================================================================
// StorageType 
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
  if( sc.isFailure() ) { return Error("Could not initialize base class!",sc); }
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
 *  @param  address address of the object 
 *  @param  object  reference to created object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::createObj
( IOpaqueAddress*  address , 
  DataObject*&     object  )
{
  ///
  object = 0 ;
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL!" ) ; }
  ///
  object        = new MCVertices();
  StatusCode sc = updateObj( address , object );
  if( sc.isFailure() ) 
    { 
      if( 0 != object ) { delete object ; object =  0 ; }
      return Error(" Could not create the object" , sc );
    }; 
  ///
  return StatusCode::SUCCESS;
};

// ============================================================================
/** fill the object references 
 *  @param  address address of the object 
 *  @param  object  reference to the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::fillObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  )
{
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"     ) ; } 
  MCVertices* vertices = dynamic_cast<MCVertices*> ( object ); 
  if( 0 ==  vertices ) { return Error(" DataObject*(of type '"          + 
                                      GiGaUtil::ObjTypeName( object)    + 
                                      "*') is not 'Vertices*'!"         ) ; }  
  ///       
  return updateObjRefs( address , object ); 
}; 

// ============================================================================
/** update the Object
 *  @param  address address of the object 
 *  @param  object  reference to updated object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateObj
( IOpaqueAddress*  address , 
  DataObject*      object  ) 
{
  ///
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"     ) ; }   
  MCVertices* vertices = dynamic_cast<MCVertices*> ( object ); 
  if( 0 ==  vertices ) { return Error(" DataObject*(of type '"          + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'Vertices*'!"         ) ; }  
  // clear the container before update 
  vertices->clear(); 
  //
  // 1) get all vertices from "Primary Event"
  const G4Event* event  = 0 ;
  try { *gigaSvc() >> event ; }
  catch( const GaudiException& Excpt ) 
    { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( const std::exception& Excpt ) 
    { return Exception("UpdateObj: " , Excpt ) ; }  
  catch( ... )                         
    { return Exception("UpdateObj: "         ) ; }  
  if( 0 == event ) 
    { return Error("No G4Event* object is found!"); }
  
  // 2) get  all vertices from trajectories 
  G4TrajectoryContainer* trajectories = 0 ; 
  //
  try{ *gigaSvc() >> trajectories ; }               // NB!!
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
  vertices->reserve( 2 * trajectories->size() + 10 );
  
  { // convert "primary" vertices 
    // create converter
    GiGaCnvFunctors::PrimaryVertex2Vertex Cnv;
    const G4PrimaryVertex* vertex = event->GetPrimaryVertex();
    while( 0 != vertex ) 
      {
        MCVertex* mcvertex = Cnv( vertex );
        if( 0 != mcvertex ) { vertices->insert( mcvertex ); }
        vertex = vertex->GetNext();
      }
  }
  { // convert "secondary" vertices 
    // create "converter"
    GiGaCnvFunctors::Point2Vertex Cnv;
    // convert points into vertices 
    typedef TrajectoryVector::const_iterator IT;

    // clear the vector of "one-point" trajectories
    m_onepointIDs.clear();
    
    for( IT iTr = trajectories->GetVector()->begin() ; 
         trajectories->GetVector()->end() != iTr ; ++iTr ) 
      {
        //
        const G4VTrajectory* vt = *iTr ;
        if( 0 == vt ) { return Error("G4VTrajectory* points to NULL") ; } 
        const GiGaTrajectory* trajectory = gigaTrajectory( vt ) ; 
        if( 0 == trajectory ) 
          { return Error("G4VTrajectory*(of type '"  + 
                         GiGaUtil::ObjTypeName( vt ) + 
                         "*') could not be cast to GiGaTrajectory*"  ) ; }
  
        // check if the trajectory contains only 1 point
        // if yes, store it in the map, together with 
        // the pointer to the TrajectoryPoint
        if (trajectory->GetPointEntries()==1)
          {
            GiGaTrajectoryPoint* point = 
              dynamic_cast<GiGaTrajectoryPoint*> ( trajectory->GetPoint(0) );

            m_onepointIDs[trajectory->trackID()] = point;
            cout << "Found trajectory with one point " << trajectory->trackID() 
                 << " mother trID " << trajectory->parentID() << endl;
            

          }
        
        for (GiGaTrajectory::const_iterator ittr=trajectory->begin(); 
             ittr!=trajectory->end();++ittr)
          { vertices->insert( Cnv( *ittr ) ); }
      } 
  }
  
  // sort and eliminate duplicates
  {
    GiGaCnvFunctors::MCVerticesLess  Less ;
    std::stable_sort( vertices->begin() , vertices->end() , Less );
    
    /** due to close relations between ContainedObject 
     *  and its parent Container we could not use standard 
     *  algorithm std::unique. 
     *  Therefore the following ugly lines are used.  
     */
    typedef MCVertices::iterator IT;
    IT     end = vertices->end   () ; /// unsorted garbage iterator
    for( IT it = vertices->begin () ;  end != it ; ++it )
      { 
        // find the first element which is "bigger" - should be fast!
        IT iL = it + 1; 
        IT iU = std::find_if( iL , end , std::bind1st( Less , *it ) ) ;
        if( end == iU ) { end = iL ; }
        else  { std::rotate( iL, iU , end ); end -= (iU-iL) ; }
      }
    if( vertices->end()  != end ) 
      {
        /** remove unsorted garbage        
         *  again one could not apply 'simple' methods, since 
         *  the public interface of KeyedContainer class is 
         *  too restrictive
         */
        std::vector<MCVertex*> tmp( vertices->end() - end , (MCVertex *) 0 );
        std::copy( end , vertices->end() , tmp.begin() );
        while( !tmp.empty() )
          { 
            MCVertex* v = tmp.back() ;
            vertices->erase( v ); tmp.pop_back(); 
          }
      }
  }
  //
  return StatusCode::SUCCESS;
};

// ============================================================================
/** update the object references 
 *  @param  address address of the object 
 *  @param  object  reference to the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateObjRefs
( IOpaqueAddress*  address , 
  DataObject*      object  )
{
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; }
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"     ) ; }   
  MCVertices* vertices = dynamic_cast<MCVertices*> ( object ); 
  if( 0 ==  vertices ) { return Error(" DataObject*(of type '"          + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'Vertices*'!"         ) ; }  
  // get the trajectories from GiGa 
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
  // get converted MCParticles
  IRegistry* parent = 
    GiGaCnvUtils::parent( address->registry() , evtSvc() );
  if( 0 == parent ) { return Error( " Parent directory is not accessible!"); }
  const std::string particlesPath( parent->identifier() + "/Particles" );
  SmartDataPtr<MCParticles> particles( evtSvc() , particlesPath );
  if( !particles ) 
    { return Error("Could not locate Particles at=" + particlesPath ); }
  const long refID = object->linkMgr()->addLink( particlesPath ,  particles );
  if( (unsigned) particles->size() != (unsigned) trajectories->size() )
    { return Error("Size of G4TrajectoryContainer mismatch size of '" 
                   + particlesPath + "'") ; }
  // reset all existing relations
  std::transform( vertices -> begin                  () , 
                  vertices -> end                    () , 
                  vertices -> begin                  () , 
                  GiGaCnvFunctors::MCVertexResetRefs () );
  
  
  // fill relations
  {
    typedef SmartRef<MCParticle> Ref;
    typedef TrajectoryVector::const_iterator ITT ;
    typedef GiGaTrajectory::const_iterator        ITG ;
    typedef MCParticles::iterator                 ITP ;
    typedef MCVertices::iterator                  ITV ;
    // auxillary values 
    MCVertex miscVertex;
    GiGaCnvFunctors::MCVerticesLess  Less  ; 
    GiGaCnvFunctors::MCVerticesEqual Equal ;
    // get the references between MCParticles and Geant4 TrackIDs
    GiGaKineRefTable& table = kineSvc()->table();
    TrajectoryVector* tv = trajectories->GetVector();
    ITV iVertex     = vertices     -> begin() ;

    for(ITT iTrajectory = tv->begin(); tv->end() != iTrajectory; ++iTrajectory )
      {
        const G4VTrajectory* vt = *iTrajectory ;
        if( 0 == vt  ) { return Error("G4VTrajectory* points to NULL" ) ; } 
        const GiGaTrajectory*  trajectory  = gigaTrajectory( vt ) ; 
        if( 0 == trajectory  ) 
          { return Error("G4VTrajectory*(of type '" + 
                         GiGaUtil::ObjTypeName( vt ) + 
                         "*') could not be cast to GiGaTrajectory*" ) ; }
        // own MCParticle 
        int trid=trajectory->trackID();
        MCParticle* particle  = table( trid ).particle();
        if( 0 == particle ) { return Error("MCParticle* points to NULL!" ) ; } 

        // index of mother 
        int parid=trajectory->parentID();
        GiGaKineRefTableEntry& entry = table( parid ) ;
        // mother MCParticle (could be NULL!)
        MCParticle* mother  = entry.particle () ;
        // loop over trajectory points (vertices)  
        for( ITG iPoint = trajectory->begin() ; 
             trajectory->end() != iPoint ; ++iPoint )
          {
            if( 0 == *iPoint  ) 
              { return Error("GiGaTrajectoryPoint* points to MULL!" ) ; }  
            // fill parameters of auxillary vertex                
            miscVertex.setPosition    ( (*iPoint) -> GetPosition () );
            miscVertex.setTimeOfFlight( (*iPoint) -> GetTime     () );
            // look for vertex, special treatment for "first" 
            // vertex. should be quite fast 
            iVertex = 
              std::lower_bound( trajectory -> begin () == iPoint ? 
                                vertices   -> begin () :  iVertex     , 
                                vertices   -> end   () ,  &miscVertex , Less  );
            // no vertex is found? 
            if( vertices->end() == iVertex || !Equal( &miscVertex , *iVertex ) )
              { return Error("appropriate MCVertex is not found !") ; }
            MCVertex* vertex = *iVertex ;
            if( 0 == vertex ) { return Error("MCVertex* points to NULL!") ; }
            // is it the first vertex for track?
            if ( trajectory->begin() == iPoint ) 
              {
                // is the parent a trajectory with only one point?
                // if yes do not attach the particle to the vertex
                // it will be treated later (an additional vertex
                // will need to be created first)
                if(m_onepointIDs.end()==m_onepointIDs.find(parid))
                  {
                    // add daughter particle to the vertex 
                    Ref dau( vertex , refID , particle->key() , particle );
                    vertex->addToProducts( dau) ;
                  }
                
                // mother is known ?            
                if ( !vertex->mother() && 0 != mother )
                  { 
                    Ref moth( vertex , refID , mother->key() , mother ) ;
                    vertex->setMother( moth ) ; 
                  }
              }	       
            // decay vertex  ?
            else if ( !vertex->mother()  ) 
              {
                Ref moth( vertex , refID , particle->key() , particle );
                vertex->setMother( moth ) ; 
              }
            // corrupted data! 
            else 
              { return Error("'MotherParticle' is already set!") ; }
	    
          } // end loop over points 
      } // end loop over trajectories

    // now loop through the trajectories which were attached to problematic 
    // particles (i.e. to trajectories with one point only)
    
    for (  std::map<int, GiGaTrajectoryPoint*, less<int> >::iterator 
             miter=m_onepointIDs.begin(); 
           m_onepointIDs.end()!=miter; ++miter)
      {
        // to look at later  <<<<<<<<<<<<<<<<<<<<<<<<<<<<<
        // should I check if it has any daughters? 

        int mytrid=(*miter).first;
        
        // look for the corresponding MCParticle
        MCParticle* mcpart  = table( mytrid ).particle();
        
        // create a new end vertex and set (x,t)        
        SmartRef<MCVertex> endvtx= new MCVertex();
        GiGaTrajectoryPoint* point=(*miter).second;
        
        endvtx->setPosition(point->GetPosition());
        endvtx->setTimeOfFlight(point->GetTime());
        
        // add the new vertex to the container
        vertices->insert( endvtx ); 
        
        // set mother of the new vertex
        Ref moth( endvtx , refID , mcpart->key() , mcpart );
        endvtx->setMother( moth ) ;             
        
        // look through the trajectories, find particles that should 
        // be outgoing from the new vertex and attach them
        // unfortunately a loop over trajectories, due to lack of 
        // a pointer to "daughter trajectory"        

        for(ITT itra=tv->begin(); tv->end() != itra; ++itra)
          {
            if ((*itra)->GetParentID()==mytrid)
              {
                MCParticle* outpart=table((*itra)->
                                          GetTrackID() ).particle();
                
                Ref dau( endvtx , refID , outpart->key() , outpart );
                
                endvtx->addToProducts(dau);
                
              }
          }
      }
  } // end end of relations scope
  //
  return StatusCode::SUCCESS; 
};

// ============================================================================
/** create the representation of the object 
 *  @param  object  reference to the object 
 *  @param  address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::createRep
( DataObject*      object  , 
  IOpaqueAddress*& address ) 
{
  address = 0 ; 
  if( 0 ==  object   ) { return Error(" DataObject* points to NULL"  ); } 
  MCVertices* vertices = dynamic_cast<MCVertices*>( object ) ; 
  if( 0 ==  vertices ) { return Error(" DataObject*(of type '"       + 
                                      GiGaUtil::ObjTypeName( object) + 
                                      "*') is not 'Vertices*'!"      ) ; }  
  // create IOpaqueAddress
  if( 0 == addressCreator() ) 
    { return Error("CreateRep::AddressCreator is not available"); } 
  StatusCode status = 
    addressCreator()->createAddress( repSvcType  () , 
                                     classID     () , 
                                     m_leaf.par  () , 
                                     m_leaf.ipar () , 
                                     address        );   
  if( status.isFailure() ) 
    { return Error("CreateRep::Error from Address Creator",status) ; }
  if( 0 == address       ) 
    { return Error("CreateRe::Invalid address is created") ; } 
  //
  return updateRep( object , address ) ; 
}; 
// ============================================================================

// ============================================================================
/** update the representation of the object 
 *  @param  object  reference to the object 
 *  @param  address address of the object 
 *  @return status code
 */ 
// ============================================================================
StatusCode GiGaMCVertexCnv::updateRep
( DataObject*      object  , 
  IOpaqueAddress*  address ) 
{ 
  // chech arguments 
  if( 0 ==  address  ) { return Error(" IOpaqueAddress* points to NULL" ) ; } 
  if( 0 ==  object   ) { return Error(" DataObject*     points to NULL" ) ; } 
  MCVertices* vertices = dynamic_cast<MCVertices*>( object ) ;  
  if( 0 ==  vertices ) { return Error("DataObject*(of type '"           + 
                                      GiGaUtil::ObjTypeName( object )   + 
                                      "*') is not 'Vertices*'!"         ) ; }  
  // vertex counter
  unsigned int nVertex=0; 
  // create the vertex converter
  Vertex2Vertex Cnv( ppSvc() );
  // loop over all vertices, "convert" them and load them into GiGa 
  typedef MCVertices::const_iterator IT;
  for( IT pVertex = vertices->begin() ; 
       vertices->end()  != pVertex ; ++pVertex ) 
    {
      const MCVertex* vertex = *pVertex ; 
      //  skip artificial NULLS 
      if( 0 == vertex                     )             { continue; } 
      // find primary MCVertices (without origin MCparticle) 
      if( 0 != vertex->mother()           )             { continue; } 
      // skip empty vertices 
      if( vertex->products().empty()      )       { continue; } 
      // perform the conversion 
      G4PrimaryVertex* Vertex =  Cnv( vertex );
      // skip epmty 
      if( 0 == Vertex )                                 { continue; }
      // printout 
      { MsgStream log( msgSvc(),  name() ) ; 
      log << MSG::VERBOSE << "UpdateRep::Add Vertex to GiGa" << endreq; }
      // add the vertex to GiGa !!!
      *gigaSvc() << Vertex ; 
      ++nVertex ;
    }
  //
  { MsgStream log( msgSvc(),  name() ) ; 
  log << MSG::DEBUG << "UpdateRep::end #" << 
    nVertex << " primary vertices converted " << endreq; }
  ///
  return StatusCode::SUCCESS; 
}; 
// ============================================================================

// ============================================================================
// The END 
// ============================================================================











































































