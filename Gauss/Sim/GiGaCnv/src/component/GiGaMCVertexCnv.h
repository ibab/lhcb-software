// $Id: GiGaMCVertexCnv.h,v 1.9 2004-03-20 20:16:14 ibelyaev Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2003/03/05 15:19:20  ranjard
// v11r2 - fixes for Win32
//
// Revision 1.7  2003/01/23 10:06:30  witoldp
// added handling of shortlived particles
//
// Revision 1.6  2002/05/02 11:57:03  ibelyaev
//  cleanup of header files
//
// ============================================================================
#ifndef GIGACNV_GIGAMCVERTEXCNV_H
#define GIGACNV_GIGAMCVERTEXCNV_H 1 
// ============================================================================
/// STL 
#include <set>
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
#include "GiGaCnv/GiGaLeaf.h" 
/// forward declarations:
template <class TYPE> 
class   CnvFactory;
///
class   G4PrimaryVertex;
class   G4PrimaryParticle; 
class   G4ParticleDefinition; 


/** @class GiGaMCVertexCnv GiGaMCVertexCnv.h
 *    
 *  Converter of Geant4 trajectories into Gaudi MCVertices 
 *  and Gaudi MCVertices into Geant4 G4PrimaryVertices 
 *  
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    22/02/2001
 */

class GiGaMCVertexCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCVertexCnv>; 
  /// 
protected:
  
  /** standard constructor
   *  @param Loc pointer to service locator 
   */ 
  GiGaMCVertexCnv( ISvcLocator* Loc );

  /// Standard (virtual) destructor 
  virtual ~GiGaMCVertexCnv();
  ///
public:

  /** create the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to created object 
   *  @return status code
   */ 
  virtual StatusCode createObj     
  ( IOpaqueAddress*     Address    , 
    DataObject*&        Object     ) ;
  
  /** update the Object
   *  @param  Address address of the object 
   *  @param  Object  reference to updated object 
   *  @return status code
   */ 
  virtual StatusCode updateObj     
  ( IOpaqueAddress*     Address    , 
    DataObject*         Object     ) ; 
  
  /** fill the object references 
   *  @param  Address address of the object 
   *  @param  Object  reference to the object 
   *  @return status code
   */ 
  virtual StatusCode fillObjRefs   
  ( IOpaqueAddress*     Address    , 
    DataObject*         Object     ) ;
  
  /** update the object references 
   *  @param  Address address of the object 
   *  @param  Object  reference to the object 
   *  @return status code
   */ 
  virtual StatusCode updateObjRefs 
  ( IOpaqueAddress*     Address    , 
    DataObject*         Object     ) ;
  
  /** create the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode createRep     
  ( DataObject*         Object     , 
    IOpaqueAddress*&    Address    ) ;
  
  /** update the representation of the object 
   *  @param  Object  reference to the object 
   *  @param  Address address of the object 
   *  @return status code
   */ 
  virtual StatusCode updateRep     
  ( DataObject*         Object     , 
    IOpaqueAddress*     Address    ) ; 
  
  /** class ID for created object = class ID for this specific converter
   *  @return class id of created objects
   */
  static const CLID&          classID();

  /** storage type 
   *  @return storage type 
   */
  static const unsigned char storageType() ; 

  /** initialize the converter
   *  @return status code 
   */
  virtual StatusCode initialize();
  
  /** finalize  the converter
   *  @return status code 
   */
  virtual StatusCode finalize ();
  ///
private:
  ///
  GiGaMCVertexCnv () ; ///< no default constructor 
  GiGaMCVertexCnv ( const GiGaMCVertexCnv& ); ///< no copy
  GiGaMCVertexCnv& operator=( const GiGaMCVertexCnv& ); ///< no assignment  
  ///
private:

  GiGaLeaf m_leaf;

  // map containing IDs of problematic trajectories with only one point

  //  std::vector<int> m_onepointIDs;
  std::map<int, GiGaTrajectoryPoint*, std::less<int> > m_onepointIDs;
  
  // ORDERED vector of hadronic processes 
  std::vector<std::string> m_hadronicProcesses ;

};


// ============================================================================
// End 
// ============================================================================
#endif   //     __GIGA_KINECONVERSION_GIGAMCVERTEXCNV_H__
// ============================================================================














