      subroutine vbf_wp_wp_vtowpm_reset
      implicit none
      include 'src/Lib/vbf_wp_wp/tensorww.inc'
      complex*16 zero
      parameter (zero=(0d0,0d0))
      integer j,jj,mu,nu

      do mu = 0,3
	 do nu = 0,3
            do j = 1,3
               wppwpp(mu,nu,j) = zero
               wmmwmm(mu,nu,j) = zero
	       
	       do jj = 1,2
		  
     		  awen(mu,nu,jj,j) = zero
		  zwen(mu,nu,jj,j) = zero
     		  waen(mu,nu,jj,j) = zero
		  wzen(mu,nu,jj,j) = zero
     		  awmu(mu,nu,jj,j) = zero
		  zwmu(mu,nu,jj,j) = zero
     		  wamu(mu,nu,jj,j) = zero
		  wzmu(mu,nu,jj,j) = zero
		  
		enddo !jj

            enddo !j
         enddo !nu
      enddo !mu
            
      return
      end
