// $Id: CaloExtraDigits.h,v 1.8 2009-04-16 16:10:11 odescham Exp $
// ============================================================================
#ifndef CALORECO_CALOEXTRADIGITS_H 
#define CALORECO_CALOEXTRADIGITS_H 1
// Include files
// from STL
#include <string>
// Kernel 
#include "GaudiAlg/GaudiTool.h"
#include "CaloInterfaces/ICaloHypoTool.h"
#include "CaloInterfaces/ICaloHypo2Calo.h"

/** @class CaloExtraDigits CaloExtraDigits.h
 *  
 *
 *  @author Vanya Belyaev Ivan Belyaev
 *  @date   31/03/2002
 */
class CaloExtraDigits : 
  public virtual     ICaloHypoTool ,
  public                  GaudiTool 
{
  /// friend factory for instantiation 
  friend struct ToolFactory<CaloExtraDigits>;

public:
  
  virtual StatusCode initialize () ;
  virtual StatusCode process    ( LHCb::CaloHypo* hypo  ) const ;
  virtual StatusCode operator() ( LHCb::CaloHypo* hypo  ) const ;
  
protected:

  CaloExtraDigits( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);
  
  /// Destructor (virtual and protected)
  virtual ~CaloExtraDigits();

private:
  
  /// default constructor is private 
  CaloExtraDigits();
  /// copy constructor is private
  CaloExtraDigits( const CaloExtraDigits& );
  /// assignement operator is private
  CaloExtraDigits& operator=( const CaloExtraDigits& );
  
private:

  std::vector<std::string> m_toDet;
  std::map<std::string,ICaloHypo2Calo*> m_toCalo;
  std::string m_det;
};
#endif // CALOEXTRADIGITS_H
