// $Id: GiGaLAssemblyCnv.h,v 1.2 2009-10-15 10:01:28 silviam Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:44  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//  
// ============================================================================
#ifndef GIGACNV_GIGALASSEMBLYCNV_H 
#define GIGACNV_GIGALASSEMBLYCNV_H 1
// Include files
// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
#include "GiGaCnv/GiGaLeaf.h"
// forward declarations
template <class CONVERTER>
class CnvFactory ;

/** @class GiGaLAssemblyCnv GiGaLAssemblyCnv.h 
 *  
 *  The specific converter for LAssembly objects 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   19/01/2002
 */

class GiGaLAssemblyCnv: public GiGaCnvBase 
{
  /** friend factory for instantiation
   */
  friend class CnvFactory<GiGaLAssemblyCnv>;
  
protected:
  
  /** Standard constructor
   *  @param Locator pointer to service locator 
   */
  GiGaLAssemblyCnv( ISvcLocator* Locator );
  
  /** destructor 
   */
  virtual ~GiGaLAssemblyCnv();
  
public:
    /** create the representation]
   *  @param Object pointer to object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode createRep
  ( DataObject*      Object  , 
    IOpaqueAddress*& Address ) ;
  
  /** Update representation 
   *  @param Object pointer to object 
   *  @param Address address 
   *  @return status code 
   */
  virtual StatusCode updateRep
  ( DataObject*      Object  , 
    IOpaqueAddress*  Address ) ; 
  
  /** class ID for converted objects
   */
  static const CLID&         classID();
  
  /** storage Type 
   */
  static unsigned char storageType() ; 
  
private:
  
  /** The default constructor is private!
   */
  GiGaLAssemblyCnv();

  /** The copy constructor is private!
   *  @param copy 
   */
  GiGaLAssemblyCnv
  ( const GiGaLAssemblyCnv& copy );
  
  /** The assignement operator is private!
   *  @param copy 
   */
  GiGaLAssemblyCnv& operator=
  ( const GiGaLAssemblyCnv& copy );

private:
  
  GiGaLeaf m_leaf;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // COMPONENT_GIGALASSEMBLYCNV_H
// ============================================================================
