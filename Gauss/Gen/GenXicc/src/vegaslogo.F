
c********************
c***   vegas logo:
c********************

      subroutine vegaslogo()
      implicit double precision(a-h, o-z)
	implicit integer(i-n)

      common/vegasinf/number,nitmx
	common/vegasbin/nvbin
	common/subopen/subfactor,subenergy,isubonly,ichange,iconsbarnum

       write(*,'(a,i5)') 'the used bin number in vegas:', nvbin
       !write(3,'(a,i5)') 'the used bin number in vegas:', nvbin
	 write(*,'(a,i9,a,i3)') 'using vegas: number in each iter.='
     &	   ,number,'  iter.=',nitmx
   	 !write(3,'(a,i9,a,i3)') 'using vegas: number in each iter.='
c     &	   ,number,'  iter.=',nitmx
       write(*,*)
       !write(3,*)
	 write(*,'(a)') 
     &	   '......................................................'
	 write(*,'(a)') 
     &	   '.............. end of initialization .................'
	 write(*,'(a)') 
     &	   '......................................................'
	 !write(3,'(a)') 
c     &	   '......................................................'
	 !write(3,'(a)') 
c     &	   '.............. end of initialization .................'
	 !write(3,'(a)') 
c     &	   '......................................................'

	 if(isubonly.eq.0) then
	  write(*,*)
	  write(*,'(a)') '......getting the hadronic information......'
	  write(*,*)
	  !write(3,'(a)') '......getting the hadronic information......'
	 else
	  write(*,*)
	  write(*,'(a,a,1x,g12.5)') 'getting subprocess info....',
     &	 'at c.m. energy(gev)',subenergy
	  !write(3,'(a,a,1x,g12.5)') 'getting subprocess info....',
c     &	 'at c.m. energy(gev)',subenergy
	  write(*,'(a)') 'hadron collider information is no use !!!'
	  write(*,*)
	  !write(3,'(a)') 'hadron collider information is no use !!!'
	 end if

       write(*,*)
       !write(3,*)
	 write(*,'(a)') '.....waiting......  vegas running  ......'
	 !write(3,'(a)') '.....waiting......  vegas running  ......'
       write(*,*)
       !write(3,*)

	return
	end

c*******************************************************************

c*** vegas close statements:

      subroutine vegasend(vegasopen,ievntdis,usegrade)
      implicit double precision(a-h, o-z)
	implicit integer(i-n)
	logical vegasopen,usegrade

      if(vegasopen) then
	 write(*,'(a)') '...... end of vegas, grade generated ......'
	 !write(3,'(a)') '...... end of vegas, grade generated ......'
	 if(ievntdis.eq.1) then
	     write(*,'(a)')'ok, to get the evnt number distributions'
	     !write(3,'(a)')'ok, to get the evnt number distributions'
       else
	     write(*,'(a)')'ok, to get the differential distributions'
	     !write(3,'(a)')'ok, to get the differential distributions'
	 end if
       write(*,*)
       !write(3,*)
      else
	 write(*,'(a)') '......    vegas has not been used     ......'
	 !write(3,'(a)') '......    vegas has not been used     ......'
       if(usegrade) then
         write(*,'(a)') 'using existed grade to generate events.....'
         !write(3,'(a)') 'using existed grade to generate events.....'
	   write(*,'(a)')'(existed) grade has not been further improved'
	   !write(3,'(a)')'(existed) grade has not been further improved'
       else
         write(*,'(a)') 'no grade is adopted, using trivial MC.'
         !write(3,'(a)') 'no grade is adopted, using trivial MC.'
         if(imix.eq.1) then
           write(*,'(a)') 'not an appreciable way for mixed events'
           !write(3,'(a)') 'not an appreciable way for mixed events'
           write(*,'(a)') 'the precision is lower than other methods'
           !write(3,'(a)') 'the precision is lower than other methods'
         end if
       end if
       write(*,*)
       !write(3,*)
       write(*,'(a)')
     &     '......................................................'
       write(*,'(a)')
     &     '.............. end of initialization .................'
       write(*,'(a)')
     &     '......................................................'
       !write(3,'(a)')
c     &     '......................................................'
       !write(3,'(a)')
c     &     '.............. end of initialization .................'
       !write(3,'(a)')
c     &     '......................................................'
       write(*,*)
       !write(3,*)
	 if(ievntdis.eq.1) then
	  write(*,'(a)')'ok, to get the evnt number distributions.....'
	  !write(3,'(a)')'ok, to get the evnt number distributions.....'
       else
	  write(*,'(a)')'ok, to get the differential distributions....'
	  !write(3,'(a)')'ok, to get the differential distributions....'
	 end if
	end if

      return
	end

