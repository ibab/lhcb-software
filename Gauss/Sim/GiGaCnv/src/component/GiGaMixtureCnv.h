// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaMixtureCnv.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef         GIGA_GIGAMixtureCnv_H
#define         GIGA_GIGAMixtureCnv_H  1 
/// from STL 
#include <set>
/// base class from GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
///
template <class TYPE> 
class CnvFactory;
class Material;
class Mixture;
class Element; 
class Isotope; 

/** @class GiGaMixtureCnv GiGaMixtureCnv.h GiGa/GiGaMixtureCnv.h
    
    Converter of Mixture class to Geant4 
    
    @author  Vanya Belyaev
*/

class GiGaMixtureCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMixtureCnv>; 
  /// 
protected: 
  /// Standard Constructor
  GiGaMixtureCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaMixtureCnv();
  ///
public:
  /// Create representation 
  virtual StatusCode createRep(DataObject*     Object  , IOpaqueAddress*& Address ) ;
  /// Update representation 
  virtual StatusCode updateRep(DataObject*     Object  , IOpaqueAddress*  Address ) ; 
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
protected: 
  /// miscellaneous functions which performs the conversion itself
private: 
  ///
  GiGaMixtureCnv           ()                       ; /// no default constructor 
  GiGaMixtureCnv           ( const GiGaMixtureCnv& ); /// no copy
  GiGaMixtureCnv& operator=( const GiGaMixtureCnv& ); /// no assignment 
  ///
};


#endif   //     __GIGA_GEOMCONVERSION_GIGAMixtureCnv_H__



