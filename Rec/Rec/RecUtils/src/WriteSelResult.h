// $Id: WriteSelResult.h,v 1.1 2007-09-07 12:21:14 pkoppenb Exp $
#ifndef WRITESELRESULT_H 
#define WRITESELRESULT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IWriteSelResult.h"            // Interface


/** @class WriteSelResult WriteSelResult.h
 *  
 *  Writes SelResults to SlResults container. If the container does not exist
 *  it is created.
 *
 *  @author Patrick Koppenburg
 *  @date   2007-09-07
 */
class WriteSelResult : public GaudiTool, virtual public IWriteSelResult {
public: 
  /// Standard constructor
  WriteSelResult( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~WriteSelResult( ); ///< Destructor

  /// write a SelResult 
  StatusCode write(LHCb::SelResult&) ;

  /// create and write a SelResult from algo name and result             
  StatusCode write(std::string name, bool result, std::string decay="") ;

protected:

private:

  /// get pointer. Create if needed
  LHCb::SelResults*  getSelResults();

  std::string m_location; ///< container location
  
  std::string m_node ; ///< node of location (e.g. /Event/Phys/)
  

};
#endif // WRITESELRESULT_H
