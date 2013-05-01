/* Common block decalaration */
#define  himain1 himain1_
#define  himain2 himain2_ 
#define  hijjet1 hijjet1_
#define  hijjet2 hijjet2_
#define  hiparnt hiparnt_
#define  hijjet4 hijjet4_
#define  hijcrdn hijcrdn_

extern struct HIMAIN1 {
  int natt;
  int nattpd;
  double eatt;
  int jatt;
  int nt;
  int np;
  int n0;
  int n01;
  int n10;
  int n11;
} himain1;


extern struct HIMAIN2 {
  int katt[4][130000];
  double patt[4][130000];
} himain2;


extern struct HIJJET1 {
  int npj[300];
  int kfpj[500][300];
  double pjpx[500][300];
  double pjpy[500][300];
  double pjpz[500][300];
  double pjpe[500][300];
  double pjpm[500][300];
  int ntj[300];
  int kftj[500][300];
  double pjtx[500][300];
  double pjty[300][500];
  double pjtz[300][500];
  double pjte[300][500];
  double pjtm[300][500];
} hijjet1;


extern struct HIJJET2 {
  int nsg;
  int nsgpd;
  int njsg[900];
  int iasg[3][900];
  int k1sg[100][900];
  int k2sg[900][100];
  double pxsg[100][900];
  double pysg[100][900];
  double pzsg[100][900];
  double pesg[100][900];
  double pmsg[900][100];
} hijjet2;

extern struct HIPARNT {
  double hipr1[100];
  int ihpr2[50];
  double hint1[100];
  int ihnt2[50];
} hiparnt;
