// $Id: L0FromRawBase.h,v 1.2 2009-10-29 10:41:22 robbep Exp $
#ifndef L0FROMRAWBASE_H 
#define L0FROMRAWBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"


/** @class L0FromRawBase L0FromRawBase.h
 *  Base class for the L0 banks decoding algorithms
 *
 *  @author Julien Cogan
 *  @date   2009-10-20
 */
class L0FromRawBase : public GaudiAlgorithm {
public: 
  /// Standard constructor
  L0FromRawBase( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0FromRawBase( ); ///< Destructor

protected:

/** If algorithm should write the processor data on TES
 */
  bool writeProcData() const { return m_writeProcData ; } ;

/** If algorithm should write the L0XXCandidates or L0DUReport on TES
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

  bool m_writeProcData;    ///< Flag to activate the writing of the processor data
  bool m_writeOnTES;       ///< Flag to activate the writing of the Candidates/Report on the TES
  std::string m_l0context; ///< Suffix appended to the default location in TES

private:

};
#endif // L0FROMRAWBASE_H
