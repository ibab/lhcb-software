// $Id: GiGaCheckEventStatus.h,v 1.1 2005-04-20 15:00:06 gcorti Exp $
#ifndef GIGACHECKEVENTSTATUS_H 
#define GIGACHECKEVENTSTATUS_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Forward declarations
// from GiGa
class IGiGaSvc;

/** @class GiGaCheckEventStatus GiGaCheckEventStatus.h
 *  
 *  Algorithm to check processing status of G4Event as set by G4RunManager.
 *  If event was aborted by Geant4 set filtering status of algorithm as
 *  false.
 *
 *  @author Gloria Corti
 *  @date   2005-04-19
 */
class GiGaCheckEventStatus : public GaudiAlgorithm {
public: 
  /// Standard constructor
  GiGaCheckEventStatus( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~GiGaCheckEventStatus( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
  /** Accessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  inline IGiGaSvc* gigaSvc() const {
    return m_gigaSvc; 
  }  

private:

  std::string     m_gigaSvcName ;   ///< Name of GiGaSvc to be used
  IGiGaSvc*       m_gigaSvc     ;   ///< Pointer of GiGaSvc

};
#endif // GIGACHECKEVENTSTATUS_H
