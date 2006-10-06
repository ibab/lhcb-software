#ifndef HIST_TYPES
#define HIST_TYPES
#define L_TITLE 80
typedef enum
{
	H_ILLEGAL,
  H_1DIM,
  H_2DIM,
  H_PROFILE
}HTYPE;

typedef float bintype;
typedef struct
{
	int netries;
	bintype sum;
	double sum2;
} bindesc;
#endif
