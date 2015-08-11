// Generated automatically with Gen/LPair/python/LPair.py.
// Do not manually modify. Based on LPair.120920.
extern "C" {

  void zduevt_(int *iwant);

  void lpair_photos__(int *flags, float *omegamin);

  void zduini_();

  extern struct {
    double inpe[1], inpp[1];
    int intge[1], intgp[1], gpdf[1], spdf[1], pmod[1], emod[1], ipair[1], 
      nquark[1];
  } beam_;

  extern struct {
    int ndim[1], ncvg[1], itmx[1], nprn[1], igraph[1], npoin[1], nprin[1], 
      ntreat[1], ibeg[1], iend[1];
  } vegpar_;

  extern struct {
    double cotth1[1], cotth2[1], ecut[1], ptcut[1], mxmin2[1], mxmax2[1];
    float thmax[1], thmin[1];
    double qp2min[1], qp2max[1];
    int modcut[1];
    float mxmn[1], mxmx[1], q2mn[1], q2mx[1];
  } cuts_;

  extern struct {
    int mff[1];
  } mgmge_;

  extern struct {
    int mvacpol[1];
  } vacpol_cb__;

  extern struct {
    int mstu[200];
    float paru[200];
    int mstj[200];
    float parj[200];
  } ludat1_;

  extern struct {
    float dmmin[1];
  } zmdiffcut_;

  extern struct {
    int mint[400];
    float vint[400];
  } pyint1_;

  extern struct {
    int mstp[200];
    float parp[200];
    int msti[200];
    float pari[200];
  } pypars_;

  extern struct {
    int n[1], k[4000][5];
    float p[4000][5], v[4000][5];
  } lujets_;

}
