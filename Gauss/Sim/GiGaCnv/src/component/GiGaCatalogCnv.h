// $Id: GiGaCatalogCnv.h,v 1.4 2009-10-15 10:00:14 silviam Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/12/10 14:04:24  ranjard
// v14r0 - fix for Gaudi v13r0
//
// Revision 1.2  2002/01/22 18:24:43  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// Revision 1.1  2001/11/19 18:27:00  ibelyaev
//  bux fix and the new converter for catalogs
//
// ============================================================================
#ifndef GIGACNV_GIGACATALOGCNV_H 
#define GIGACNV_GIGACATALOGCNV_H 1
/// include files 
/// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"
/// forward declarations 
template <class TYPE> 
class CnvFactory;

/** @class GiGaCatalogCnv GiGaCatalogCnv.h
 *
 *  Auxilalry converter for Catalog ("DataObject").
 *
 *  @author  Vanya Belyaev
 *  @date    19/11/2001
 */

class GiGaCatalogCnv :
  public GiGaCnvBase
{
  /** friend factory for instantiation
   */
  friend class CnvFactory<GiGaCatalogCnv>; 
  
protected:
  
  /** Standard Constructor
   *  @param loc pointer to service locator 
   */
  GiGaCatalogCnv( ISvcLocator* loc );
  
  /** destructor, virtual and protected 
   */ 
  virtual ~GiGaCatalogCnv();
  
public:
  
  /** initialize the converter 
   *  @return status code
   */
  virtual StatusCode initialize() ;
  
  /** finalize  the converter 
   *  @return status code
   */
  virtual StatusCode finalize  () ;
 
  /** create the representation of the Object 
   *  @param  Object  object 
   *  @param  Address address 
   *  @return status  code 
   */
  virtual StatusCode createRep
  ( DataObject*      Object  , 
    IOpaqueAddress*& Address ) ;
  
  /** update the representation of Object 
   *  @param Object   object
   *  @param Address  address 
   *  @return status  code 
   */
  virtual StatusCode updateRep
  ( DataObject*      Object  , 
    IOpaqueAddress*  Address ) ;
 
  /** retrieve the class identifier  for created object 
   *  @return class idenfifier for created object 
   */
  static const CLID&          classID();
  
  /** retrieve the storage type for created object 
   *  @return storage type  for created object 
   */
  //static const unsigned char storageType() ; 
  static long storageType() ; 
  
private:

  /** default constructor is private! 
   */
  GiGaCatalogCnv () ;

  /** copy constructor is private!
   *  @param copy "right" object 
   */
  GiGaCatalogCnv            ( const GiGaCatalogCnv& copy ) ;
  
  /** assignement is private!
   *  @param copy "right" object 
   *  @return self-reference
   */
  GiGaCatalogCnv& operator= ( const GiGaCatalogCnv& copy ) ; 

private:
  
  GiGaLeaf m_leaf ;
  
};


// ============================================================================
// The End 
// ============================================================================
#endif // GIGACNV_GIGACATALOGCNV_H
// ============================================================================
