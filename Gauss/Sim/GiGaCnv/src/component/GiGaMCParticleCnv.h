// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Sim/GiGaCnv/src/component/GiGaMCParticleCnv.h,v 1.1.1.1 2001-04-23 08:34:15 ibelyaev Exp $ 
#ifndef        GIGA_GIGAMCParticleCNV_H
#define        GIGA_GIGAMCParticleCNV_H  1 
/// STL 
#include <set>
/// GiGaCnv
#include "GiGaCnv/GiGaCnvBase.h" 
template <class TYPE> 
class CnvFactory;
///
class MCParticle;
class GiGaTrajectory;
///

/** @class GiGaMCParticleCnv GiGaMCParticleCnv.h GiGa/GiGaMCParticleCnv.h

    Converter of Geant4 trajectories into Gaudi MCParticles

    @author  Vanya Belyaev
    @date    22/02/2001
*/


class GiGaMCParticleCnv: public GiGaCnvBase
{
  ///
  friend class CnvFactory<GiGaMCParticleCnv>; 
  /// 
 protected: 
  /// Standard Constructor
  GiGaMCParticleCnv( ISvcLocator* );
  /// Standard (virtual) destructor 
  virtual ~GiGaMCParticleCnv();
  ///
 public:
  ///
  /// create Object 
  virtual StatusCode createObj     ( IOpaqueAddress* /* Address */, DataObject*&     /* Object  */) ;
  /// update Object 
  virtual StatusCode updateObj     ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ; 
  /// fill the references 
  virtual StatusCode fillObjRefs   ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ;
  /// update the references 
  virtual StatusCode updateObjRefs ( IOpaqueAddress* /* Address */, DataObject*      /* Object  */) ;
  /// Class ID for created object == class ID for this specific converter
  static const CLID&          classID();
  /// storage Type 
  static const unsigned char storageType() ; 
  ///
private:
  ///
  GiGaMCParticleCnv           ()                           ; /// no default constructor 
  GiGaMCParticleCnv           ( const GiGaMCParticleCnv& ) ; /// no copy 
  GiGaMCParticleCnv& operator=( const GiGaMCParticleCnv& ) ; /// no assignement
  ///
private: 
  ///
  MCParticle* mcParticle( const GiGaTrajectory* ) ; 
  ///
};


#endif   //     __GIGA_KINECONVERSION_GIGAMCParticleCNV_H__














