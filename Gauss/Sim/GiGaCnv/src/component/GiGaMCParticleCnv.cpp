// $Id: GiGaMCParticleCnv.cpp,v 1.10 2002-02-12 17:10:48 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.9  2002/01/22 18:24:44  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.8  2001/11/19 18:27:00  ibelyaev
//  bux fix and the new converter for catalogs
//
// Revision 1.7  2001/08/12 17:24:53  ibelyaev
// improvements with Doxygen comments
//
// Revision 1.6  2001/07/27 11:24:57  ibelyaev
// bug fix in MCParticles<->MCVertices relations
//
// Revision 1.5  2001/07/25 17:19:32  ibelyaev
// all conversions now are moved from GiGa to GiGaCnv
//
// Revision 1.4  2001/07/24 11:13:56  ibelyaev
// package restructurization(III) and update for newer GiGa
//
// Revision 1.3  2001/07/15 20:45:11  ibelyaev
// the package restructurisation
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
/// GaudiKernle
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
#include "LHCbEvent/MCParticle.h" 
/// Geant4 includes
#include "G4TrajectoryContainer.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
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
  setNameOfGiGaConversionService( "GiGaKineCnvSvc" ); 
  setConverterName              ( "GiGaMCPCnv"     );
  ///
  declareObject( GiGaLeaf( "/Event/MC/MCParticles", objType() ) );
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
{ return Particles::classID(); }

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
  object        = new Particles();
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
  Particles* particles = dynamic_cast<Particles*> ( object ); 
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
  Particles* particles = dynamic_cast<Particles*> ( object ); 
  if( 0 ==   particles ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'Particles*'! "   );}  
  /// clear the object 
  particles->erase( particles->begin() , particles->end() ); 
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
      particles->reserve( trajectories->size() );
      /// reserve elements on reference table 
      table.resize( 4 * trajectories->size() );
      /// create the conversion functor 
      Trajectory2Particle Cnv( ppSvc() );
      /// perform the conversion itself  
      for( G4TrajectoryContainer::const_iterator iTr = trajectories->begin() ;
           trajectories->end() != iTr ; ++iTr ) 
        {
          const GiGaTrajectory* trajectory = 
            ( 0 == *iTr ) ? (const GiGaTrajectory*) 0 : 
            dynamic_cast<const GiGaTrajectory*> (*iTr );
          /// convert trajectory into particle and add it into container 
          MCParticle* mcp = Cnv( trajectory );
          particles->push_back( mcp );
          /// fill the reference table 
          table[ trajectory->trackID() ] =
            GiGaKineRefTableEntry( mcp , particles->size() - 1 ) ;
        } 
    }
  catch( const GaudiException& Excp )
    {      
      particles->erase( particles->begin() , particles->end() ); table.clear();
      return Exception("updateObj: " , Excp ) ;
    }  
  catch( const std::exception& Excp )
    {      
      particles->erase( particles->begin() , particles->end() ); table.clear();
      return Exception("updateObj: " , Excp ) ;  
    }
  catch( ...  )
    {      
      particles->erase( particles->begin() , particles->end() ); table.clear();
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
  Particles* particles = dynamic_cast<Particles*> ( object ); 
  if( 0 ==   particles ) { return Error(" DataObject*(of type '"      + 
                                        GiGaUtil::ObjTypeName(object) + 
                                        "*') is not 'Particles*'!"    );}  
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
  if( trajectories->size() != particles->size() ) 
    { return Error(" 'Particles' and G4Tajectories have different sizes!"); }
  /// get the vertices:
  IRegistry* parent  = 
    GiGaCnvUtils::parent( address->registry() , evtSvc() );
  if( 0 == parent ) { return Error( " Parent directory is not accessible!"); }
  const std::string verticesPath( parent->identifier() + "/MCVertices" ) ;
  SmartDataPtr<Vertices> vertices( evtSvc() , verticesPath );
  if( !vertices ) 
    { return Error("Could not locate Vertices in '" + verticesPath + "'" ); }
  const long refID = object->linkMgr()->addLink( verticesPath ,  vertices );
  /// clear all existing references 
  std::transform( particles->begin                     () , 
                  particles->end                       () ,
                  particles->begin                     () , 
                  GiGaCnvFunctors::MCParticleResetRefs () ) ;
  ///
  typedef SmartRef<MCVertex> Ref;
  typedef GiGaTrajectory::const_iterator ITG;
  MCVertex miscVertex; /// misc 
  GiGaCnvFunctors::MCVerticesLess  Less  ; 
  GiGaCnvFunctors::MCVerticesEqual Equal ;
  Vertices::iterator                    iVertex     = vertices     -> begin() ;
  Particles::iterator                   iParticle   = particles    -> begin() ; 
  G4TrajectoryContainer::const_iterator iTrajectory = trajectories -> begin() ;
  for( ; trajectories->end() != iTrajectory && particles->end() != iParticle ; 
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
            std::lower_bound( trajectory -> begin () == iPoint             ? 
                              vertices   -> begin () : iVertex             , 
                              vertices   -> end   () , &miscVertex , Less  ) ;
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
              particle->addToDecayMCVertices ( decay ) ; 
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

// ======================================================================
// End 
// ======================================================================
















































