// $Id: RichFFPlan.h,v 1.3 2009-06-05 17:21:32 jonrob Exp $
#ifndef FFPLAN_RICHFFPLAN_H
#define FFPLAN_RICHFFPLAN_H 1

// Include files
// from Gaudi

#include "RichRingRec/RichRingRecToolBase.h"

#include "RichRingRec/IRichFFPlan.h"            // Interface

#include "fftw3.h"

namespace Rich
{
  namespace Rec
  {
    namespace TemplateRings
    {

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
        void InitFFForwardPlan();
        void InitFFInvPlan();
        void ResetFFTWForwardPlanArrays();
        void ResetFFTWInvPlanArrays();
        void ReleaseFFTWArrays();



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

        double* m_Input2drTocF;    //array input for Forward FF transform
        fftw_complex * m_Output2drTocF; // array output for Forward FF transform
        fftw_plan  m_plan2drTocForward;  //Plan for forward FF transform

        fftw_complex* m_Input2dcTocInverse; // array input for Inverse FF transform
        fftw_complex* m_Output2dcTocInverse; // array output for Inverse FF transform
        fftw_plan m_plan2dcTocInverse;  // Plan for Inverse FF transform


      };

    }
  }
}

#endif // FFPLAN_RICHFFPLAN_H
