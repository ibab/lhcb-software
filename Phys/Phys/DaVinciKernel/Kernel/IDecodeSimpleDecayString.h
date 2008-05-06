// $Id: IDecodeSimpleDecayString.h,v 1.4 2008-05-06 08:19:21 cattanem Exp $
// ============================================================================
#ifndef KERNEL_IDECODESIMPLEDECAYSTRING_H 
#define KERNEL_IDECODESIMPLEDECAYSTRING_H 1
// ============================================================================
// Include files
// ============================================================================
// STS&STL
// ============================================================================
#include <string>
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/IAlgTool.h"
// ============================================================================
// forward declarations
// ============================================================================
namespace LHCb { class Decay ; }
// ============================================================================
/// the helper type for vector of strings
typedef std::vector<std::string> strings;
/// the helper type for vector of integers
typedef std::vector<int> ints;
// ============================================================================
/** @class IDecodeSimpleDecayString 
 *  IDecodeSimpleDecayString.h Kernel/IDecodeSimpleDecayString.h
 *  
 *
 *  @author Patrick KOPPENBURG
 *  @date   2004-06-30
 */
class IDecodeSimpleDecayString : virtual public IAlgTool 
{
public:
  // ==========================================================================
  virtual StatusCode  setDescriptor(const std::string&) = 0;
  virtual std::string getDescriptor(void) const = 0;
  virtual StatusCode  getStrings(std::string&, strings&) const = 0;
  virtual StatusCode  getPIDs(int&, ints&) const = 0;
  virtual StatusCode  getStrings_cc(std::string&, strings&) const = 0;
  virtual StatusCode  getPIDs_cc(int&, ints&) const = 0;
  virtual bool is_cc(void) const = 0;
  // ==========================================================================
public:
  // ==========================================================================
  /** get the decay form the descriptor 
   *  @param decay (output) the decay 
   *  @return status code 
   */
  virtual StatusCode getDecay ( LHCb::Decay& decay ) const  = 0 ;
  // ==========================================================================
  /** get the charge conjugated decay form the descriptor 
   *  @param decay (output) the decay 
   *  @return status code 
   */
  virtual StatusCode getDecay_cc ( LHCb::Decay& decay ) const  = 0 ;
  // ==========================================================================
  /** get all decays form the descriptor 
   *  @param decays (output) the vector of decays
   *  @return status code 
   */
  virtual StatusCode getDecays ( std::vector<LHCb::Decay>& decays ) const = 0 ;
  // ==========================================================================
public:
  // ==========================================================================
  /// Return the unique interface ID
  static const InterfaceID& interfaceID() ;
  // ==========================================================================
protected:
  // ==========================================================================
  /// virtual & protected desctructor 
  virtual ~IDecodeSimpleDecayString() ; // virtual & protected desctructor 
  // ==========================================================================
};
// ============================================================================
// The END 
// ============================================================================
#endif // KERNEL_IDECODESIMPLEDECAYSTRING_H
// ============================================================================
