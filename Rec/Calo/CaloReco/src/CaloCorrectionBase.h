// $Id: CaloCorrectionBase.h,v 1.1 2010-05-20 09:47:06 odescham Exp $
#ifndef CALOCORRECTIONBASE_H 
#define CALOCORRECTIONBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "DetDesc/Condition.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Kernel/CaloCellID.h"

static const InterfaceID IID_CaloCorrectionBase ( "CaloCorrectionBase", 1, 0 );

/** @class CaloCorrectionBase CaloCorrectionBase.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-05-07
 */



// namespaces
namespace CaloCorrection {
  // correction functions
  enum  Function{
    InversPolynomial=0,
    Polynomial,
    Unknown  // MUST be the last item
  };
  // parameter typedef
  typedef std::pair<CaloCorrection::Function , std::vector<double> > Parameters;
  // correction type
  enum Type{
    alphaG,
    alphaE,
    alphaB,
    alphaX,
    alphaY,
    beta,
    globalC,
    lastType  // MUST BE THE LAST LINE
  };
  static const int nT = lastType+1;
  static const int nF = Unknown+1;
  static const std::string typeName[nT] = { "alphaG", "alphaE","alphaB","alphaX","alphaY","beta","globalC","Unknown"};    
  static const std::string funcName[nF] = { "InversPolynomial", "Polynomial","Unknown"};  
}


class CaloCorrectionBase : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_CaloCorrectionBase; }

  /// Standard constructor
  CaloCorrectionBase( const std::string& type, 
        const std::string& name,
        const IInterface* parent);

  virtual ~CaloCorrectionBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  StatusCode updParams();
protected:
  double getCorrection(CaloCorrection::Type type, const LHCb::CaloCellID id , double var = 0.,double def = 1.) const;
  std::string m_conditionName;
  std::vector<std::string> m_corrections;

private:

  bool accept(std::string name){
    for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
      if( name == *it || *it == "All")return true;
    }    
    return false;
  }

  
  StatusCode setOptParams();
  StatusCode setDBParams();
  CaloCorrection::Parameters getParams(CaloCorrection::Type type, const LHCb::CaloCellID id) const;
  void checkParams();
  
  

  std::map<std::string, std::vector<double> > m_params;
  bool m_useCondDB;
  std::map<std::string, std::vector<double> > m_optParams;
  Condition* m_cond;
};



#endif // CALOCORRECTIONBASE_H
