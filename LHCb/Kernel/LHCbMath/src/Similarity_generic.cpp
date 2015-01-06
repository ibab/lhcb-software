
namespace LHCb {
namespace Math {
namespace similarity_5_generic {

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


}}}
