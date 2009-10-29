// $Id: L0AlgBase.h,v 1.2 2009-10-29 10:41:22 robbep Exp $
#ifndef L0ALGBASE_H 
#define L0ALGBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0AlgBase L0AlgBase.h
 *  
 *
 *  @author Julien Cogan
 *  @date   2009-10-20
 */
class L0AlgBase : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0AlgBase( const std::string& name, ISvcLocator* pSvcLocator );
  
  virtual ~L0AlgBase( ); ///< Destructor
  
protected:
  
/** if algorithm should write L0XXXCandidates on TES
 *  The location will be the "default" location + l0context
 *  as suffix
 */
  bool writeOnTES() const { return m_writeOnTES ; } ; 
  
  /** The suffix to append to the default locations where
   *  to store the objects on the TES
   */
  std::string l0context() const { return m_l0context ; } ;

  /** returns the location where to write the data on TES
   *  @param[in] standardLocation "default" path where objects are stored
   */
  std::string dataLocation( const std::string & standardLocation ) const {
    return ( rootInTES() + standardLocation + l0context() ) ; } ;  

  /** If algorithm should write the raw banks it emulates
   */
  bool writeBanks() const { return m_writeBanks ; } ;
  
  bool m_writeBanks;       ///< Flag to activate the writing of the raw banks
  bool m_writeOnTES;       ///< Flag to activate the writing of the Candidates/Report on the TES
  std::string m_l0context; ///< Suffix appended to the default location in TES

private:

};
#endif // L0ALGBASE_H
