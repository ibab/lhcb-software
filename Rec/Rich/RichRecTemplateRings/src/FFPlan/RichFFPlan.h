// $Id: RichFFPlan.h,v 1.1.1.1 2009-03-04 12:01:45 jonrob Exp $
#ifndef FFPLAN_RICHFFPLAN_H 
#define FFPLAN_RICHFFPLAN_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"

#include "RichRingRec/IRichFFPlan.h"            // Interface


/** @class RichFFPlan RichFFPlan.h FFPlan/RichFFPlan.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2007-06-15
 */
class RichFFPlan : public RichRingRecToolBase,
                   virtual public IRichFFPlan {
public: 
  /// Standard constructor
  RichFFPlan( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~RichFFPlan( ); ///< Destructor
  void setDimensionPolar(int nx, int ny );
  void InitFFPlan();
  VD ConvertToFF2d( VD A, VD B);
  VVD ConvertToInvFF2d(VD F );
  
  

protected:

private:

  // sizes of forward 2d Real-To-Complex Transform from Log-Polar

  int  m_fNx;     //m_dimx_2drTocFPol
  int  m_fNy;     //m_dimy_2drTocFPol
  int  m_fNxy;    //m_dimxy_2drTocFPol
  int  m_fNyh;    //m_dimhalfy_2drTocFPol
 

  // sizes of inverse 2d Complex-To-Complex Transform to Log-Polar.

  int  m_iNx;   //m_dimx_2dcTocInvPol
  int  m_iNyh;   //m_dimy_2dcTocInvPol
  int  m_iNxyh;  // m_dimxy_2dcTocInvPol


  // Cartisian coord not used in this version, for now.


};
#endif // FFPLAN_RICHFFPLAN_H
