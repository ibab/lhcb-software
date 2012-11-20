      subroutine zz_genericpdf(ndns,ih,xmu2,x,fx)
c Interface to zz_MLMPDF package.
      implicit none
      integer ndns,ih
      real * 8 xmu2,x,fx(-6:6)
      real * 4 sxmu2,sx,sfx(-5:5)
      integer j
      sx=x
      sxmu2=xmu2
      call zz_MLMPDF(ndns,ih,sxmu2,sx,sfx,5)
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
      subroutine zz_getq2min()
      end

      subroutine zz_genericpdfpar(ndns,ih,xlam,scheme,iorder,iret)
      implicit none
      integer ndns,ih
      real * 8 xlam
      character * 2 scheme
      integer iret,iorder
      call zz_PDFPAR(ndns,ih,xlam,scheme,iret)
c not yet implemented
      iorder=-1
      end

      function zz_whichpdfpk()
      character * 3 zz_whichpdfpk
      zz_whichpdfpk='mlm'
      end
