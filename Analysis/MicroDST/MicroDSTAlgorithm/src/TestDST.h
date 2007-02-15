// $Id: TestDST.h,v 1.1.1.1 2007-02-15 14:07:05 ukerzel Exp $
#ifndef TESTDST_H 
#define TESTDST_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include <GaudiKernel/IRegistry.h>

/** @class TestDST TestDST.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2007-02-06
 */
class TestDST : public DVAlgorithm {
public: 
  /// Standard constructor
  TestDST( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestDST( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  StatusCode  PrintDefaultPV();
  inline std::string objectLocation(const DataObject *pObject) const 
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }


  bool        m_printPVtracks;            ///< print information about tracks at PV
  bool        m_printCaloElectrons;
  bool        m_printCaloPRS;     

};
#endif // TESTDST_H
