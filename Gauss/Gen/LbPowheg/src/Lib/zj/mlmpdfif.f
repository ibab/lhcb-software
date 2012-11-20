      subroutine zj_genericpdf(ndns,ih,xmu2,x,fx)
c Interface to zj_MLMPDF package.
      implicit none
      integer ndns,ih
      real * 8 xmu2,x,fx(-6:6)
      real * 4 sxmu2,sx,sfx(-5:5)
      integer j
      sx=x
      sxmu2=xmu2
      call zj_MLMPDF(ndns,ih,sxmu2,sx,sfx,5)
      do j=-5,5
         fx(j)=sfx(j)
      enddo
      fx(1)=sfx(2)
      fx(-1)=sfx(-2)
      fx(2)=sfx(1)
      fx(-2)=sfx(-1)
      fx(6)=0
      fx(-6)=0
      end

c This subroutine is in LHAPDF, and is invoked in
c setstrongcoupl.f in case the lhapdfif.f (and LHAPDF) is linked in
c If not, like now, it should be set to a dummy function to avoid
c link errors. It is never invoked in the present case.
      subroutine zj_getq2min()
      end

      subroutine zj_genericpdfpar(ndns,ih,xlam,zj_scheme,iorder,iret)
      implicit none
      integer ndns,ih
      real * 8 xlam
      character * 2 zj_scheme
      integer iret,iorder
      call zj_PDFPAR(ndns,ih,xlam,zj_scheme,iret)
c not yet implemented
      iorder=-1
      end

      function zj_whichpdfpk()
      character * 3 zj_whichpdfpk
      zj_whichpdfpk='mlm'
      end
