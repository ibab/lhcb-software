// $Id: L0CondDBProvider.h,v 1.4 2009-07-27 13:43:29 odescham Exp $
#ifndef L0CONDDBPROVIDER_H 
#define L0CONDDBPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "CaloDet/DeCalorimeter.h"
#include "L0Interfaces/IL0CondDBProvider.h"            
/** @class L0CondDBProvider L0CondDBProvider.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2007-10-13
 */
class L0CondDBProvider : public GaudiTool, virtual public IL0CondDBProvider {
public: 
  /// Standard constructor
  L0CondDBProvider( const std::string& type, 
            const std::string& name,
            const IInterface* parent);

  virtual ~L0CondDBProvider( ); ///< Destructor

  StatusCode initialize();
  double scale(unsigned int base);
  double caloEtScale();
  double muonPtScale();
  const std::vector<int> RAMBCID(std::string vsn);
  int RAMBCID(std::string vsn,int bcid);
  
protected:

private:
  DeCalorimeter* m_ecal;
  Condition* m_gain;
  std::map<std::string,std::vector<int> > m_mapRam;
  unsigned int m_cycle;
  std::string m_rams;
};
#endif // L0CONDDBPROVIDER_H
