#ifndef ALIGNSOLVTOOLS_PERSISTSOLV_H
#define ALIGNSOLVTOOLS_PERSISTSOLV_H

#include <math.h>

#include "SolvKernel/AlSymMat.h"
#include "SolvKernel/AlMat.h"
#include "SolvKernel/AlVec.h"

//Class containing methods to write, read, and combine algebra objects to/from binary files. Each object is filled from a job processed on certain subsample of data used for the alignment procedure

namespace Al {
  class Equations;
}

class PersistSolv {

 public:
  PersistSolv();
  ~PersistSolv();


  void WriteToBinary(AlSymMat& M,AlVec& r,std::string pathbin);
  void ReadFromBinary(AlSymMat& M,AlVec& r,std::string pathbin);
  void CombineBinary(AlSymMat& M,AlVec& r,std::string pathbin);

  void WriteToBinary(Al::Equations& eq,std::string pathbin);
  void ReadFromBinary(Al::Equations& eq,std::string pathbin);
  void CombineBinary(Al::Equations& eq,std::string pathbin);

 private:
 

};

#endif // ALIGNSOLVTOOLS_PERSISTSOLV_H
