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
  int eatt;
  int jatt;
  int np;
  int nt;
  int n0;
  int n01;
  int n10;
  int n11;
} himain1;


extern struct HIMAIN2 {
  int katt[4][130000];
  float patt[4][130000];
} himain2;


extern struct HIJJET1 {
  int npj[300];
  int kfpj[500][300];
  float pjpx[500][300];
  float pjpy[500][300];
  float pjpz[500][300];
  float pjpe[500][300];
  float pjpm[500][300];
  int ntj[300];
  int kftj[500][300];
  float pjtx[500][300];
  float pjty[300][500];
  float pjtz[300][500];
  float pjte[300][500];
  float pjtm[300][500];
} hijjet1;


extern struct HIJJET2 {
  int nsg;
  int njsg[900];
  int iasg[3][900];
  int k1sg[100][900];
  int k2sg[900][100];
  float pxsg[100][900];
  float pysg[100][900];
  float pzsg[100][900];
  float pesg[100][900];
  float pmsg[900][100];
} hijjet2;

extern struct HIPARNT {
  float hipr1[100];
  int ihpr2[50];
  float hint1[100];
  int ihnt2[50];
} hiparnt;


