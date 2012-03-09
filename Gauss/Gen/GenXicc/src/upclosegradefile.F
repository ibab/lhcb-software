
c...close grade files

c...here: kmix-->imix; kmixtype-->imixtype; kveggrade-->iveggrade

	subroutine upclosegradefile(kveggrade)
      implicit double precision(a-h, o-z)
	implicit integer(i-n) 

	close(11)
	if(kveggrade.eq.1) close(29)

	return
	end

