// $Id: AxPart2MCParticleAsct.h,v 1.1 2002-02-08 18:25:42 gcorti Exp $
#ifndef DAVINCITOOLS_AXPART2MCPARTICLEASCT_H
#define DAVINCITOOLS_AXPART2MCPARTICLEASCT_H

// Include files
#include "GaudiTools/Associator.h"
#include <map>


/** @class AxPart2MCParticleAsct AxPart2MCParticleAsct.h DaVinciTools/AxPart2MCParticleAsct.h
 *  An example implementaion of an associator.
 *               This example implements and associator between 
 *                AxPartCandidate and MCParticle. 
 *                This particular implementation retrieves the
 *                link in the data where it is stored (SICb data)
 *                as a 1-to-1 relation. This is a particular 
 *                implementation but an associator can follow 
 *                links in the data or retrieve it if available or
 *                chosen.
 *                It build a reverse map for convenience when
 *                a reverse association is requested the first
 *                time. A method to clean the cache needs to be 
 *                implemented and is called by the Associator
 *                base class when notified a new events is being
 *                processed. 
 *
 *  @uthor Gloria Corti
 *  @date  13/03/2001
 */

class AxPart2MCParticleAsct : public Associator {

public:

/// IAssociator interfaces must be implemented by the concrete Associators
  
  /** Retrieve 1-to-1 direct relation from newer object in processing stage
   *  (AxPartCandidate) to older object in processing stage (MCParticle)
   *  following direction of links between classes. 
   *  In this specific example links between these classes do not exist
   *  because they are only converted and the shortcuts that are stored
   *  in SICb data are retrieved.
   */
  virtual StatusCode i_retrieveDirect( ContainedObject* objFrom, 
                                       ContainedObject*& objTo, 
                                       const CLID idFrom, const CLID idTo );
  
  /** Retrieve 1-to-many direct relation.
   *  This particular implementation is a fake implementation to show how to
   *  use it in the AsctExampleAlgorithm. In SICb data only the 1-to-1 relation
   *  in both direction is possible.
   *  This implementation retrieves the 1-to-1 relation 
   *  that exist and fill a vector with the same associated MCParticle twice
   */
  virtual StatusCode i_retrieveDirect( ContainedObject* objFrom, 
                                       std::vector<ContainedObject*>& vObjTo, 
                                       const CLID idFrom, const CLID idTo );

  /** Retrieve 1-to-1 inverse relation from older object in processing stage
   * (AxPartCandidate) to newer object in processing stage (MCParticle)
   * following the opposite direction of links between classes. 
   * It loops over all direct relation to retrieve the 
   * inverse.
   */
  virtual StatusCode i_retrieveInverse( ContainedObject* objFrom, 
                                        ContainedObject*& objTo, 
                                        const CLID idFrom, const CLID idTo);
  
  /** Retrieve 1-to-many inverse relation.
   *  Since in SICb data only the 1-to-1 relation in both direction is possible
   *  it always return a StatusCode::FAILURE.
   */
  virtual StatusCode i_retrieveInverse( ContainedObject* objFrom, 
                                        std::vector<ContainedObject*>& vObjTo, 
                                        const CLID idFrom, const CLID idTo);


/// Associator Base class overridden methods

  /** Reset the AxPart2MCParticle associator to initial conditions
   *  This method is called by the Associator base class when notified that
   *  a new event is being processed
   */
  virtual StatusCode flushCache();

  /** Build table (in this case a locally kept map) with inverse relations
   *  This example has chosen to keep such a table.
   */
  virtual StatusCode buildInverse();

   
  /** Standard Constructor.
   *  @param type the concrete class of the associator
   *  @param name the full name of the instance
   *  @param parent the "owner" of the associator
   */
  AxPart2MCParticleAsct( const std::string& type,   
                         const std::string& name,  
                         const IInterface* parent ); 
 
  /// Destructor
  ~AxPart2MCParticleAsct();

protected:

/// Additional methods of this specific AxPart2MCParticleAsct

  /** Retrieve direct links from data.
   * This example keeps locally a pointer to a SmartRefTable filled by a
   * converter the first time there is a request for an association.
   * The example could access the data everytime.
   */
  virtual StatusCode buildDirect();

  /// Check if a local direct table has been kept locally
  bool directExist() {  return m_direct; }

  /// Set the value of a flag keeping trace if a direct table for an
  /// event has been built
  void setDirectFlag( bool value ) { m_direct = value; }


private:
  
  typedef RefTable1toN<AxPartCandidate,MCParticle> DirTable;
  typedef std::map< MCParticle*, AxPartCandidate*> InvTable;
   
  DirTable*  m_directTable;  ///< Pointer to SmartRefTable in the event
                             ///   that contains the direct relations
  InvTable  m_inverseTable;  ///< Map with the inverse relations
  bool      m_direct;        ///< Flag for retrieved direct relation
  
}; 

#endif // DAVINCITOOLS_AXPART2MCPARTICLEASCT_H

