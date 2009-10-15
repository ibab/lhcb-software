// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.4  2006/01/31 10:39:33  gcorti
/// adapt to event model
///
/// Revision 1.3  2002/12/07 14:36:25  ibelyaev
///  see $GIGACNVROOT/doc/release.notes
///
/// Revision 1.2  2001/08/12 17:24:50  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
/// 
// ============================================================================
#ifndef GIGACNV_GIGAKINEREFTABLEENTRY_H 
#define GIGACNV_GIGAKINEREFTABLEENTRY_H 1
// ============================================================================
/// STD & STL 
#include <utility>
/// forward declaration
namespace LHCb {
  class MCParticle;
}

/** @class GiGaKineRefTableEntry GiGaKineRefTableEntry.h 
 *  
 *  An auxilalry class to support the relations between
 *  Geant4 track/trajectory identifiers and converted 
 *  MCParticle objects
 *
 *  @author Ivan Belyaev
 *  @date   21/07/2001
 */

class GiGaKineRefTableEntry: private std::pair<LHCb::MCParticle*,int>
{
  
public:
  
  /** constructor 
   *  @param particle pointer to MCParticle object
   *  @param index    its  position in container of MCParticle
   */ 
  GiGaKineRefTableEntry( LHCb::MCParticle* particle =  0 ,
                         const int   index    = -1 )
    : std::pair<LHCb::MCParticle*,int>(particle,index){};
  /// destructor 
  ~GiGaKineRefTableEntry(){}
  
  /** accessor to MC particle pointer  
   *  @return  pointer to MCParticle Object 
   */
  inline       LHCb::MCParticle*& particle()       { return first  ; }
  
  /** accessor to MC particle pointer  (const version)
   *  @return  pointer to MCParticle Object 
   */
  inline const LHCb::MCParticle*  particle() const { return first  ; }
  
  /** accessor to MCParticle index 
   *  @return index  
   */
  inline       int&         index   ()       { return second ; }
  
  /** accessor to MCParticle index  (const version)
   *  @return index  
   */
  inline       int          index   () const { return second ; }
  
};

// ============================================================================
#endif // GIGACNV_GIGAKINEREFTABLEENTRY_H
// ============================================================================
