// $Id: DecodeSimpleDecayString.h,v 1.7 2008-03-31 13:03:01 ibelyaev Exp $
// ============================================================================
#ifndef DECODESIMPLEDECAYSTRING_H 
#define DECODESIMPLEDECAYSTRING_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgTool.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/Decay.h"            
#include "Kernel/IDecodeSimpleDecayString.h"            // Interface
// ============================================================================
// forwarde declaration
// ============================================================================
class IParticlePropertySvc;
// ============================================================================
/** @class DecodeSimpleDecayString DecodeSimpleDecayString.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-30
 */
class DecodeSimpleDecayString
  : public GaudiTool, 
    virtual public IDecodeSimpleDecayString 
{
public: 
  /// Standard constructor
  DecodeSimpleDecayString( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  virtual ~DecodeSimpleDecayString( ); ///< Destructor
  /// Initialize method 
  StatusCode initialize();  
  StatusCode setDescriptor(const std::string&);
  std::string getDescriptor(void) const {return m_descriptor;} ;
  StatusCode getStrings(std::string&, strings&) const;
  StatusCode getStrings_cc(std::string&, strings&) const;
  StatusCode getPIDs(int&, ints&) const;
  StatusCode getPIDs_cc(int&, ints&) const;
  bool is_cc(void) const;
  
public:
  
  // ==========================================================================
  /** get the decay form the descriptor 
   *  @param decay (output) the decay 
   *  @return status code 
   */
  virtual StatusCode getDecay    
  ( DaVinci::Decay& decay ) const  ;
  // ==========================================================================
  /** get the charge conjugated decay form the descriptor 
   *  @param decay (output) the decay 
   *  @return status code 
   */
  virtual StatusCode getDecay_cc 
  ( DaVinci::Decay& decay ) const ;
  // ==========================================================================
  /** get all decays form the descriptor 
   *  @param decays (output) the vector of decays
   *  @return status code 
   */
  virtual StatusCode getDecays   
  ( std::vector<DaVinci::Decay>& decays ) const ;
  // ==========================================================================
  
protected:
  StatusCode reset();
  StatusCode PID(const std::string&, int&) const;
  StatusCode splitDescriptor(const std::string&,std::string&,
                                         strings&) const;
  StatusCode strip_cc(void) ;
  StatusCode buildPIDs(const std::string,const strings,int&,ints&) const;
  StatusCode do_cc(void) ;
  std::string conjugate(const std::string&)const;
  
  
private:

  std::string m_descriptor;
  std::string m_mother;
  strings m_daughters;
  std::string m_mother_cc;
  strings m_daughters_cc;
  IParticlePropertySvc* m_ppSvc;
  bool m_iscc;

};
#endif // DECODESIMPLEDECAYSTRING_H
