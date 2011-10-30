// $Id$
#ifndef BREMADDER_H 
#define BREMADDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
// from LHCb
#include "Kernel/IBremAdder.h"            // Interface
#include "CaloDet/DeCalorimeter.h"

/** @class BremAdder BremAdder.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-10-25
 */
class BremAdder : public GaudiTool, virtual public IBremAdder {
public: 
  /// Standard constructor
  BremAdder( const std::string& type, 
             const std::string& name,
             const IInterface* parent);

  virtual ~BremAdder( ); ///< Destructor
  virtual StatusCode initialize();

  bool addBrem   ( LHCb::Particle* particle ) const {return brem4particle( particle, "add"   ); };
  bool removeBrem( LHCb::Particle* particle ) const {return brem4particle( particle, "remove"); };
protected:

  bool brem4particle( LHCb::Particle* particle , std::string what) const;

private:
  double m_bremCor;
  double m_dllBrem;
  double m_chi2Brem;
  DeCalorimeter* m_calo;  
};
#endif // BREMADDER_H
