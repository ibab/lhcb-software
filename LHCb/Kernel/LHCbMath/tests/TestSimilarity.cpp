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
 * Orthogonalize a SMatrix
 */
template <typename Mat>
Mat orthogonalizeMatrix(Mat &M, bool &processOk)
{
    processOk = false;
    int n = M.kRows;
    Mat Q;
    Mat U;

    // Checking that we have a square matrix
    if (M.kRows != M.kCols)
    {
        processOk = false;
        return Q;
    }


    for (int i=0; i<n; i++) {

        // Initializing U with the original column from M
        for(int k=0; k<n; k++) {
            U(k,i) = M(k, i);
        }

        // Iterate of already established basis vectors to make sure we have an orthogonal vector
        for (int j=0; j<i; j++) {
            // Computing the scalar product
            double_t sp = 0.0;
            for(int k=0; k<n; k++) {
                sp += Q(k, j) * M(k, i);
            }

            // Now doing the actual projection
            for (int k = 0; k < n; k++) {
                U(k, i) += -sp * Q(k, j);
            }
        }

        double_t norm = 0.0;
        for (int k = 0; k < n; k++) {
            norm += U(k, i) * U(k, i);
        }
        norm = sqrt(norm);
        for (int k = 0; k < n; k++) {
            Q(k, i) = U(k, i) / norm;
        }
    }
    processOk = true;
    return Q;
}


/**
 * Generate a random SymMatrix with a given max Condition number
 */
template <typename Mat, typename SymMat>
void fillSMatrixSymWithCondNumber(SymMat &F, TRandom& r, double condNumber)
{

    Mat T;
    Mat Q;
    bool processOk = false;

    do
    {
        fillRandomSMatrix(T, r);
        Q = orthogonalizeMatrix(T, processOk);
    } while (!processOk);

    Mat tQ = ROOT::Math::Transpose(Q);

    Mat D;
    D(0,0) = 1;
    for (int i=1; i < 5; i++)
    {
        D(i, i) = r.Rndm() * condNumber;
    }

    Mat origin;
    origin = Q * D * tQ;

    for (int i = 0; i < F.kRows; i++)
    {
        for(int j=0; j<=i; j++)
        {
            F(i, j) = origin(i,j);
        }
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
            cmpres(i, j) = (A(i,j) - B(i,j));
            if (TMath::Abs(cmpres(i,j)) > diffThreshold)
            {
              hasDiff = true;
            }
        }

    return std::make_pair(cmpres, hasDiff);
}



template <typename Mat, typename SymMat>
int compareInstructionSets(Mat &F, SymMat &origin, double conditionNumber,
                           std::map<ISet, similarity_t>&  simFuncs,
                           int printResults = true) 
{
 
  // Setting the threshold for error
  double diffThreshold = 1e-15  * conditionNumber;
  // Checking the instruction sets available
  bool hasAVX = hasInstructionSet(ISet::AVX);
  bool hasSSE3 = hasInstructionSet(ISet::SSE3);
  if (printResults)
    std::cout << "Has SSE3: " <<  hasSSE3 
              << " Has AVX: " << hasAVX << std::endl;
    
  if (!hasSSE3) 
  {
    std::cerr << "WARNING: SSE3 Not available" << std::endl;
  }
  
  if (!hasAVX) 
  {
    std::cerr << "WARNING: AVX Not available" << std::endl;
  }

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

    auto cmpSSE3Res = compareSMatrix(targetSSE3, target, true, diffThreshold);
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
    
    auto cmpAVXRes = compareSMatrix(targetAVX, target, true, diffThreshold);
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
  
  std::vector<double> condNumbers = { 1, 1e6, 1e9 };
  

  std::cout << "============= Similarity_5_5 Test =============" << std::endl;
  for(int i=0; i<testcount; i++) 
  {
    for (auto condNumber: condNumbers) {   
      Gaudi::Matrix5x5 F;
      Gaudi::SymMatrix5x5 origin;
      fillRandomSMatrix(F, r);
      fillSMatrixSymWithCondNumber<Gaudi::Matrix5x5,Gaudi::SymMatrix5x5>(origin, r, condNumber);
      retval += compareInstructionSets(F, origin, condNumber, vtbl_5_5, (i%5000) == 0);
    } 
  }

  
  std::cout << "============= Similarity_5_7 Test =============" << std::endl;
  for(int i=0; i<testcount; i++) 
  {
    for (auto condNumber: condNumbers) {   
      ROOT::Math::SMatrix<double,7,5> F;
      Gaudi::SymMatrix5x5 origin;
      fillRandomSMatrix(F, r);
      fillSMatrixSymWithCondNumber<Gaudi::Matrix5x5,Gaudi::SymMatrix5x5>(origin, r, condNumber);
      retval += compareInstructionSets(F, origin, condNumber,  vtbl_5_7, (i%5000) == 0);
    }  
  }

   std::cout << "============= Similarity_5_1 Test =============" << std::endl;
   for(int i=0; i<testcount; i++) 
   {
     for (auto condNumber: condNumbers) {   
       Gaudi::Matrix1x5 F;
       Gaudi::SymMatrix5x5 origin;
       fillRandomSMatrix(F, r);
       fillSMatrixSymWithCondNumber<Gaudi::Matrix5x5,Gaudi::SymMatrix5x5>(origin, r, condNumber);
       retval += compareInstructionSets(F, origin, condNumber, vtbl_5_1, (i%5000) == 0);
     }    
   }

  return retval;
  
}
// ============================================================================
//                                                                      The END 
// ============================================================================
