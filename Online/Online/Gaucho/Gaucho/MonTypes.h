#ifndef MONTYPES_H
#define MONTYPES_H
#define  bintype double
enum MONTYPE
{
  H_ILLEGAL = 0xFEED0000,
  H_1DIM,
  H_2DIM,
  H_PROFILE,
  H_RATE,
  C_INT,
  C_LONGLONG,
  C_FLOAT,
  C_DOUBLE,
  C_STATENT,
  C_VOIDSTAR
};
#endif
