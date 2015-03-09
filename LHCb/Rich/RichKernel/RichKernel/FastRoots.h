
#ifndef RICHKERNEL_FASTROOTS_H 
#define RICHKERNEL_FASTROOTS_H  1

namespace Rich
{ 
  namespace Maths
  {
    namespace FastRoots
    {
      
      //-----------------------------------------------------------------------
      // Cube Roots
      //-----------------------------------------------------------------------

      /** This is a novel and fast routine for the approximate cube root of
          an IEEE float (single precision). It is derived from a similar program
          for the approximate square root.
          The relative error ranges from 0 to +0.00103.
          Caution:
          Result for -0 is NaN.
          Result for 0 is 1.24e-13.
          For denorms it is either within tolerance or gives a result < 2.1e-13.
          Gives the correct result (inf) for x = inf.
          Gives the correct result (NaN) for x = NaN. 
      */
      inline float vfast_cbrt( const float x0 )
      {
        union {int ix; float x;};
        x = x0;                      // x can be viewed as int.
        ix = ix/4 + ix/16;           // Approximate divide by 3.
        ix = ix + ix/16;
        ix = ix + ix/256;
        ix = 0x2a5137a0 + ix;        // Initial guess.
        x = 0.33333333f*(2.0f*x + x0/(x*x));  // Newton step.
        return x;
      }

      /**  This is vfast_cbrt with an additional step of the Newton iteration, for
           increased accuracy.
           The relative error ranges from 0 to +0.00000116.
      */
      inline float fast_cbrt( const float x0 )
      {
        union {int ix; float x;};
        x  = x0;                     // x can be viewed as int.
        ix = ix/4 + ix/16;           // Approximate divide by 3.
        ix = ix + ix/16;
        ix = ix + ix/256;
        ix = 0x2a5137a0 + ix;        // Initial guess.
        x  = 0.33333333f*(2.0f*x + x0/(x*x));  // Newton step 1
        x  = 0.33333333f*(2.0f*x + x0/(x*x));  // Newton step 2
        x  = 0.33333333f*(2.0f*x + x0/(x*x));  // Newton step 3
        return x;
      }
      
      //-----------------------------------------------------------------------
      // Square roots
      //-----------------------------------------------------------------------
      
      /** This is a novel and fast routine for the approximate square root of
          an IEEE float (single precision). It is derived from a similar program
          for the approximate reciprocal square root.
          The relative error ranges from 0 to +0.0006011.
          Caution:
          Result for -0 is -1.35039e+19 (unreasonable).
          Result for 0 is 3.96845e-20.
          For denorms it is either within tolerance or gives a result < 1.0e-19.
          Gives the correct result (inf) for x = inf.
          Gives the correct result (NaN) for x = NaN. 
      */      
      float vfast_sqrt(const float x0) 
      {
        union {int ix; float x;};
        x  = x0;                      // x can be viewed as int.
        ix = 0x1fbb67a8 + (ix >> 1);  // Initial guess.
        x  = 0.5f*(x + x0/x);         // Newton step.
        return x;
      }

      /** This is vfast_sqrt with an additional step of the Newton iteration, for
          increased accuracy.
          The relative error ranges from 0 to +0.00000023.
      */
      template < class TYPE >
      TYPE fast_sqrt(const TYPE x0)
      {
        union {int ix; float x;};
        x  = x0;                      // x can be viewed as int.
        ix = 0x1fbb3f80 + (ix >> 1);  // Initial guess.
        x  = 0.5f*(x + x0/x);         // Newton step.
        x  = 0.5f*(x + x0/x);         // Newton step again.
        return x;
      }

    }
    
  }
}

#endif // RICHKERNEL_FASTROOTS_H 
