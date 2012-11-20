c     returns 2 Re(M_B * M_V)/(as/(2pi)), 
c     where M_B is the Born amplitude and 
c     M_V is the finite part of the virtual amplitude
c     The as/(2pi) factor is attached at a later point
      subroutine wz_setvirtual(pin,vflav,virtual)
      implicit none
      include 'src/Lib/wz/nlegborn.h'
      include 'src/Lib/wz/pwhg_st.h'
      include 'src/Lib/wz/pwhg_math.h'
      include 'src/Lib/wz/qcdcouple.h'
      integer mxpart
      parameter (mxpart=12)
      double precision msqB(-5:5,-5:5),msq(-5:5,-5:5)
      double precision p(mxpart,4),pTTTTTT(4,mxpart)
      real * 8 pin(0:3,nlegborn)
      integer vflav(nlegborn)
      real * 8 virtual
      real * 8 born,dummy(0:3,0:3)
      real *8 s,wz_dotp
      external wz_dotp
      integer i,j
      real * 8 wz_suppfact2e

      ason2pi = st_alpha/2d0/pi


      do i=1,nlegborn
         p(i,4) = pin(0,i)
         p(i,1:3) = pin(1:3,i)
      enddo
      p(1,:)=-p(1,:)
      p(2,:)=-p(2,:)



!$$      p(1,:)=(/   0.00000000000000d0,        0.00000000000000d0,  
!$$     .     -2.79463492129477d0,       -2.79463492129477d0/)     
!$$      p(2,:)=(/   0.00000000000000d0,        0.00000000000000d0,  
!$$     .      988.661720781338d0,       -988.661720781338d0/)     
!$$      p(3,:)=(/   8.23048171823666d0,       -2.30356108622719d0,  
!$$     .     -172.106980931630d0,        172.319064843077d0/)     
!$$      p(4,:)=(/  0.900050565513091d0,      -0.348892229878580d0,   
!$$     .    -40.2025373819361d0,        40.2141247418679d0/)     
!$$      p(5,:)=(/   19.8897202344351d0,       -1.91903780278773d0,   
!$$     .    -691.093906124847d0,        691.382723793407d0/)     
!$$      p(6,:)=(/  -29.0202525181849d0,        4.57149111889350d0,   
!$$     .    -82.4636614216307d0,        87.5404423242805d0/)      


      call wz_qqb_wz(p,msqB)
      call wz_qqb_wz_v(p,msq)

!$$      write(*,*)'BORN and VIRTUAAAAAALLLL'
!$$
!$$      do i=-5,5
!$$         do j=-5,5
!$$            if (msqB(i,j).ne.0d0) then
!$$            write(*,*)'msqB',i,j,msqB(i,j)
!$$         endif
!$$         enddo
!$$      enddo
!$$      do i=-5,5
!$$         do j=-5,5
!$$            if (msq(i,j).ne.0d0) then
!$$            write(*,*)'msq',i,j,msq(i,j)
!$$         endif
!$$         enddo
!$$      enddo
!$$
!$$      stop


      ! -- divide out ason2pi
      virtual = msq(vflav(1),vflav(2))/ason2pi
      born = msqB(vflav(1),vflav(2))

      ! -- scheme change from dred 
      virtual = virtual + born*(-2d0*(cf/2d0)) 

      virtual = virtual * wz_suppfact2e(pin,vflav)

      end

