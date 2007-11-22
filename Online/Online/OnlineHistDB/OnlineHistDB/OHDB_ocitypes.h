// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Online/OnlineHistDB/OnlineHistDB/OHDB_ocitypes.h,v 1.2 2007-11-22 17:38:35 ggiacomo Exp $
#ifndef OHDB_OCITYPES_H
#define OHDB_OCITYPES_H 1
struct dispopt_s
{
  OCIString * LABEL_X;
  OCIString * LABEL_Y;
  OCIString * LABEL_Z;
  OCINumber YMIN;
  OCINumber YMAX;
  OCINumber STATS;
  OCINumber FILLSTYLE;
  OCINumber FILLCOLOR;
  OCINumber LINESTYLE;
  OCINumber LINECOLOR;
  OCINumber LINEWIDTH;
  OCIString * DRAWOPTS;
  OCINumber XMIN;
  OCINumber XMAX;
  OCINumber ZMIN;
  OCINumber ZMAX;
  OCINumber LOGX;
  OCINumber LOGY;
  OCINumber LOGZ;
  OCIString * TIMAGE;
  OCIString * REF;
  OCINumber REFRESH;
  OCINumber TIT_X_SIZE;
  OCINumber TIT_X_OFFS;
  OCINumber TIT_Y_SIZE;
  OCINumber TIT_Y_OFFS;
  OCINumber TIT_Z_SIZE;
  OCINumber TIT_Z_OFFS;
  OCINumber LAB_X_SIZE;
  OCINumber LAB_X_OFFS;
  OCINumber LAB_Y_SIZE;
  OCINumber LAB_Y_OFFS;
  OCINumber LAB_Z_SIZE;
  OCINumber LAB_Z_OFFS;
  OCINumber GRIDX;
  OCINumber GRIDY;
  OCINumber THETA;
  OCINumber PHI;
  OCIString * CNTPLOT;
};
typedef struct dispopt_s dispopt;

struct dispopt_ind
{
  OCIInd _atomic;
  OCIInd LABEL_X;
  OCIInd LABEL_Y;
  OCIInd LABEL_Z;
  OCIInd YMIN;
  OCIInd YMAX;
  OCIInd STATS;
  OCIInd FILLSTYLE;
  OCIInd FILLCOLOR;
  OCIInd LINESTYLE;
  OCIInd LINECOLOR;
  OCIInd LINEWIDTH;
  OCIInd DRAWOPTS;
  OCIInd XMIN;
  OCIInd XMAX;
  OCIInd ZMIN;
  OCIInd ZMAX;
  OCIInd LOGX;
  OCIInd LOGY;
  OCIInd LOGZ;
  OCIInd TIMAGE;
  OCIInd REF;
  OCIInd REFRESH;
  OCIInd TIT_X_SIZE;
  OCIInd TIT_X_OFFS;
  OCIInd TIT_Y_SIZE;
  OCIInd TIT_Y_OFFS;
  OCIInd TIT_Z_SIZE;
  OCIInd TIT_Z_OFFS;
  OCIInd LAB_X_SIZE;
  OCIInd LAB_X_OFFS;
  OCIInd LAB_Y_SIZE;
  OCIInd LAB_Y_OFFS;
  OCIInd LAB_Z_SIZE;
  OCIInd LAB_Z_OFFS;
  OCIInd GRIDX;
  OCIInd GRIDY;
  OCIInd THETA;
  OCIInd PHI;
  OCIInd CNTPLOT;
};
typedef struct dispopt_ind dispoptInd;

#endif // OHDB_OCITYPES_H
