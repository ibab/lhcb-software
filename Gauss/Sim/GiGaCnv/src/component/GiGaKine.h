// $Id: GiGaKine.h,v 1.1 2002-12-07 14:36:26 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef      GIGACNV_GiGaKine_H
#define      GIGACNV_GiGaKine_H  1
// ============================================================================
/// from STL
#include <string>
/// from GiGaCnv
#include "GiGaCnv/IGiGaKineCnvSvc.h"
#include "GiGaCnv/GiGaCnvSvcBase.h"
#include "GiGaCnv/GiGaKineRefTable.h"
///
template <class SERVICE>
class SvcFactory;
///
class    IGiGaSvc;
namespace LHCb {
  class    IParticlePropertySvc;
}
class    IDataSelector;

/** @class GiGaKine GiGaKine.h
 *
 *  conversion service  for converting of Gaudi
 *  MCParticle/MCVertex structure into
 *  Geant4 Primary Event conversion of
 *  Geant4 Trajectories into Gaudi MCVertex/MCParticle structure
 *
 *  @author  Vanya Belyaev
 *  @date    07/08/2000
 */

class GiGaKine:
  public          GiGaCnvSvcBase ,
  virtual public IGiGaKineCnvSvc
{
  ///
  friend class SvcFactory<GiGaKine>;
  ///
protected:

  /** standard constructor
   *  @param name  name of the service
   *  @param loc   pointer to service locator
   */
  GiGaKine( const std::string& name ,
                  ISvcLocator* loc );
  /// virtual destructor
  virtual ~GiGaKine();
  ///
public:

  /** initialization
   *  @return status code
   */
  virtual StatusCode            initialize() ;

  /** finalization
   *  @return status code
   */
  virtual StatusCode            finalize  () ;

  /** retrieve the relation table between Geant4 track/trajectory
   *  identifiers and the converted MCParticle objects
   *  @return the reference to relation table
   */
  virtual GiGaKineRefTable&     table()        { return m_table;} ;

  /** access to particle properties service
   *  @return pointer to particle properties service
   */
  virtual LHCb::IParticlePropertySvc* ppSvc () const { return m_ppSvc; }

  /** query the interface
   *  @param ID unique interface identifier
   *  @param II placeholder for interface
   *  @return status code
   */
  virtual StatusCode queryInterface( const InterfaceID& ID ,
                                     void**             II ) ;

private:

  /// name of particle property service
  std::string            m_ppSvcName ;
  /// pointer to particle property service
  LHCb::IParticlePropertySvc*  m_ppSvc     ;
  /// reference table
  GiGaKineRefTable       m_table     ;

};

// ============================================================================
#endif  ///<  GIGACNV_GiGaKine_H
// ============================================================================


























