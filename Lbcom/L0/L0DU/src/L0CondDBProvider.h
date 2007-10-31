// $Id: L0CondDBProvider.h,v 1.2 2007-10-31 16:42:10 odescham Exp $
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

protected:

private:
  DeCalorimeter* m_ecal;
  Condition* m_gain;
};
#endif // L0CONDDBPROVIDER_H
