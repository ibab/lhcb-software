      subroutine dijet_pdfcall(ih,x,pdf)
      implicit none
      integer ih
      real * 8 x,pdf(-6:6)
      include 'src/Lib/dijet/pwhg_st.h'
      include 'src/Lib/dijet/pwhg_pdf.h'
      if(ih.eq.1) then
         call dijet_genericpdf0(pdf_ndns1,pdf_ih1,st_mufact2,x,pdf)
      elseif(ih.eq.2) then
         call dijet_genericpdf0(pdf_ndns2,pdf_ih2,st_mufact2,x,pdf)
      else
         write(*,*) ' dijet_pdfcall: invalid call, ih=',ih
         stop
      endif
      end


c Front end to dijet_genericpdf; it stores the arguments and return values of
c the nrec most recent calls to dijet_genericpdf. When invoked it looks in the
c stored calls; if a match its found, its return value is used.
c In this framework it is found that nrec=8 would be enough.
c This provides a remarkable increase in spead (better than a factor of 3)
c when dijet_CTEQ6 pdf are used.
      subroutine dijet_genericpdf0(ns,ih,xmu2,x,fx)
      implicit none
      integer ns,ih
      real * 8 xmu2,x,fx(-6:6)
      integer nrec
      parameter (nrec=10)
      real * 8 oxmu2(nrec),ox(nrec),ofx(-6:6,nrec)
      integer ons(nrec),oih(nrec)
      integer irec
      save oxmu2,ox,ofx,ons,oih,irec
c set to impossible values to begin with
      data ox/nrec*-1d0/
      data irec/0/
      integer j,k
      real * 8 charmthr2,bottomthr2
      logical ini
      data ini/.true./
      save ini,charmthr2,bottomthr2
      real * 8 dijet_powheginput
      external dijet_powheginput
      if(ini) then
         charmthr2=dijet_powheginput('#charmthrpdf')
         bottomthr2=dijet_powheginput('#bottomthrpdf')
         if(charmthr2.lt.0) charmthr2=1.5
         if(bottomthr2.lt.0) bottomthr2=5
         charmthr2=charmthr2**2
         bottomthr2=bottomthr2**2
         ini=.false.
      endif
      do j=1,nrec
         if(x.eq.ox(j)) then
            if(xmu2.eq.oxmu2(j)) then
               if(ns.eq.ons(j).and.ih.eq.oih(j)) then
                  do k=-6,6
                     fx(k)=ofx(k,j)
                  enddo
                  return
               endif
            endif
         endif
      enddo
      irec=irec+1
      if(irec.gt.nrec) irec=1
      ons(irec)=ns
      oih(irec)=ih
      oxmu2(irec)=xmu2
      ox(irec)=x
      call dijet_genericpdf(ns,ih,xmu2,x,ofx(-6,irec))
c Flavour thresholds:
      if(xmu2.lt.bottomthr2) then
         ofx(5,irec)=0
         ofx(-5,irec)=0
      endif
      if(xmu2.lt.charmthr2) then
         ofx(4,irec)=0
         ofx(-4,irec)=0
      endif
      do k=-6,6
         fx(k)=ofx(k,irec)
      enddo
      end

