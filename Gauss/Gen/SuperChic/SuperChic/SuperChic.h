// Generated automatically with Gen/SuperChic/python/SuperChic.py.
// Do not manually modify. Based on superchicv1.47.f.
extern "C" {

  void superchic_(int *entryp, int *exitp);

  extern struct {
    char corr[10], gen[10], pflag[10], phdecay[10], photo[10], prefix[50], 
      surv[10], xflag[10];
    int ap[1], chiflag[1], chiv[1], cross[1], d[1], decay[1], e[1], eflagp[1], 
      h[1], i[1], icut[1], id[20], id0[1], id0m[1], id1[1], id2[1], idh1[1], 
      idh2[1], idh3[1], idh4[1], iord[1], iset[1], j[1], j1[1], j2[1], j3[1], 
      j4[1], k[1], kk[1], l[1], m[1], mm[1], n[1], naf[1], ncut[1], nev[1], 
      nhist[1], npart[1], nrun[1], ntotal[1], ntotal1[1], num[1], o[1], p[1], 
      r[1];
    double a21[1], a21n[1], a28[1], a28n[1], a2g[1], a2gn[1], aa1[1], aa2[1], 
      alfas[1], alphap[1], alphapb[1], alphas0[1], alphasg[1], alphasg0[1], 
      alphasp[1], am[100], amp[1], amp1[1], amp2[1], amp3[1], asmur[1], b[1], 
      b0[1], b1[1], bare[1], bb[1], beff[1], beff0[1], beff0m[1], beff1[1], 
      beff2[1], beta[1], beta0[1], bm2[1], bp[1], bp2[1], bpsi[1], bpsi0[1], 
      brchi0had[1], brchi1had[1], brchi2had[1], brchihad[1], brchipsi[1], 
      brchipsi0[1], brchipsi1[1], brchipsi2[1], bretahad[1], brpsimu[1], 
      cc1[1], cc2[1], ccut[1], cf[1], clpm[1], clpp[1], conv[1], cost[1], 
      cost6[1], cost8[1], costg[1], costpi[1], cphi6[1], cphi8[1], cphig[1], 
      cpi[1], d1[1], d2[1], delta[1], deltad[1], e0[4], e0rot[4], ebeam[1], 
      ein[1], epm[1], epp[1], eppg[1], errbrchi[1], errbrchi0[1], errbrchi1[1], 
      errbrchi2[1], errbrpsi[1], fe[1], fm[1], fn[1], fnorm[1], fnt[1], fnx[1], 
      fny[1], fpi[1], fpi1[1], fpi8[1], fpip[1], fr2[1], g[4][4], gamgg[10], 
      gamgq[10], gamm[10], gamp[10], gamqg[10], gamqq[10], ge[1], glu[1], 
      gm[1], lambdacap[1], mb[1], mc[1], mchi0[1], mchi1[1], mchi2[1], mepm[1], 
      mepp[1], meppg[1], meta[1], mgam[1], mh[1], mh1[1], mh2[1], mjpsi[1], 
      mmax[1], mmes[1], mmes1[1], mmes2[1], mmin[1], mmu[1], mp[1], mpsi[1], 
      mt[1], mu[1], mum[1], mur[1], mxx[1], my[1], n1[4], n1rot[4], n2[4], 
      n2rot[4], n3[4], nc[1], neff[1], neff0[1], neff1[1], neff2[1], nf[1], 
      ng[1], norm[1], norm0[1], norm0m[1], norm1[1], norm2[1], normpsip[1], 
      num1[1], p1m[1], p1p[1], p2m[1], p2p[1], pboo[4], pcm[4], pcmod[1], 
      pepm[1], pepp[1], peppg[1], pgmod[1], pgtot[4], phi[1], phi1[1], phi2[1], 
      phi6[1], phi8[1], phig[1], pi[1], picostsq[1], pin[4], plb[4], pmod[1], 
      pnorm[1], pout[4][100], ppi[1], pt1sq[1], pt1x[1], pt1y[1], pt2sq[1], 
      pt2x[1], pt2y[1], ptdif[2], ptmax[1], ptmin[1], ptw[1], ptxsq[1], 
      ptxx[1], q14[4], q14c[4], q14q15[1], q15[4], q15c[4], q5[4], q6[4], 
      q6c[4], q6q7[1], q7[4], q7c[4], q8[4], q8q9[1], q9[4], qf[1], qs[1], 
      qsq[1], qsqmin[1], ran0[1], ran1[1], ran10[1], ran11[1], ran12[1], 
      ran2[1], ran3[1], ran4[1], ran5[1], ran6[1], ran7[1], ran8[1], ran9[1], 
      rang1[1], rang2[1], ranhist[1], ranpsi[1], ranyp[1], rec[20][20], 
      rhom[10], rhop[10], rm[1], rmx[1], root1sq[1], root2sq[1], rpsip[1], 
      rt2[1], rtso[1], senh[1], senh0[1], senh1[1], senh2[1], sh[1], sign[1], 
      sint6[1], sint8[1], sintg[1], sphi6[1], sphi8[1], sphig[1], sum[1], 
      sum1[1], sum2[1], sum3[1], sum4[1], sumr[1], sumr1[1], sumt[1], sv[1], 
      sv0[1], sv0m[1], sv1[1], sv2[1], svj0[1], svj2[1], t[1], t1[1], t2[1], 
      theta1[1], thetap[1], thetap1[1], thetap8[1], tvec[4], u[1], uvec[4], 
      v[1], var[1], vmp[1], vpm[1], w0[1], w0b[1], w0psip[1], w2[1], weight[1], 
      weight1[1], weight2[1], weightgam[1], weightm[1], weightnf[1], 
      weightnfpi[1], weightp[1], weighty[1], width[1], wl[1], wpsi[1], wt[1], 
      wthist[1], ww1[1], x[3], x1[1], x11[1], x2[1], x22[1], xcap2[1], xgam[1], 
      xglu[1], xgmin[1], xnorm[1], xo[1], xoo[1], y[3], ycap2[1], ycut[1], 
      ym[1], ymax[1], ymin[1], ynorm[1], yp[1], ypmax[1], ypmin[1];
    struct {double r, i;} cechi1[4], cechi1p[4], cechi2[4][4], cechi2p[4][4], 
      cepsi1[4], cepsi1c[4], cepsi1p[4], echi0[4], echi1[4], echi1p[4], 
      echi2[4][4], echi20[4][4], echi2m1[4][4], echi2m2[4][4], echi2p[4][4], 
      echi2p1[4][4], echi2p2[4][4], echiminus[4], echiplus[4], epsi0[4], 
      epsi1[4], epsi1c[4], epsi1p[4], epsiminus[4], epsiplus[4], rho1chi[3][3], 
      rho1psi[3][3], rho2chi[5][5], zammpp[1], zamp[1], zamp1[1], zamp2[1], 
      zamppp[1], zapmmp[1], zapppp[1], zcechi1pcepsi1[1], zcechi1pechi1[1], 
      zechi1cechi1p[1], zechi1epsi1p[1], zepsi1cepsi1p[1], zepsi1pcepsi1[1], 
      zi[1], zq14[4], zq14cechi1p[1], zq14echi1[1], zq15[4], zq15cechi1p[1], 
      zq15echi1[1], zq5[4], zq5epsi1p[1], zq6[4], zq6cechi1p[1], zq6cepsi1[1], 
      zq6echi1[1], zq6epsi1p[1], zq7[4], zq7epsi1p[1], zq8[4], zq8cepsi1p[1], 
      zq8epsi1[1], zweight1[1];
  } sccom_;

  extern struct {
    double s[1], rts[1], mchi[1], ptcut[1], etacut[1], ecut[1], rcut[1], yx[1], 
      mx[1];
  } vars_;

  extern struct {
    int iphi[1], i1[1], i2[1], ef[1], n[1];
  } it_;

  extern struct {
    double grid[5][5][50][50][50][50];
  } dat_;

  extern struct {
    double rg[1], incp[1];
    int n1[1], ii[1], ll[1];
  } init_;

  extern struct {
    int pdf[1], eflag[1];
  } flags_;

  extern struct {
    double q[4][20];
  } mom_;

  extern struct {
    int nt[1], nphi[1];
    double ht[1], hphi[1], bh[1];
  } dlse_;

  extern struct {
    int outevt[1], outhst[1];
  } output_;

  extern struct {
    double phi1[1], phi2[1];
  } az_;

  extern struct {
    double pt1[2], pt2[2], ptx[2];
  } mompt_;

  extern struct {
    int af[1];
  } it1_;

}
