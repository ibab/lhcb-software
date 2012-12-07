#ifndef MUONTELL1CONFIG_H  
#define MUONTELL1CONFIG_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "MuonDAQ/IMuonTell1Config.h"            // Interface
#include "MuonTell1ConfigData.h"
#include "MuonDet/DeMuonDetector.h"


/** @class MuonTell1Config MuonTell1Config.h
 *  
 *
 *  @author 
 *  @date   2008-02-29
 */
class MuonTell1Config : public GaudiTool, virtual public IMuonTell1Config {
public: 
  /// Standard constructor
  MuonTell1Config( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~MuonTell1Config( ); ///< Destructor
  StatusCode initialize();

  StatusCode configTell1(int Tell1);
protected:

private:
 DeMuonDetector* m_muonDet;
std::vector<std::pair<std::string,long> > TUName;
};
#endif // MUONTELL1CONFIG_H
