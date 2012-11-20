      subroutine w_btildecoll(xrad,rescoll,www)
      implicit none
      include 'src/Lib/w/nlegborn.h'
      include 'src/Lib/w/pwhg_flst.h'
      include 'src/Lib/w/pwhg_kn.h'
      include 'src/Lib/w/pwhg_br.h'
      include 'src/Lib/w/pwhg_math.h'
      include 'src/Lib/w/pwhg_st.h'
      include 'src/Lib/w/pwhg_flg.h'
      include 'src/Lib/w/pwhg_par.h'
      real * 8 xrad,rescoll(flst_nborn),www
      real * 8 un
      parameter (un=1d0)
c pdfb: born pdf's, pdfs: pdfs with scaled x->x/z
      real * 8 pdfb1(-6:6),pdfb2(-6:6),pdfs1(-6:6),pdfs2(-6:6),
     1         pdfs1sng,pdfs2sng
      real * 8 z,omzpgg,omzpqq,ppqq,omzpqg,ppqg,omzpgq,ppgq,
     1         sb,tot,plfrc,plfr0,z1,z2,xjac1,xjac2,rm1,rm2,res1,res2,
     2         x,xjac
      integer j,jb,fl1,fl2
      logical w_pwhg_isfinite
      external w_pwhg_isfinite
c Statement Functions
c omz:(1-z)*pgg(z), 2.106 of FNO2007
      omzpgg(z)=2*ca*(z+(1-z)**2/z+z*(1-z)**2)
c 2.103
      omzpqq(z)=cf*(1+z**2)
c ppqq: (p prime qq) d pqq'/ d epsilon, from 2.103
      ppqq(z)=-cf*(1-z)
c 2.104
      omzpqg(z)=cf*(1+(1-z)**2)/z*(1-z)
      ppqg(z)=-cf*z
c 2.105
      omzpgq(z)=tf*(1-2*z*(1-z))*(1-z)
      ppgq(z)=-tf*2*z*(1-z)
c log coeff., from 2.102, with partonic s=sb/z
      plfrc(z)=1/(1-z)*log(sb/z/st_mufact2)+2*log(1-z)/(1-z)
c same, with w_soft limit s
      plfr0(z)=1/(1-z)*log(sb/st_mufact2)+2*log(1-z)/(1-z)
c End Statement Functions
      if(flg_collremnsamp) then
         xjac=630*(1-xrad)**4*xrad**4
         x=xrad**5*(70*xrad**4-315*xrad**3+540*xrad**2-420*xrad+126)
      else
         xjac=1
         x=xrad
      endif
c      z1=1-par_isrtinycsi-(1-kn_xb1-par_isrtinycsi)*x
      z1=1-(1-kn_xb1)*par_isrtinycsi-(1-kn_xb1)*(1-par_isrtinycsi)*x
      xjac1=(1-kn_xb1)*xjac
c      z2=1-par_isrtinycsi-(1-kn_xb2-par_isrtinycsi)*x
      z2=1-(1-kn_xb2)*par_isrtinycsi-(1-kn_xb2)*(1-par_isrtinycsi)*x
      xjac2=(1-kn_xb2)*xjac

      sb=kn_sborn
c See 7.224 and 7.225 of FNO2007
c Remnant: 1/(1-z)_csicut=1/(1-z)_(1-x)+log((1-x)/csicut) delta(1-z)
c Remnant: log(1-z)/(1-z)_csicut=log(1-z)/(1-z)_(1-x)+
c          (log(1-x)^2-log(csicut)^2)/2 delta(1-z)
      rm1=log((1-kn_xb1)/par_csicut)*log(sb/st_mufact2)
     #        +(log(1-kn_xb1)**2-log(par_csicut)**2)
      rm2=log((1-kn_xb2)/par_csicut)*log(sb/st_mufact2)
     #        +(log(1-kn_xb2)**2-log(par_csicut)**2)
c get pdfs at underlying born x values
      call w_pdfcall(1,kn_xb1,pdfb1)
      call w_pdfcall(2,kn_xb2,pdfb2)
c get pdfs at underlying born x/z value values
      call w_pdfcall(1,kn_xb1/z1,pdfs1)
      call w_pdfcall(2,kn_xb2/z2,pdfs2)
c Compute the singlet
      pdfs1sng=0
      pdfs2sng=0
      do j=1,st_nlight
         pdfs1sng=pdfs1sng+pdfs1(j)+pdfs1(-j)
         pdfs2sng=pdfs2sng+pdfs2(j)+pdfs2(-j)
      enddo
c loop over all Born configurations
      tot=0
      do jb=1,flst_nborn
         fl1=flst_born(1,jb)
         fl2=flst_born(2,jb)
         if(fl1.eq.0) then
c FNO2007, 2.102
c gg remnant
c Remember 1/z factor, 4.23 and 7.206 of FNO2007
            res1=        omzpgg(z1)*plfrc(z1)/z1  * pdfs1(fl1)*xjac1 
     #                 - omzpgg(un)*plfr0(z1)     * pdfb1(fl1)*xjac1
     #                 + omzpgg(un)*rm1           * pdfb1(fl1)
c qg remnant
            res1=res1 + (omzpqg(z1)*plfrc(z1)
     #                             - ppqg(z1))/z1 * pdfs1sng*xjac1 
         else
c qq remnant            
            res1=       (omzpqq(z1)*plfrc(z1)
     #                             - ppqq(z1))/z1 * pdfs1(fl1)*xjac1 
     #                 - omzpqq(un)*plfr0(z1)     * pdfb1(fl1)*xjac1
     #                 + omzpqq(un)*rm1           * pdfb1(fl1)
c gq remnant
            res1=res1 + (omzpgq(z1)*plfrc(z1)
     #                           - ppgq(z1))/z1   * pdfs1(0)*xjac1 
         endif
c Copy the above, replace 1 with 2 with the editor
         if(fl2.eq.0) then
c FNO2007, 2.102
c gg remnant
            res2=        omzpgg(z2)*plfrc(z2)/z2  * pdfs2(fl2)*xjac2 
     #                 - omzpgg(un)*plfr0(z2)     * pdfb2(fl2)*xjac2
     #                 + omzpgg(un)*rm2           * pdfb2(fl2)
c qg remnant
            res2=res2 + (omzpqg(z2)*plfrc(z2)
     #                             - ppqg(z2))/z2 * pdfs2sng*xjac2 
         else
c qq remnant            
            res2=       (omzpqq(z2)*plfrc(z2)
     #                             - ppqq(z2))/z2 * pdfs2(fl2)*xjac2 
     #                 - omzpqq(un)*plfr0(z2)     * pdfb2(fl2)*xjac2
     #                 + omzpqq(un)*rm2           * pdfb2(fl2)
c gq remnant
            res2=res2 + (omzpgq(z2)*plfrc(z2)
     #                           - ppgq(z2))/z2   * pdfs2(0)*xjac2 
         endif
         rescoll(jb)=( res1*pdfb2(fl2)+res2*pdfb1(fl1) )
     #    *br_born(jb)*st_alpha/(2*pi)*kn_jacborn
         tot=tot+rescoll(jb)
      enddo
      if (.not.w_pwhg_isfinite(tot)) then
         do jb=1,flst_nborn
            rescoll(jb)=0d0
         enddo
         tot=0d0
      endif
      if(flg_nlotest) then
         tot=tot*www
         call w_analysis_driver(tot,0)
      endif
      end
