// $Id: MuonGeometry.h,v 1.4 2002-08-05 12:52:24 dhcroft Exp $
#ifndef MUONGEOMETRY_H 
#define MUONGEOMETRY_H 1

// Include files

/** @namespace MuonGeometry MuonGeometry.h
 *  
 *
 *  @author David Hutchcroft
 *  @date   11/03/2002
 */
namespace MuonGeometry {
  // the number of X and Y indices in a tweleth of the region ie total/4
  // this applies everywhere
  // defines chambers all stations are the same....
  static const unsigned int chamberGridX[4] = {1,1,1,2};
  static const unsigned int chamberGridY[4] = {1,2,4,8};

  // these are the chamber numbers in x,y coordinates across a region
  // if you turn your head 90deg clockwise they are in the 
  // x,y pattern as seen from z=0
  static const unsigned int chamberR1[4][4] = {
    { 9, 7, 4, 6}, // x=0 column
    { 8, 0, 0, 5}, 
    {11, 0, 0, 2},
    {12,10, 1, 3} // x=3 column
  };
  
  static const unsigned int chamberR2[4][8] = {
    {18,16,14,13, 7, 8,10,12}, // x=0 column
    {17,15, 0, 0, 0, 0, 9,11},
    {23,21, 0, 0, 0, 0, 3, 5},
    {24,22,20,19, 1, 2, 4, 6} // x=3 column
  };
  
  static const unsigned int chamberR3[4][16] = {
    {36,34,32,30,28,27,26,25,13,14,15,16,18,20,22,24}, // x=0 column
    {35,33,31,29, 0, 0, 0, 0, 0, 0, 0, 0,17,19,21,23},
    {47,45,43,41, 0, 0, 0, 0, 0, 0, 0, 0, 5, 7, 9,11},
    {48,46,44,42,40,39,38,37, 1, 2, 3, 4, 6, 8,10,12} // x=3 column
  };

  static const unsigned int chamberR4[8][32] = {
    {144,140,136,132,128,124,120,116,112,110,108,106,104,102,100, 98,
      50, 52, 54, 56, 58, 60, 62, 64, 68, 72, 76, 80, 84, 88, 92, 96},
    // x=0 column
    {143,139,135,131,127,123,119,115,111,109,107,105,103,101, 99, 97,
      49, 51, 53, 55, 57, 59, 61, 63, 67, 71, 75, 79, 83, 87, 91, 95},
    {142,138,134,130,126,122,118,114,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0, 66, 70, 74, 78, 82, 86, 90, 94},
    {141,137,133,129,125,121,117,113,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0, 65, 69, 73, 77, 81, 85, 89, 93},
    {189,185,181,177,173,169,165,161,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0, 17, 21, 25, 29, 33, 37, 41, 45},
    {190,186,182,178,174,170,166,162,  0,  0,  0,  0,  0,  0,  0,  0,
       0,  0,  0,  0,  0,  0,  0,  0, 18, 22, 26, 30, 34, 38, 42, 46},
    {191,187,183,179,175,171,167,163,159,157,155,153,151,149,147,145,
       1,  3,  5,  7,  9, 11, 13, 15, 19, 23, 27, 31, 35 ,39, 43, 47},
    {192,188,184,180,176,172,168,164,160,158,156,154,152,150,148,146,
       2,  4,  6,  8, 10, 12, 14, 16, 20, 24, 28, 32, 36, 40, 44, 48}
    // x=7 column
  };

};
#endif // MUONGEOMETRY_H
