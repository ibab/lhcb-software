// $Id: BlindVertexFitter.cpp,v 1.1.1.1 2008-02-20 15:48:44 ibelyaev Exp $
// ============================================================================
// Incldue files 
// ============================================================================
// local 
// ============================================================================
#include "VertexFitter.h"
// ============================================================================
/** @class BlindVertexFitter  BlindVertexFitter.cpp  
 *  the most trivial implementation of Kalman-filter based vertex fitter.
 *  It should be very efficient (CPU).
 * 
 *  @see LoKi::VertexFitter
 *  @todo proper implementation of IVertexFit:remove 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-05-24
 */
// ============================================================================
class BlindVertexFitter : public LoKi::VertexFitter 
{
  friend class ToolFactory<BlindVertexFitter> ;
protected:
  // ==========================================================================
  /// standard constructor 
  BlindVertexFitter 
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) 
    : LoKi::VertexFitter ( type , name , parent )
  {} 
  // protected destrcutor
  virtual ~BlindVertexFitter() {}
  // ==========================================================================
private:
  // ==========================================================================
  BlindVertexFitter() ; // the default constructor is disabled 
  BlindVertexFitter( const BlindVertexFitter& );// copy is disabled 
  BlindVertexFitter& operator=( const BlindVertexFitter& ); // disabled 
  // ==========================================================================
} ;
// ============================================================================
DECLARE_TOOL_FACTORY ( BlindVertexFitter ) ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================

