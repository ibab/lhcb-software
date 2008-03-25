//PersistSolv.cpp, A.Hicheur, EPFL, Adlene.Hicheur@epfl.ch
//include files
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>

#include "AlignSolvTools/PersistSolv.h"
#include "AlignSolvTools/AlEquations.h"
#include "AlignSolvTools/AlFileIO.h"

typedef std::vector<Al::ElementData> ElementContainer;

PersistSolv::PersistSolv() {}
PersistSolv::~PersistSolv() {}

using namespace Al::FileIO ;

std::ofstream& operator<<(std::ofstream& file, const Al::ElementData& data)
{
  file  << data.m_v
	<< data.m_m 
	<< data.m_numHits
	<< data.m_weight ;
  return file ;
}
  
inline
std::ifstream& operator>>(std::ifstream& file, Al::ElementData& data)
{
  file  >> data.m_v
	>> data.m_m 
	>> data.m_numHits
	>> data.m_weight ;
  return file ;
}

void PersistSolv::WriteToBinary(AlSymMat& M,AlVec& r,std::string pathbin) {

std::string path = pathbin+"bigmatrix.bin";

std::fstream matrixbin(path.c_str(), std::ios::out | std::ios::binary);


    int msiz;
    double melem;

  
    msiz = M.size();

    matrixbin.write ((char*)&msiz, sizeof (msiz));

    //    std::cout << "Matrix size  :   " << msiz << std::endl;
    for( int i=0; i<msiz; i++) {
         for( int j=0; j<=i; j++) {
           melem = M[i][j];
	   //           melem = melem/m_scale;
           matrixbin.write((char*)&(melem), sizeof (melem)); 
         }
    }

    matrixbin.close();

    path = pathbin+"bigvector.bin";

// write big vector

    std::fstream vectorbin(path.c_str(), std::ios::out | std::ios::binary);

    int vsiz; 
    //   int    ielem = 0;
    double velem;
    //  int scale=int(m_scale);
    vsiz = r.size(); 
    vectorbin.write ((char*)&vsiz, sizeof (vsiz));

    //    vectorbin.write ((char*)&scale, sizeof (scale));

    for( int i=0; i<vsiz; i++) {



 
      velem = r[i];
      //  velem = velem/m_scale;
      vectorbin.write((char*)&(velem), sizeof (velem)); 
    }

    vectorbin.close();

}


void PersistSolv::ReadFromBinary(AlSymMat& M,AlVec& r,std::string pathbin) {

  std::string path = pathbin+"bigmatrix.bin";

  std::fstream matrixbin(path.c_str(), std::ios::in | std::ios::binary);

  int msiz;
  double  melem;

    matrixbin.read ((char*)&msiz, sizeof (msiz));

 
    if (M.size()==0) M.reSize(msiz);
    if (r.size()==0) r.reSize(msiz);

 
  
      for( int i=0; i<msiz; i++) {
           for( int j=0; j<=i; j++) {
             matrixbin.read((char*)&melem, sizeof (melem));
             M[i][j] = melem; 
           }
      }

 
    matrixbin.close();

    path = pathbin+"bigvector.bin";

    std::fstream vectorbin(path.c_str(), std::ios::in | std::ios::binary);

    int vsiz;
    //   int     ielem;
    double  velem;
    vectorbin.read ((char*)&vsiz, sizeof (vsiz));
    //   vectorbin.read ((char*)&scale, sizeof (scale));
    //   m_scale = scale;

    for( int i=0; i<vsiz; i++) {
      //   vectorbin.read((char*)&ielem, sizeof (ielem));    
       vectorbin.read((char*)&velem, sizeof (velem));
       r[i] = velem;
    }

    vectorbin.close();

 
    }


void PersistSolv::CombineBinary(AlSymMat& M,AlVec& r,std::string pathbin) {

  std::string path = pathbin+"bigmatrix.bin";

  std::fstream matrixbin(path.c_str(), std::ios::in | std::ios::binary);

  unsigned int msiz;
  double  melem;

    matrixbin.read ((char*)&msiz, sizeof (msiz));

    if (M.size()==0) M.reSize(msiz);
    if (r.size()==0) r.reSize(msiz);

    if(M.size()!=0 && msiz!=M.size()) std::cerr << "Can't combine matrices, sizes don't match" << std::endl;
  
      for( unsigned int i=0; i<msiz; i++) {
           for( unsigned int j=0; j<=i; j++) {
             matrixbin.read((char*)&melem, sizeof (melem));
             M[i][j] += melem; 
           }
      }

 
    matrixbin.close();

    path = pathbin+"bigvector.bin";

    std::fstream vectorbin(path.c_str(), std::ios::in | std::ios::binary);

    int vsiz;
    //   int     ielem;
    double  velem;
    vectorbin.read ((char*)&vsiz, sizeof (vsiz));
    //   vectorbin.read ((char*)&scale, sizeof (scale));
    //   m_scale = scale;

    for( int i=0; i<vsiz; i++) {
      //   vectorbin.read((char*)&ielem, sizeof (ielem));    
       vectorbin.read((char*)&velem, sizeof (velem));
       r[i] += velem;
    }

    vectorbin.close();


    }


//Second version with AlEquations objects:

void PersistSolv::WriteToBinary(Al::Equations& eq,std::string pathbin) {

std::string path = pathbin+"alignobject.bin";

std::ofstream file(path.c_str(),std::ios::out | std::ios::binary);
    file << eq.m_elements
	 << eq.m_numTracks
	 << eq.m_totalChiSquare
	 << eq.m_totalNumDofs
	 << eq.m_numExternalHits ;
    file.close() ;


}


void PersistSolv::ReadFromBinary(Al::Equations& eq,std::string pathbin) {

std::string path = pathbin+"alignobject.bin";

std::ifstream file(path.c_str(),std::ios::in | std::ios::binary);
    file >> eq.m_elements
	 >> eq.m_numTracks
	 >> eq.m_totalChiSquare
	 >> eq.m_totalNumDofs
	 >> eq.m_numExternalHits ;
    file.close() ;


}

void PersistSolv::CombineBinary(Al::Equations& eq,std::string pathbin) {


    ElementContainer rhs_elements ;
    size_t           rhs_numTracks ;
    double           rhs_totalChiSquare ;
    size_t           rhs_totalNumDofs ;
    size_t           rhs_numExternalHits ;

std::string path = pathbin+"alignobject.bin";

std::ifstream file(path.c_str(),std::ios::in | std::ios::binary);
    file >> rhs_elements
	 >> rhs_numTracks
	 >> rhs_totalChiSquare
	 >> rhs_totalNumDofs
	 >> rhs_numExternalHits ;
    file.close() ;

    assert( eq.m_elements.size() == rhs_elements.size() ) ;
    ElementContainer::iterator it = eq.m_elements.begin() ;
    for( ElementContainer::const_iterator rhsit = rhs_elements.begin() ;
	 rhsit != rhs_elements.end(); ++rhsit, ++it) (*it).add( *rhsit ) ;
    // eq.m_elements        += rhs_elements;
    eq.m_numTracks       += rhs_numTracks ;
    eq.m_totalChiSquare  += rhs_totalChiSquare ;
    eq.m_totalNumDofs    += rhs_totalNumDofs ;
    eq.m_numExternalHits += rhs_numExternalHits ;
}
