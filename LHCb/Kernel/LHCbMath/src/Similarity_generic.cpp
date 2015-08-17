#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb {
namespace Math {
namespace generic {

void  similarity_5_1(const double* Ci, const double* Fi, double* ti)  {
      auto _0 = Ci[ 0]*Fi[0]+Ci[ 1]*Fi[1]+Ci[ 3]*Fi[2]+Ci[ 6]*Fi[3]+Ci[10]*Fi[4];
      auto _1 = Ci[ 1]*Fi[0]+Ci[ 2]*Fi[1]+Ci[ 4]*Fi[2]+Ci[ 7]*Fi[3]+Ci[11]*Fi[4];
      auto _2 = Ci[ 3]*Fi[0]+Ci[ 4]*Fi[1]+Ci[ 5]*Fi[2]+Ci[ 8]*Fi[3]+Ci[12]*Fi[4];
      auto _3 = Ci[ 6]*Fi[0]+Ci[ 7]*Fi[1]+Ci[ 8]*Fi[2]+Ci[ 9]*Fi[3]+Ci[13]*Fi[4];
      auto _4 = Ci[10]*Fi[0]+Ci[11]*Fi[1]+Ci[12]*Fi[2]+Ci[13]*Fi[3]+Ci[14]*Fi[4];
      *ti = Fi[ 0]*_0 + Fi[ 1]*_1 + Fi[ 2]*_2 + Fi[ 3]*_3 + Fi[ 4]*_4;
}

void similarity_5_5(const double* Ci, const double* Fi, double* ti)  {
      if (Ci == ti ) throw "target and source overlap -- do not do that";
      // std::cout << "using similarity_5_5_generic" << std::endl;
      auto _0 = Ci[ 0]*Fi[0]+Ci[ 1]*Fi[1]+Ci[ 3]*Fi[2]+Ci[ 6]*Fi[3]+Ci[10]*Fi[4];
      auto _1 = Ci[ 1]*Fi[0]+Ci[ 2]*Fi[1]+Ci[ 4]*Fi[2]+Ci[ 7]*Fi[3]+Ci[11]*Fi[4];
      auto _2 = Ci[ 3]*Fi[0]+Ci[ 4]*Fi[1]+Ci[ 5]*Fi[2]+Ci[ 8]*Fi[3]+Ci[12]*Fi[4];
      auto _3 = Ci[ 6]*Fi[0]+Ci[ 7]*Fi[1]+Ci[ 8]*Fi[2]+Ci[ 9]*Fi[3]+Ci[13]*Fi[4];
      auto _4 = Ci[10]*Fi[0]+Ci[11]*Fi[1]+Ci[12]*Fi[2]+Ci[13]*Fi[3]+Ci[14]*Fi[4];
      ti[ 0] = Fi[ 0]*_0 + Fi[ 1]*_1 + Fi[ 2]*_2 + Fi[ 3]*_3 + Fi[ 4]*_4;
      ti[ 1] = Fi[ 5]*_0 + Fi[ 6]*_1 + Fi[ 7]*_2 + Fi[ 8]*_3 + Fi[ 9]*_4;
      ti[ 3] = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[ 6] = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[10] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4; 
      _0 =  Ci[ 0]*Fi[5]+Ci[ 1]*Fi[6]+Ci[ 3]*Fi[7]+Ci[ 6]*Fi[8]+Ci[10]*Fi[9];
      _1 =  Ci[ 1]*Fi[5]+Ci[ 2]*Fi[6]+Ci[ 4]*Fi[7]+Ci[ 7]*Fi[8]+Ci[11]*Fi[9];
      _2 =  Ci[ 3]*Fi[5]+Ci[ 4]*Fi[6]+Ci[ 5]*Fi[7]+Ci[ 8]*Fi[8]+Ci[12]*Fi[9];
      _3 =  Ci[ 6]*Fi[5]+Ci[ 7]*Fi[6]+Ci[ 8]*Fi[7]+Ci[ 9]*Fi[8]+Ci[13]*Fi[9];
      _4 =  Ci[10]*Fi[5]+Ci[11]*Fi[6]+Ci[12]*Fi[7]+Ci[13]*Fi[8]+Ci[14]*Fi[9];
      ti[2]  = Fi[ 5]*_0 + Fi[ 6]*_1 + Fi[ 7]*_2 + Fi[ 8]*_3 + Fi[ 9]*_4;
      ti[4]  = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[7]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[11] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      _0 = Ci[ 0]*Fi[10]+Ci[ 1]*Fi[11]+Ci[ 3]*Fi[12]+Ci[ 6]*Fi[13]+Ci[10]*Fi[14];
      _1 = Ci[ 1]*Fi[10]+Ci[ 2]*Fi[11]+Ci[ 4]*Fi[12]+Ci[ 7]*Fi[13]+Ci[11]*Fi[14];
      _2 = Ci[ 3]*Fi[10]+Ci[ 4]*Fi[11]+Ci[ 5]*Fi[12]+Ci[ 8]*Fi[13]+Ci[12]*Fi[14];
      _3 = Ci[ 6]*Fi[10]+Ci[ 7]*Fi[11]+Ci[ 8]*Fi[12]+Ci[ 9]*Fi[13]+Ci[13]*Fi[14];
      _4 = Ci[10]*Fi[10]+Ci[11]*Fi[11]+Ci[12]*Fi[12]+Ci[13]*Fi[13]+Ci[14]*Fi[14];
      ti[5]  = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[8]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[12] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      _0 = Ci[ 0]*Fi[15]+Ci[ 1]*Fi[16]+Ci[ 3]*Fi[17]+Ci[ 6]*Fi[18]+Ci[10]*Fi[19];
      _1 = Ci[ 1]*Fi[15]+Ci[ 2]*Fi[16]+Ci[ 4]*Fi[17]+Ci[ 7]*Fi[18]+Ci[11]*Fi[19];
      _2 = Ci[ 3]*Fi[15]+Ci[ 4]*Fi[16]+Ci[ 5]*Fi[17]+Ci[ 8]*Fi[18]+Ci[12]*Fi[19];
      _3 = Ci[ 6]*Fi[15]+Ci[ 7]*Fi[16]+Ci[ 8]*Fi[17]+Ci[ 9]*Fi[18]+Ci[13]*Fi[19];
      _4 = Ci[10]*Fi[15]+Ci[11]*Fi[16]+Ci[12]*Fi[17]+Ci[13]*Fi[18]+Ci[14]*Fi[19];
      ti[9]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[13] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      _0 = Ci[ 0]*Fi[20]+Ci[1]*Fi[21]+Ci[3]*Fi[22]+Ci[6]*Fi[23]+Ci[10]*Fi[24];
      _1 = Ci[ 1]*Fi[20]+Ci[2]*Fi[21]+Ci[4]*Fi[22]+Ci[7]*Fi[23]+Ci[11]*Fi[24];
      _2 = Ci[ 3]*Fi[20]+Ci[4]*Fi[21]+Ci[5]*Fi[22]+Ci[8]*Fi[23]+Ci[12]*Fi[24];
      _3 = Ci[ 6]*Fi[20]+Ci[7]*Fi[21]+Ci[8]*Fi[22]+Ci[9]*Fi[23]+Ci[13]*Fi[24];
      _4 = Ci[10]*Fi[20]+Ci[11]*Fi[21]+Ci[12]*Fi[22]+Ci[13]*Fi[23]+Ci[14]*Fi[24];
      ti[14]= Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
}

void similarity_5_7(const double* Ci, const double* Fi, double* ti)  {
      if (Ci == ti ) throw "target and source overlap -- do not do that";
      // std::cout << "using similarity_5_7_generic" << std::endl;
      auto _0 = Ci[ 0]*Fi[0]+Ci[ 1]*Fi[1]+Ci[ 3]*Fi[2]+Ci[ 6]*Fi[3]+Ci[10]*Fi[4];
      auto _1 = Ci[ 1]*Fi[0]+Ci[ 2]*Fi[1]+Ci[ 4]*Fi[2]+Ci[ 7]*Fi[3]+Ci[11]*Fi[4];
      auto _2 = Ci[ 3]*Fi[0]+Ci[ 4]*Fi[1]+Ci[ 5]*Fi[2]+Ci[ 8]*Fi[3]+Ci[12]*Fi[4];
      auto _3 = Ci[ 6]*Fi[0]+Ci[ 7]*Fi[1]+Ci[ 8]*Fi[2]+Ci[ 9]*Fi[3]+Ci[13]*Fi[4];
      auto _4 = Ci[10]*Fi[0]+Ci[11]*Fi[1]+Ci[12]*Fi[2]+Ci[13]*Fi[3]+Ci[14]*Fi[4];
      ti[ 0] = Fi[ 0]*_0 + Fi[ 1]*_1 + Fi[ 2]*_2 + Fi[ 3]*_3 + Fi[ 4]*_4;
      ti[ 1] = Fi[ 5]*_0 + Fi[ 6]*_1 + Fi[ 7]*_2 + Fi[ 8]*_3 + Fi[ 9]*_4;
      ti[ 3] = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[ 6] = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[10] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4; 
      ti[15] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4; 
      ti[21] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4; 
      _0 =  Ci[ 0]*Fi[5]+Ci[ 1]*Fi[6]+Ci[ 3]*Fi[7]+Ci[ 6]*Fi[8]+Ci[10]*Fi[9];
      _1 =  Ci[ 1]*Fi[5]+Ci[ 2]*Fi[6]+Ci[ 4]*Fi[7]+Ci[ 7]*Fi[8]+Ci[11]*Fi[9];
      _2 =  Ci[ 3]*Fi[5]+Ci[ 4]*Fi[6]+Ci[ 5]*Fi[7]+Ci[ 8]*Fi[8]+Ci[12]*Fi[9];
      _3 =  Ci[ 6]*Fi[5]+Ci[ 7]*Fi[6]+Ci[ 8]*Fi[7]+Ci[ 9]*Fi[8]+Ci[13]*Fi[9];
      _4 =  Ci[10]*Fi[5]+Ci[11]*Fi[6]+Ci[12]*Fi[7]+Ci[13]*Fi[8]+Ci[14]*Fi[9];
      ti[2]  = Fi[ 5]*_0 + Fi[ 6]*_1 + Fi[ 7]*_2 + Fi[ 8]*_3 + Fi[ 9]*_4;
      ti[4]  = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[7]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[11] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      ti[16] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4;
      ti[22] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
      _0 = Ci[ 0]*Fi[10]+Ci[ 1]*Fi[11]+Ci[ 3]*Fi[12]+Ci[ 6]*Fi[13]+Ci[10]*Fi[14];
      _1 = Ci[ 1]*Fi[10]+Ci[ 2]*Fi[11]+Ci[ 4]*Fi[12]+Ci[ 7]*Fi[13]+Ci[11]*Fi[14];
      _2 = Ci[ 3]*Fi[10]+Ci[ 4]*Fi[11]+Ci[ 5]*Fi[12]+Ci[ 8]*Fi[13]+Ci[12]*Fi[14];
      _3 = Ci[ 6]*Fi[10]+Ci[ 7]*Fi[11]+Ci[ 8]*Fi[12]+Ci[ 9]*Fi[13]+Ci[13]*Fi[14];
      _4 = Ci[10]*Fi[10]+Ci[11]*Fi[11]+Ci[12]*Fi[12]+Ci[13]*Fi[13]+Ci[14]*Fi[14];
      ti[5]  = Fi[10]*_0 + Fi[11]*_1 + Fi[12]*_2 + Fi[13]*_3 + Fi[14]*_4;
      ti[8]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[12] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      ti[17] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4;
      ti[23] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
      _0 = Ci[ 0]*Fi[15]+Ci[ 1]*Fi[16]+Ci[ 3]*Fi[17]+Ci[ 6]*Fi[18]+Ci[10]*Fi[19];
      _1 = Ci[ 1]*Fi[15]+Ci[ 2]*Fi[16]+Ci[ 4]*Fi[17]+Ci[ 7]*Fi[18]+Ci[11]*Fi[19];
      _2 = Ci[ 3]*Fi[15]+Ci[ 4]*Fi[16]+Ci[ 5]*Fi[17]+Ci[ 8]*Fi[18]+Ci[12]*Fi[19];
      _3 = Ci[ 6]*Fi[15]+Ci[ 7]*Fi[16]+Ci[ 8]*Fi[17]+Ci[ 9]*Fi[18]+Ci[13]*Fi[19];
      _4 = Ci[10]*Fi[15]+Ci[11]*Fi[16]+Ci[12]*Fi[17]+Ci[13]*Fi[18]+Ci[14]*Fi[19];
      ti[9]  = Fi[15]*_0 + Fi[16]*_1 + Fi[17]*_2 + Fi[18]*_3 + Fi[19]*_4;
      ti[13] = Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      ti[18] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4;
      ti[24] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
      _0 = Ci[ 0]*Fi[20]+Ci[1]*Fi[21]+Ci[3]*Fi[22]+Ci[6]*Fi[23]+Ci[10]*Fi[24];
      _1 = Ci[ 1]*Fi[20]+Ci[2]*Fi[21]+Ci[4]*Fi[22]+Ci[7]*Fi[23]+Ci[11]*Fi[24];
      _2 = Ci[ 3]*Fi[20]+Ci[4]*Fi[21]+Ci[5]*Fi[22]+Ci[8]*Fi[23]+Ci[12]*Fi[24];
      _3 = Ci[ 6]*Fi[20]+Ci[7]*Fi[21]+Ci[8]*Fi[22]+Ci[9]*Fi[23]+Ci[13]*Fi[24];
      _4 = Ci[10]*Fi[20]+Ci[11]*Fi[21]+Ci[12]*Fi[22]+Ci[13]*Fi[23]+Ci[14]*Fi[24];
      ti[14]= Fi[20]*_0 + Fi[21]*_1 + Fi[22]*_2 + Fi[23]*_3 + Fi[24]*_4;
      ti[19] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4;
      ti[25] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
      _0 = Ci[ 0]*Fi[25]+Ci[1]*Fi[26]+Ci[3]*Fi[27]+Ci[6]*Fi[28]+Ci[10]*Fi[29];
      _1 = Ci[ 1]*Fi[25]+Ci[2]*Fi[26]+Ci[4]*Fi[27]+Ci[7]*Fi[28]+Ci[11]*Fi[29];
      _2 = Ci[ 3]*Fi[25]+Ci[4]*Fi[26]+Ci[5]*Fi[27]+Ci[8]*Fi[28]+Ci[12]*Fi[29];
      _3 = Ci[ 6]*Fi[25]+Ci[7]*Fi[26]+Ci[8]*Fi[27]+Ci[9]*Fi[28]+Ci[13]*Fi[29];
      _4 = Ci[10]*Fi[25]+Ci[11]*Fi[26]+Ci[12]*Fi[27]+Ci[13]*Fi[28]+Ci[14]*Fi[29];
      ti[20] = Fi[25]*_0 + Fi[26]*_1 + Fi[27]*_2 + Fi[28]*_3 + Fi[29]*_4;
      ti[26] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
      _0 = Ci[ 0]*Fi[30]+Ci[1]*Fi[31]+Ci[3]*Fi[32]+Ci[6]*Fi[33]+Ci[10]*Fi[34];
      _1 = Ci[ 1]*Fi[30]+Ci[2]*Fi[31]+Ci[4]*Fi[32]+Ci[7]*Fi[33]+Ci[11]*Fi[34];
      _2 = Ci[ 3]*Fi[30]+Ci[4]*Fi[31]+Ci[5]*Fi[32]+Ci[8]*Fi[33]+Ci[12]*Fi[34];
      _3 = Ci[ 6]*Fi[30]+Ci[7]*Fi[31]+Ci[8]*Fi[32]+Ci[9]*Fi[33]+Ci[13]*Fi[34];
      _4 = Ci[10]*Fi[30]+Ci[11]*Fi[31]+Ci[12]*Fi[32]+Ci[13]*Fi[33]+Ci[14]*Fi[34];
      ti[27] = Fi[30]*_0 + Fi[31]*_1 + Fi[32]*_2 + Fi[33]*_3 + Fi[34]*_4;
}


bool average( const double* X1, const double* C1,
              const double* X2, const double* C2,
              double* X, double* C ) {
      // compute the inverse of the covariance (i.e. weight) of the difference: R=(C1+C2)
      Gaudi::SymMatrix5x5 invRM;
      auto invR = invRM.Array();
      for (int i=0;i<15;++i) invR[i]=C1[i]+C2[i];
      bool success = invRM.InvertChol() ;
      // compute the gain matrix

      // K <- C1*inverse(C1+C2) = C1*invR
      double K[25];
      K[ 0] = C1[ 0]*invR[ 0] + C1[ 1]*invR[ 1] + C1[ 3]*invR[ 3] + C1[ 6]*invR[ 6] + C1[10]*invR[10];
      K[ 1] = C1[ 0]*invR[ 1] + C1[ 1]*invR[ 2] + C1[ 3]*invR[ 4] + C1[ 6]*invR[ 7] + C1[10]*invR[11];
      K[ 2] = C1[ 0]*invR[ 3] + C1[ 1]*invR[ 4] + C1[ 3]*invR[ 5] + C1[ 6]*invR[ 8] + C1[10]*invR[12];
      K[ 3] = C1[ 0]*invR[ 6] + C1[ 1]*invR[ 7] + C1[ 3]*invR[ 8] + C1[ 6]*invR[ 9] + C1[10]*invR[13];
      K[ 4] = C1[ 0]*invR[10] + C1[ 1]*invR[11] + C1[ 3]*invR[12] + C1[ 6]*invR[13] + C1[10]*invR[14];

      K[ 5] = C1[ 1]*invR[ 0] + C1[ 2]*invR[ 1] + C1[ 4]*invR[ 3] + C1[ 7]*invR[ 6] + C1[11]*invR[10];
      K[ 6] = C1[ 1]*invR[ 1] + C1[ 2]*invR[ 2] + C1[ 4]*invR[ 4] + C1[ 7]*invR[ 7] + C1[11]*invR[11];
      K[ 7] = C1[ 1]*invR[ 3] + C1[ 2]*invR[ 4] + C1[ 4]*invR[ 5] + C1[ 7]*invR[ 8] + C1[11]*invR[12];
      K[ 8] = C1[ 1]*invR[ 6] + C1[ 2]*invR[ 7] + C1[ 4]*invR[ 8] + C1[ 7]*invR[ 9] + C1[11]*invR[13];
      K[ 9] = C1[ 1]*invR[10] + C1[ 2]*invR[11] + C1[ 4]*invR[12] + C1[ 7]*invR[13] + C1[11]*invR[14];

      K[10] = C1[ 3]*invR[ 0] + C1[ 4]*invR[ 1] + C1[ 5]*invR[ 3] + C1[ 8]*invR[ 6] + C1[12]*invR[10];
      K[11] = C1[ 3]*invR[ 1] + C1[ 4]*invR[ 2] + C1[ 5]*invR[ 4] + C1[ 8]*invR[ 7] + C1[12]*invR[11];
      K[12] = C1[ 3]*invR[ 3] + C1[ 4]*invR[ 4] + C1[ 5]*invR[ 5] + C1[ 8]*invR[ 8] + C1[12]*invR[12];
      K[13] = C1[ 3]*invR[ 6] + C1[ 4]*invR[ 7] + C1[ 5]*invR[ 8] + C1[ 8]*invR[ 9] + C1[12]*invR[13];
      K[14] = C1[ 3]*invR[10] + C1[ 4]*invR[11] + C1[ 5]*invR[12] + C1[ 8]*invR[13] + C1[12]*invR[14];

      K[15] = C1[ 6]*invR[ 0] + C1[ 7]*invR[ 1] + C1[ 8]*invR[ 3] + C1[ 9]*invR[ 6] + C1[13]*invR[10];
      K[16] = C1[ 6]*invR[ 1] + C1[ 7]*invR[ 2] + C1[ 8]*invR[ 4] + C1[ 9]*invR[ 7] + C1[13]*invR[11];
      K[17] = C1[ 6]*invR[ 3] + C1[ 7]*invR[ 4] + C1[ 8]*invR[ 5] + C1[ 9]*invR[ 8] + C1[13]*invR[12];
      K[18] = C1[ 6]*invR[ 6] + C1[ 7]*invR[ 7] + C1[ 8]*invR[ 8] + C1[ 9]*invR[ 9] + C1[13]*invR[13];
      K[19] = C1[ 6]*invR[10] + C1[ 7]*invR[11] + C1[ 8]*invR[12] + C1[ 9]*invR[13] + C1[13]*invR[14];

      K[20] = C1[10]*invR[ 0] + C1[11]*invR[ 1] + C1[12]*invR[ 3] + C1[13]*invR[ 6] + C1[14]*invR[10];
      K[21] = C1[10]*invR[ 1] + C1[11]*invR[ 2] + C1[12]*invR[ 4] + C1[13]*invR[ 7] + C1[14]*invR[11];
      K[22] = C1[10]*invR[ 3] + C1[11]*invR[ 4] + C1[12]*invR[ 5] + C1[13]*invR[ 8] + C1[14]*invR[12];
      K[23] = C1[10]*invR[ 6] + C1[11]*invR[ 7] + C1[12]*invR[ 8] + C1[13]*invR[ 9] + C1[14]*invR[13];
      K[24] = C1[10]*invR[10] + C1[11]*invR[11] + C1[12]*invR[12] + C1[13]*invR[13] + C1[14]*invR[14];

      // X <- X1 + C1*inverse(C1+C2)*(X2-X1) =  X1 + K*(X2-X1) = X1 + K*d
      double d[5] { X2[0]-X1[0], X2[1]-X1[1], X2[2]-X1[2], X2[3]-X1[3], X2[4]-X1[4] };
      X[0] = X1[0] + K[ 0]*d[0] + K[ 1]*d[1] + K[ 2]*d[2] + K[ 3]*d[3] + K[ 4]*d[4];
      X[1] = X1[1] + K[ 5]*d[0] + K[ 6]*d[1] + K[ 7]*d[2] + K[ 8]*d[3] + K[ 9]*d[4];
      X[2] = X1[2] + K[10]*d[0] + K[11]*d[1] + K[12]*d[2] + K[13]*d[3] + K[14]*d[4];
      X[3] = X1[3] + K[15]*d[0] + K[16]*d[1] + K[17]*d[2] + K[18]*d[3] + K[19]*d[4];
      X[4] = X1[4] + K[20]*d[0] + K[21]*d[1] + K[22]*d[2] + K[23]*d[3] + K[24]*d[4];
    
      // C <-  C1 * inverse(C1+C2)  * C2 =  K * C2
      C[ 0] = K[ 0]*C2[ 0] + K[ 1]*C2[ 1] + K[ 2]*C2[ 3] + K[ 3]*C2[ 6] + K[ 4]*C2[10];
      C[ 1] = K[ 5]*C2[ 0] + K[ 6]*C2[ 1] + K[ 7]*C2[ 3] + K[ 8]*C2[ 6] + K[ 9]*C2[10];
      C[ 3] = K[10]*C2[ 0] + K[11]*C2[ 1] + K[12]*C2[ 3] + K[13]*C2[ 6] + K[14]*C2[10];
      C[ 6] = K[15]*C2[ 0] + K[16]*C2[ 1] + K[17]*C2[ 3] + K[18]*C2[ 6] + K[19]*C2[10];
      C[10] = K[20]*C2[ 0] + K[21]*C2[ 1] + K[22]*C2[ 3] + K[23]*C2[ 6] + K[24]*C2[10]; 

      C[ 2] = K[ 5]*C2[ 1] + K[ 6]*C2[ 2] + K[ 7]*C2[ 4] + K[ 8]*C2[ 7] + K[ 9]*C2[11];
      C[ 4] = K[10]*C2[ 1] + K[11]*C2[ 2] + K[12]*C2[ 4] + K[13]*C2[ 7] + K[14]*C2[11];
      C[ 7] = K[15]*C2[ 1] + K[16]*C2[ 2] + K[17]*C2[ 4] + K[18]*C2[ 7] + K[19]*C2[11];
      C[11] = K[20]*C2[ 1] + K[21]*C2[ 2] + K[22]*C2[ 4] + K[23]*C2[ 7] + K[24]*C2[11]; 

      C[ 5] = K[10]*C2[ 3] + K[11]*C2[ 4] + K[12]*C2[ 5] + K[13]*C2[ 8] + K[14]*C2[12];
      C[ 8] = K[15]*C2[ 3] + K[16]*C2[ 4] + K[17]*C2[ 5] + K[18]*C2[ 8] + K[19]*C2[12];
      C[12] = K[20]*C2[ 3] + K[21]*C2[ 4] + K[22]*C2[ 5] + K[23]*C2[ 8] + K[24]*C2[12]; 

      C[ 9] = K[15]*C2[ 6] + K[16]*C2[ 7] + K[17]*C2[ 8] + K[18]*C2[ 9] + K[19]*C2[13];
      C[13] = K[20]*C2[ 6] + K[21]*C2[ 7] + K[22]*C2[ 8] + K[23]*C2[ 9] + K[24]*C2[13]; 

      C[14] = K[20]*C2[10] + K[21]*C2[11] + K[22]*C2[12] + K[23]*C2[13] + K[24]*C2[14]; 
      // the following used to be more stable, but isn't any longer, it seems:
      //ROOT::Math::AssignSym::Evaluate(C, -2 * K * C1) ;
      //C += C1 + ROOT::Math::Similarity(K,R) ;
      return success;
}

double filter( double* X, double* C,
               const double* Xref, const double* H,
               double refResidual, double errorMeas2 )
{
      // The ugly code below makes the filter step about 20% faster
      // than SMatrix would do it.
      auto  res = refResidual +  H[0] * (Xref[0] - X[0]) 
                              +  H[1] * (Xref[1] - X[1])
                              +  H[2] * (Xref[2] - X[2])
                              +  H[3] * (Xref[3] - X[3]) 
                              +  H[4] * (Xref[4] - X[4]);
      double CHT[5] = {
       C[ 0]*H[0] + C[ 1]*H[1] + C[ 3]*H[2] + C[ 6]*H[3] + C[10]*H[4] ,
       C[ 1]*H[0] + C[ 2]*H[1] + C[ 4]*H[2] + C[ 7]*H[3] + C[11]*H[4] ,
       C[ 3]*H[0] + C[ 4]*H[1] + C[ 5]*H[2] + C[ 8]*H[3] + C[12]*H[4] ,
       C[ 6]*H[0] + C[ 7]*H[1] + C[ 8]*H[2] + C[ 9]*H[3] + C[13]*H[4] ,
       C[10]*H[0] + C[11]*H[1] + C[12]*H[2] + C[13]*H[3] + C[14]*H[4] 
      };
      auto  errorRes2  = errorMeas2 + H[0]*CHT[0] 
                                    + H[1]*CHT[1] 
                                    + H[2]*CHT[2]
                                    + H[3]*CHT[3] 
                                    + H[4]*CHT[4] ;

      // update the state vector and cov matrix
      auto w = res/errorRes2;
      X[0] += CHT[0] * w ;
      X[1] += CHT[1] * w ;
      X[2] += CHT[2] * w ;
      X[3] += CHT[3] * w ;
      X[4] += CHT[4] * w ;

      w = 1./errorRes2;
      C[ 0] -= w * CHT[0] * CHT[0] ;
      C[ 1] -= w * CHT[1] * CHT[0] ;
      C[ 3] -= w * CHT[2] * CHT[0] ;
      C[ 6] -= w * CHT[3] * CHT[0] ;
      C[10] -= w * CHT[4] * CHT[0] ;

      C[ 2] -= w * CHT[1] * CHT[1] ;
      C[ 4] -= w * CHT[2] * CHT[1] ;
      C[ 7] -= w * CHT[3] * CHT[1] ;
      C[11] -= w * CHT[4] * CHT[1] ;

      C[ 5] -= w * CHT[2] * CHT[2] ;
      C[ 8] -= w * CHT[3] * CHT[2] ;
      C[12] -= w * CHT[4] * CHT[2] ;

      C[ 9] -= w * CHT[3] * CHT[3] ;
      C[13] -= w * CHT[4] * CHT[3] ;

      C[14] -= w * CHT[4] * CHT[4] ;

      return res*res/errorRes2;
}

}}}
