// $Id$
// ============================================================================
// Include files 
// ============================================================================
// STD & STL 
// ============================================================================
#include <iostream>
#include <map>
#include <functional>
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
namespace avx {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}
namespace sse3 {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}
namespace generic {
    extern void similarity_5_1(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_5(const double* Ci, const double* Fi, double* Ti);
    extern void similarity_5_7(const double* Ci, const double* Fi, double* Ti);
}  
}
}


enum ISet : std::int8_t { GENERIC = 0, SSE3 = 3, AVX = 7 };
typedef std::function<void(const double* Ci, const double* Fi, double* Ti)> similarity_t;

std::map<ISet, similarity_t>  vtbl_5_1 = { { ISet::AVX,  LHCb::Math::avx::similarity_5_1 },
                                           { ISet::SSE3,  LHCb::Math::sse3::similarity_5_1 },
                                           { ISet::GENERIC, LHCb::Math::generic::similarity_5_1 } };
std::map<ISet, similarity_t>  vtbl_5_5 = { { ISet::AVX,  LHCb::Math::avx::similarity_5_5 },
                                           { ISet::SSE3,  LHCb::Math::sse3::similarity_5_5 },
                                           { ISet::GENERIC, LHCb::Math::generic::similarity_5_5 } };
std::map<ISet, similarity_t>  vtbl_5_7 = { { ISet::AVX,  LHCb::Math::avx::similarity_5_7 },
                                           { ISet::SSE3,  LHCb::Math::sse3::similarity_5_7 },
                                           { ISet::GENERIC, LHCb::Math::generic::similarity_5_7 } };


// ============================================================================
// Util methods
// ============================================================================
/**
 * Check whether the AVX instruction set is available
 */ 
bool hasInstructionSet(ISet lvl) 
{
  int level = instrset_detect();
  return (level >= static_cast<int>(lvl));
}

/**
 * Generate a random Matrix and a SymMatrix
 */
template <typename Mat>
void fillRandomSMatrix(Mat &F, TRandom& r, bool symmetric=false) 
{
  
  for (int i=0; i<F.kRows; i++) 
    for (int j=(symmetric==true?i:0); j<F.kCols; j++) 
    {
      F(i, j) = r.Rndm();
    }
}

/**
 * Compare SymMatrices 
 */
template <typename M>
std::pair<M, bool> compareSMatrix(M& A, M& B, bool symetric=true, 
                                  double diffThreshold=1e-15)
{
  M cmpres;
  bool hasDiff = false;
  for (int i=0; i< A.kCols; i++)
    for (int j=(symetric==true?i:0); j< A.kRows; j++) 
    { 
      cmpres(i, j) = (A(i,j) - B(i,j)) /B(i,j);
      if (TMath::Abs(cmpres(i,j)) > diffThreshold)
      {
        hasDiff = true;
      }
    }
  return std::make_pair(cmpres, hasDiff);
}

template <typename Mat, typename SymMat>
int compareInstructionSets(Mat &F, SymMat &origin, 
                           std::map<ISet, similarity_t>&  simFuncs,
                           int printResults = true) 
{
 
  
  bool hasAVX = hasInstructionSet(ISet::AVX);
  bool hasSSE3 = hasInstructionSet(ISet::SSE3);
  if (printResults)
    std::cout << "Has SSE3: " <<  hasSSE3 
              << " Has AVX: " << hasAVX << std::endl;
    

  bool SSE3Diff = false;
  bool AVXDiff = false;
  SymMat target, targetSSE3, targetAVX;

  // Running the transform for the generic method  
  (simFuncs[ISet::GENERIC])(origin.Array(), F.Array(), target.Array());
  if (printResults) 
  {  
    std::cout << "Generic similarity transform result" << std::endl;
    std::cout << target << std::endl;
  }

  if (hasSSE3)
  {    
    // Checking SSE3
    (simFuncs[ISet::SSE3])( origin.Array(), F.Array(), targetSSE3.Array() );
    if (printResults) 
    {  
      std::cout << "SSE3 similarity transform result" << std::endl;    
      std::cout << targetSSE3 << std::endl;
    }

    auto cmpSSE3Res = compareSMatrix(targetSSE3, target);
    auto cmpSSE3= cmpSSE3Res.first;
    SSE3Diff = cmpSSE3Res.second;
    
    if (printResults)
      std::cout << "SSE3 Differences" << std::endl << cmpSSE3 << std::endl;
  }

  // Checking AVX
  if (hasAVX) 
  {
    (simFuncs[ISet::AVX])( origin.Array(), F.Array(), targetAVX.Array() );
    if (printResults) 
    {  
      std::cout << "AVX similarity transform result" << std::endl;      
      std::cout << targetAVX << std::endl;
    }
    
    auto cmpAVXRes = compareSMatrix(targetAVX, target);
    auto cmpAVX= cmpAVXRes.first;
    AVXDiff = cmpAVXRes.second;

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
  int testcount=10000;


  std::cout << "============= Similarity_5_5 Test =============" << std::endl;
  for(int i=0; i<testcount; i++) 
  {
    Gaudi::Matrix5x5 F;
    Gaudi::SymMatrix5x5 origin;
    fillRandomSMatrix(F, r);
    fillRandomSMatrix(origin, r, true);
    retval += compareInstructionSets(F, origin, vtbl_5_5, (i%5000) == 0);
  }


  std::cout << "============= Similarity_5_7 Test =============" << std::endl;
  for(int i=0; i<testcount; i++) 
  {
    ROOT::Math::SMatrix<double,7,5> F;
    Gaudi::SymMatrix5x5 origin;
    fillRandomSMatrix(F, r);
    fillRandomSMatrix(origin, r, true);
    retval += compareInstructionSets(F, origin, vtbl_5_7, (i%5000) == 0);
  }

  std::cout << "============= Similarity_5_1 Test =============" << std::endl;
  for(int i=0; i<testcount; i++) 
  {
    Gaudi::Matrix1x5 F;
    Gaudi::SymMatrix5x5 origin;
    fillRandomSMatrix(F, r);
    fillRandomSMatrix(origin, r, true);
    retval += compareInstructionSets(F, origin, vtbl_5_1, (i%5000) == 0);
  }
 
  
  return retval;
  
}
// ============================================================================
//                                                                      The END 
// ============================================================================
