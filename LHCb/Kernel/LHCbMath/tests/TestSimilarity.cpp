// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
// ============================================================================
// Gaudi and ROOT types
// ============================================================================
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "TRandom3.h"

// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Similarity.h"
#include "../src/instrset.h"

// ============================================================================
// Advance declaration for similarity methods
// ============================================================================
namespace LHCb {
namespace Math {
  namespace similarity_5_generic 
  {
    extern void similarity_5_5(const double* Ci, const double* Fi, double* ti);
  }
  namespace similarity_5_sse3
  {
    extern void similarity_5_5(const double* Ci, const double* Fi, double* ti);
  }
  namespace similarity_5_avx
  {
    extern void similarity_5_5(const double* Ci, const double* Fi, double* ti);
  }
}
}

// ============================================================================
/** @file
 *  @date 2010-02-09
 */
// ============================================================================
int compareInstructionSets(Gaudi::Matrix5x5 &F, Gaudi::SymMatrix5x5 &origin, 
                           int printResults = true) 
{
 
  // Checking which instruction sets we have available
  int level = instrset_detect();
  if (printResults)
    std::cout << "Instruction set level: " << level << std::endl;

  bool hasAVX = (level >= 7);
  bool hasSSE3 = (level >= 4);
  bool SSE3Diff = false;
  bool AVXDiff = false;
  double diffThreshold = 1e-15;
  Gaudi::SymMatrix5x5 target, targetSSE3, targetAVX;

  // Running the transform for the generic method
  LHCb::Math::similarity_5_generic::similarity_5_5( origin.Array(), F.Array(), target.Array() );
  if (printResults) 
  {  
    std::cout << "Generic similarity transform result" << std::endl;
    std::cout << target << std::endl;
  }
  
  if (hasSSE3)
  {    
    // Checking SSE3


    LHCb::Math::similarity_5_sse3::similarity_5_5( origin.Array(), F.Array(), targetSSE3.Array() );
    if (printResults) 
    {  
      std::cout << "SSE3 similarity transform result" << std::endl;    
      std::cout << targetSSE3 << std::endl;
    }
    
    Gaudi::SymMatrix5x5 cmpSSE3;
    for (int i=0; i<5; i++) 
      for (int j=i; j<5; j++) 
      { 
        cmpSSE3(i, j) = (targetSSE3(i,j) - target(i,j)) /target(i,j);
        if (TMath::Abs(cmpSSE3(i,j)) > diffThreshold)
        {
          SSE3Diff = true;
        }
      }
    if (printResults)
      std::cout << "SSE3 Differences" << std::endl << cmpSSE3 << std::endl;
  }
  

  // Checking AVX
  if (hasAVX) 
  {
    LHCb::Math::similarity_5_avx::similarity_5_5( origin.Array(), F.Array(), targetAVX.Array() );
    if (printResults) 
    {  
      std::cout << "AVX similarity transform result" << std::endl;      
      std::cout << targetAVX << std::endl;
    }
    
    Gaudi::SymMatrix5x5 cmpAVX;
    for (int i=0; i<5; i++) 
      for (int j=i; j<5; j++) 
      { 
        cmpAVX(i, j) = (targetAVX(i,j) - target(i,j)) /target(i,j);
        if (TMath::Abs(cmpAVX(i,j)) > diffThreshold) 
        {
          AVXDiff = true;
        }
      }
    if (printResults)
      std::cout << "AVX Differences" << std::endl << cmpAVX << std::endl;
  }

  // Checking if we found errors
  int retval = 0;
  if (SSE3Diff || AVXDiff) 
  {
    retval = 1;
    
  }
  
  return retval;
} 
// ============================================================================
// Main method                                                                      
// ============================================================================

int main()
{

  TRandom3 r(1);
  int retval = 0;
  
  for(int i=0; i<10000; i++) 
  {
    
    // First create the Matrices randomly
    Gaudi::Matrix5x5 F;
    Gaudi::SymMatrix5x5 origin;
    for (int i=0; i<5; i++) 
      for (int j=0; j<5; j++) 
      {
        F(i, j) = r.Rndm();
        if (i >= j) 
        {
          origin(i, j) = r.Rndm();
        }
      }
    
    retval += compareInstructionSets(F, origin, (i%5000) == 0);
  }
  
  return retval;
  
}
// ============================================================================
//                                                                      The END 
// ============================================================================
