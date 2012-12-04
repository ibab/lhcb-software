#ifndef LBPYTHIA8_LBLHAUP_H 
#define LBPYTHIA8_LBLHAUP_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "LbPythia8/ILHAupFortranTool.h"            // Interface

class LbLHAupFortran ;
class IProductionTool ;

/** @class LbLHAup LbLHAup.h 
 *  
 *  Interface for Pythia6 User Processes
 *
 *  @author Patrick Robbe
 *  @date   2012-12-02
 */
class LbLHAup : public GaudiTool, virtual public ILHAupFortranTool {
public: 
  /// Standard constructor
  LbLHAup( const std::string& type, 
               const std::string& name,
               const IInterface* parent);

  virtual ~LbLHAup( ); ///< Destructor

  /// Initialize function
  virtual StatusCode initialize( ) ;

  /// Finalize function
  virtual StatusCode finalize( ) ;

  /// return pointer to LHA class
  virtual LbLHAupFortran * getLHAupPtr( ) {
    return m_lhaPtr ;
  }

protected:

private:
  IProductionTool * m_productionTool ; ///< Tool to produce user process events

  std::string m_productionToolName ; ///< Name of this tool
  
  /// Pointer to the LHAup class
  LbLHAupFortran * m_lhaPtr ;
};
#endif // LBPYTHIA8_LBLHAUP_H
