c -*- Fortran -*-
      real * 8  hist,xhis,hdel,hmin
     &,hmax,uscore,oscore,havg,hint,hsig
      character * 3 book
      character * 50 title
      integer nmh,nmh2,nmh3,nmh4,nmb,nxm,nbin,
     1          ihis,iuscore,ioscore,ient,nhist
      parameter(nmh=300,nmh2=2*nmh,nmh3=3*nmh,
     1          nmh4=4*nmh,nmb=5*nmh,nxm=1000)
      common/ww_histo/hist(nmb,nxm),xhis(nmb,nxm),hdel(nmb),hmin(nmb)
     &,hmax(nmb),uscore(nmb),oscore(nmb)
     &,nbin(nmb),ient(nmb),havg(nmb),hint(nmb),hsig(nmb)
     &,ihis(nmb,nxm),iuscore(nmb),ioscore(nmb),nhist
     &,book(nmb),title(nmb)
      real * 8 hist2,xhis2,yhis2
     &,hdel2,xproj,yproj,hmin2
     &,hmax2,havg2,hint2,hsig2
      integer nbin2,ihis2,ioscore2,ient2,nhist2
      character * 3 book2
      character * 50 title2
      common/ww_histo2/hist2(10,100,100),xhis2(10,100),yhis2(10,100)
     &,hdel2(2,10),xproj(10,100),yproj(10,100),hmin2(2,10)
     &,hmax2(2,10),havg2(2,10),hint2(10),hsig2(2,10)
     &,nbin2(2,10),ihis2(10,100,100),ioscore2(10)
     &,ient2(10),nhist2,book2(10),
     & title2(10)


