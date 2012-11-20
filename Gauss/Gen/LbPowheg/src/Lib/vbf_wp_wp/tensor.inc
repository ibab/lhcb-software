c  precalculated lepton tensors for WW --> l nubar lbar nu
c
c  lepton spinors and W+- polarization vectors
      double complex wep, wve, wmu, wvm, wp, wm, wpp, wmp, xp, xm
      double precision qp, qm
      common /vbf_wp_wp_wpmspinor/ wep(6), wve(6), wmu(6), wvm(6),
     1                   wp(6), wm(6), qp(0:4), qm(0:4),
     2                   wpp(6), wmp(6), xp, xm 
c  leptonic tensors
      double complex aww, zww, aaww, azww, zaww, zzww, 
     1           NCwpa, NCwpz, CCwpa, CCwpz, wwww5, 
     2           NCwma, NCwmz, CCwma, CCwmz, wwww6
      logical lfs
      common /vbf_wp_wp_tensww/ aww(0:5), zww(0:5), aaww(0:3,0:3,3), 
     1                azww(0:3,0:3,3), zaww(0:3,0:3,3), 
     2                zzww(0:3,0:3,3), wwww5(0:3,0:3,3), 
     3                wwww6(0:3,0:3,3),
     4                NCwpa(0:3,0:3,2,3), NCwpz(0:3,0:3,2,3), 
     5                CCwpa(0:3,0:3,2,3), CCwpz(0:3,0:3,2,3),
     6                NCwma(0:3,0:3,2,3), NCwmz(0:3,0:3,2,3), 
     7                CCwma(0:3,0:3,2,3), CCwmz(0:3,0:3,2,3), lfs(8)