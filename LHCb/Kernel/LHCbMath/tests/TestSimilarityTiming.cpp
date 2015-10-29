// $Id$
// ============================================================================
// Include files
// ============================================================================
// STD & STL
// ============================================================================
#include <iostream>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <array>

using std::chrono::high_resolution_clock;
using std::chrono::microseconds;
using std::chrono::nanoseconds;
using std::chrono::time_point;

// ============================================================================
// Gaudi and ROOT types
// ============================================================================
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "Math/SMatrix.h"
#include "TRandom3.h"
// ============================================================================
// LHCbMath
// ============================================================================
#include "LHCbMath/Similarity.h"
#include "../src/instrset.h"


#include <TH1D.h>

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


enum ISet : std::int8_t { CLASSIC = -1, GENERIC = 0, SSE3 = 3, AVX = 7 };
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

// Util class for test results
struct TestResults {
    // Max Condition number used for the test
    double_t conditionNumber;

    // Map with timing results
    std::map<ISet, std::chrono::nanoseconds::rep> timing;

    // Map with max differences
    std::map<ISet, double>  maxDiff;
};

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
 * Orthogonalze a SMatrix
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
void fillSMatrixSymWithCondNumber(Gaudi::SymMatrix5x5 &F, TRandom& r, double condNumber)
{

    Gaudi::Matrix5x5 T;
    Gaudi::Matrix5x5 Q;
    bool processOk = false;

    do
    {
        fillRandomSMatrix(T, r);
        Q = orthogonalizeMatrix(T, processOk);
    } while (!processOk);

    Gaudi::Matrix5x5 tQ = ROOT::Math::Transpose(Q);

    Gaudi::Matrix5x5 D;
    D(0,0) = 1;
    for (int i=1; i < 5; i++)
    {
        D(i, i) = r.Rndm() * condNumber;
    }

    Gaudi::Matrix5x5 origin;
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
                                  double diffThreshold=1e-5)
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


/**
 * Compare Matrices
 */
template <typename M1, typename M2>
double matrixMaxDiff(M1& A, M2& B)
{
    double maxDiff= 0;
    for (int i=0; i< A.kCols; i++)
        for (int j=0; j< A.kRows; j++)
        {
            double tmp = TMath::Abs(A(i,j) - B(i,j));
            //std::cout << "XXXX"  << tmp << " " << TMath::Abs(A(i,j) - B(i,j)) << std::endl;
            if (tmp > maxDiff)
                maxDiff = tmp;
        }
    return maxDiff;
}




template <typename Mat, typename SymMat>
TestResults compareInstructionSets(Mat &Ftype, SymMat &Otype,
                                   std::map<ISet, similarity_t>&  simFuncs,
                                   TRandom &r,
                                   const int nbentries,
                                   const double maxConditionNumber)
{

    bool hasAVX = hasInstructionSet(ISet::AVX);
    bool hasSSE3 = hasInstructionSet(ISet::SSE3);
    // Ugly, need to refactor the functions
    (void)Ftype; (void)Otype;

    // Declaring our arrays
    auto F = new Mat[nbentries];
    auto O = new SymMat[nbentries];
    auto resGeneric = new SymMat[nbentries];
    auto resClassic = new Mat[nbentries];
    auto resSSE3 = new SymMat[nbentries];
    auto resAVX = new SymMat[nbentries];

    // Just a timing helper
    auto getTime = [] (time_point<high_resolution_clock> t0) {
        auto diff = high_resolution_clock::now() - t0;
        auto total_ms = std::chrono::duration_cast<nanoseconds>(diff);
        return (total_ms.count());
    };

    TestResults results;
    results.conditionNumber = maxConditionNumber;

    // Now filling up the input data
    for(int i=0; i<nbentries; i++) {
        //        for (int k=0; k<F[i].kCols; k++) {
        //            F[i](k,k) = 2;
        //            O[i](k, k) = 3;
        //        }
        fillRandomSMatrix(F[i], r);
        fillSMatrixSymWithCondNumber(O[i], r, maxConditionNumber);
        //std::cout << "############################" << std::endl
        //            << F[i] << std::endl
        //            << "#" << std::endl
        //            << O[i] << std::endl
        //            << "############################" << std::endl;
    }

    // Now running the various methods
    {
        auto t0 = high_resolution_clock::now();
        for(int i=0; i<nbentries; i++) {
            (simFuncs[ISet::GENERIC])(O[i].Array(),F[i].Array(), resGeneric[i].Array());
        }
        results.timing[ISet::GENERIC] =  getTime(t0);
    }

    // Checking SSE3
    if (hasSSE3)
    {
        auto t0 = high_resolution_clock::now();
        for(int i=0; i<nbentries; i++) {
            (simFuncs[ISet::SSE3])(O[i].Array(), F[i].Array(), resSSE3[i].Array());
        }
        results.timing[ISet::SSE3] =  getTime(t0);
    }

    // Checking AVX
    if (hasAVX)
    {
        auto t0 = high_resolution_clock::now();
        for(int i=0; i<nbentries; i++) {
            (simFuncs[ISet::AVX])(O[i].Array(), F[i].Array(), resAVX[i].Array());
        }
        results.timing[ISet::AVX] =  getTime(t0);
    }

    // Checking the classic SMatrix method
    {
        auto t0 = high_resolution_clock::now();
        Mat FC;
        for(int i=0; i<nbentries; i++) {
            FC = F[i]*O[i];
            resClassic[i] = FC * ROOT::Math::Transpose(F[i]) ;
        }
        results.timing[ISet::CLASSIC] =  getTime(t0);
    }

    //std::cout << "Time in ns for " << nbentries << " matrices" << std::endl;
    //std::cout << "Classic timing: " << results.timing[ISet::CLASSIC]  << std::endl;
    //std::cout << "Generic timing: " << results.timing[ISet::GENERIC]  << std::endl;
    //std::cout << "SSE3    timing: " << results.timing[ISet::SSE3]  << std::endl;
    //std::cout << "AVX     timing: " << results.timing[ISet::AVX]  << std::endl;


    // Now checking the results
    {

        results.maxDiff[AVX] = 0;
        results.maxDiff[SSE3] = 0;
        results.maxDiff[GENERIC] = 0;

        for(int i=0; i<nbentries; i++) {
            {
                double tmpres = matrixMaxDiff(resClassic[i], resGeneric[i]);
                if (tmpres > results.maxDiff[GENERIC])
                    results.maxDiff[GENERIC] = tmpres;
            }
            {
                double tmpres = matrixMaxDiff(resClassic[i], resSSE3[i]);
                if (tmpres > results.maxDiff[SSE3])
                    results.maxDiff[SSE3] = tmpres; }

            {
                double tmpres = matrixMaxDiff(resClassic[i], resAVX[i]);
                if (tmpres > results.maxDiff[AVX])
                    results.maxDiff[AVX] = tmpres;
            }
        }

        //        std::cout << resClassic[0] << std::endl;
        //        std::cout << resGeneric[0] << std::endl;
        //        std::cout << resSSE3[0] << std::endl;
        //        std::cout << resAVX[0] << std::endl;
        //std::cout << "Generic maxdiff: " << results.maxDiff[ISet::GENERIC]  << std::endl;
        //std::cout << "SSE3    maxdiff: " << results.maxDiff[ISet::SSE3]  << std::endl;
        //std::cout << "AVX     maxdiff: " << results.maxDiff[ISet::AVX]  << std::endl;
    }

    delete[] F;
    delete[] O;
    delete[] resGeneric;
    delete[] resClassic;
    delete[] resSSE3;
    delete[] resAVX;

    return results;
}
// ============================================================================
// Main method
// ============================================================================

int main()
{

    TRandom3 r(1567657);
    int retval = 0;
    const int testsize=100000;
    const int testcount=100;
    const int maxConditionNumber = 1e6;

    // Check with varying condition numbers
    const double cond_min = 1;
    const double cond_max = 1e15;
    const int nbres = (int)(log10(cond_max) -log(cond_min) + 1);
    std::vector<TestResults> results;
    results.reserve(nbres);

    Gaudi::Matrix5x5 F;
    Gaudi::SymMatrix5x5 origin;

    for(double cond = cond_min; cond <= cond_max; cond *= 10) {
      results[(int)(log10(cond))] = compareInstructionSets(F, origin, vtbl_5_5, r, testsize, cond);
    }

    std::cout << std::endl << "Checking with different condition numbers" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "ConditionNumber\tGeneric\tSSE3\tAVX" << std::endl;
    for (int i=0; i<nbres; i++) {
        std::cout << results[i].conditionNumber
                  << "\t" << results[i].maxDiff[ISet::GENERIC]
                  << "\t" << results[i].maxDiff[ISet::SSE3]
                  << "\t" << results[i].maxDiff[ISet::AVX]
                  << std::endl;
    }


    // Timing check
    TestResults tresults[testcount];
    for(int i=0; i < testcount; i++) {
        //std::cout << "Test " << i << std::endl;
      tresults[i] = compareInstructionSets(F, origin, vtbl_5_5, r, testsize, maxConditionNumber);
    }


    std::cout << std::endl << "Checking timing" << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << "Classic\tGeneric\tSSE3\tAVX" << std::endl;
    for (int i=0; i<testcount; i++) {

        std::cout << tresults[i].timing[ISet::CLASSIC]
                  << "\t" << tresults[i].timing[ISet::GENERIC]
                  << "\t" << tresults[i].timing[ISet::SSE3]
                  << "\t" << tresults[i].timing[ISet::AVX]
                  << std::endl;
    }

    return retval;
}
// ============================================================================
//                                                                      The END
// ============================================================================
