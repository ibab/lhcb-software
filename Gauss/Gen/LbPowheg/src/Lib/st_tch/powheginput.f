      subroutine st_tch_wrtpowheginput(nlf)
      implicit none
      integer nlf
      character * 100 line
      character * 20 pwgprefix
      integer lprefix
      common/st_tch_cpwgprefix/pwgprefix,lprefix
      integer k,ios
      if(pwgprefix(1:lprefix).eq.'pwg') then
         open(unit=33,file='powheg.input',
     #     status='old',iostat=ios)
      else
         open(unit=33,file=pwgprefix(1:lprefix)//'powheg.input',
     #     status='old',iostat=ios)
      endif
      if(ios.ne.0) then
         write(*,*) ' cannot open st_tch_powheginput.dat'
         stop
      endif
 1    continue
      read(unit=33,fmt='(a)',iostat=ios,end=999) line
      if(ios.ne.0) then
         write(*,*) ' cannot read st_tch_powheginput.dat'
         stop
      endif
      k=100
 2    if(k.gt.0) then
         if(line(k:k).eq.' ') then
            k=k-1
            goto 2
         endif
      endif
      if(k.eq.0) k=1
      write(nlf,'(a)') line(1:k)
      goto 1
 999  end

      function st_tch_powheginput(stringa)
      implicit none
      real * 8 st_tch_powheginput
      character *(*) stringa
      integer maxnum
      parameter (maxnum=150)
      character * 100 line,line0
      character * 20 string
      character * 20 pwgprefix
      integer lprefix
      common/st_tch_cpwgprefix/pwgprefix,lprefix
      character * 20 keywords(maxnum)
      real * 8 st_tch_values(maxnum)
      logical used(maxnum)
      integer ios,numvalues,j,k,l,imode
      integer ini
      data ini/0/      
      save ini, keywords, st_tch_values, numvalues,used
      string=stringa
      if(ini.eq.0) then
         open(unit=33,file='powheg.input',status='old',iostat=ios)
         if(ios.ne.0) then
            write(*,*) ' Enter the prefix for this run < 20 characters'
            read(*,'(a)') pwgprefix
            do lprefix=20,1,-1
               if(pwgprefix(lprefix:lprefix).ne.' ') then
                  goto 11
               endif
            enddo
 11         continue
            lprefix=lprefix+1
            if(lprefix.gt.20) lprefix=20
            pwgprefix(lprefix:lprefix)='-'
            open(unit=33,file=pwgprefix(1:lprefix)//'powheg.input',
     #           status='old',iostat=ios)
            if(ios.ne.0) then            
               write(*,*) ' cannot open ',
     #              pwgprefix(1:lprefix)//'powheg.input'
               stop
            endif
         else
            pwgprefix='pwg'
            lprefix=3
         endif
         numvalues=0
         do l=1,1000000
            line0=' '
            read(unit=33,fmt='(a)',iostat=ios) line0
            if(ios.ne.0.and.line0.eq.' ') goto 10
            line=line0
            do k=1,100
               if(line(k:k).eq.'#'.or.line(k:k).eq.'!') then
                  line(k:)=' '
               endif
            enddo
            if(line.ne.' ') then
               if(numvalues.eq.maxnum) then
                  write(*,*) ' too many entries in st_tch_powheginput.dat'
                  call exit(-1)
               endif
               numvalues=numvalues+1
c skip blanks
 12            if(line(1:1).eq.' ') then
                  line=line(2:)
                  goto 12
               endif
               k=index(line,' ')
               keywords(numvalues)=line(1:k-1)
               line=line(k+1:)
               read(unit=line,fmt=*,iostat=ios) st_tch_values(numvalues)
               used(numvalues)=.false.
               if(ios.ne.0) then
                  write(*,*) ' st_tch_powheginput error: cannot parse '
                  write(*,'(a)') line0
                  stop
               endif
            endif
         enddo
 10      continue
         close(33)
         ini=1
      endif
      if(string(1:1).eq.'#') then
         string=string(2:)
         imode=0
      else
         imode=1
      endif
      do j=1,numvalues
         if(string.eq.keywords(j)) then
            st_tch_powheginput=st_tch_values(j)
            if(.not.used(j)) then
               used(j)=.true.
               write(*,*) ' st_tch_powheginput keyword ',keywords(j),
     1                    ' set to ',st_tch_values(j)
            endif
            return
         endif
      enddo
      if(imode.eq.1) then
         write(*,*) ' st_tch_powheginput: keyword ',string,' not found'
         call exit(-1)
      endif
c Not found; assign value -1d6; store the token anyhow
      if(numvalues.eq.maxnum) then
         write(*,*) ' too many entries in st_tch_powheginput.dat'
         write(*,*) ' increase maxnum in st_tch_powheginput.f'
         call exit(-1)
      endif
      numvalues=numvalues+1
      keywords(numvalues)=string
      st_tch_values(numvalues)=-1d6
      used(numvalues)=.true.
      st_tch_powheginput=-1d6
      write(*,*) ' st_tch_powheginput keyword ',keywords(j),
     1     ' absent; set to ',st_tch_values(j)
      end


c      implicit none
c      character * 10 string
c      integer j
c      real * 8 st_tch_powheginput,res
c      external st_tch_powheginput
c      string='QMASS'
c      res= st_tch_powheginput(string)
c      write(*,*) string,res
c      end
