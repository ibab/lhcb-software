#ifndef HIST_TYPES
#define HIST_TYPES
#define  bintype double
enum HTYPE
{
  H_ILLEGAL = 0xFEED0000,
  H_1DIM,
  H_2DIM,
  H_PROFILE,
  H_RATE,
  C_INT,
  C_LONGLONG,
  C_FLOAT,
  C_DOUBLE
};
#endif
