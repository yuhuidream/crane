/**************************************************************
 *
 *	lucdwt.c
 *
 *	Mersenne primality tester prototype source code.
 *
 *	Updates:
 *		20 May 97	RDW - fixed win32 compiler warning
 *		26 Apr 97	RDW - fixed tabs and unix EOL
 *		20 Apr 97 	RDW	
 * 		22 Oct 97 	REC	(Creation)
 *
 *	c. 1997 Perfectly Scientific, Inc.
 *	All Rights Reserved.
 *
 *	Compile and run with, e.g.:
 *
 *	% cc -O lucasDWT.c
 *	% a.out 216091 16384 100 1
 *
 **************************************************************/

/* Include Files */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "predefines.h"
#include "bbu_malloc.h"

/* definitions */
#define TWOPI (double)(2*3.1415926535897932384626433)
#define SQRTHALF (double)(0.707106781186547524400844362104)
#define SQRT2 (double)(1.414213562373095048801688724209)
#define BITS 16


/* compiler options */

#ifdef _WIN32
#pragma warning( disable : 4127 4706 ) /* disable conditional is constant warning */
#endif

/* global variables */

double		*cn, *sn, *two_to_phi, *two_to_minusphi, *scrambled;
double 		high,low,highinv,lowinv;
int 	 	b, c, *permute;


/* function prototypes */

void 	 	showusage(char **argv);

int get_radix_2_num(int N)
{
   double n = (double)N;
   int i = 0;
   while(n >= 2){
      n /= 2;
      i++;
   }
   if(n > 1)
      i++;
   return i;
}

/**************************************************************
 *
 *	Functions
 *
 **************************************************************/

/* rint is not ANSI compatible, so we need a definition for 
 * WIN32 and other platforms with rint.
 */
double
RINT(double x)
{
	return floor(x + 0.5);
}

#if 0
void
print(
	double 	*x,
	int  	N
)
{
	int  	printed = 0;

	while (N--)
	{
		if ((x[N]==0) && (!printed))
			continue;
		bbu_printf("%d  ",(int)(x[N]));
		printed=1;
	}
	bbu_printf("\n\r");
}
#endif

void
init_scramble_real(
	int  			n
)
{
	 int	i,j,k,halfn = n>>1;
	int  			tmp;

	for (i=0; i<n; ++i)
	{
		permute[i] = i;
	}
	for (i=0,j=0;i<n-1;i++)
	{
		if(i<j)
		{
			tmp = permute[i];
			permute[i] = permute[j];
			permute[j] = tmp;
		}
		k = halfn;
		while (k<=j)
		{
			j -= k;
			k>>=1;
		}
		j += k;
	}
}

void
init_fft(
	int		n
)
{

	int  	j;
	double 	e = TWOPI/n;

	cn = (double *)bbu_malloc(sizeof(double)*n);
	sn = (double *)bbu_malloc(sizeof(double)*n);
	for (j=0;j<n;j++)
	{
		cn[j] = cos(e*j);
		sn[j] = sin(e*j);
	}
	permute = (int *)bbu_malloc(n*sizeof(int));
	scrambled = (double *)bbu_malloc(n*sizeof(double));
	init_scramble_real(n);
}


void
fft_real_to_hermitian(
	double 				*z,
	int 	   			n
)
{
	/* Output is {Re(z^[0]),...,Re(z^[n/2),Im(z^[n/2-1]),...,Im(z^[1]).
	 * This is a decimation-in-time, split-radix algorithm.
	 */
	 int 		n4;
	 double 	*x;
	 double 	cc1, ss1, cc3, ss3;
	 int 		i1, i2, i3, i4, i5, i6, i7, i8, a, a3, dil;
	 double 	t1, t2, t3, t4, t5, t6;
	double 				e;
	int 	   			nn = n>>1, nminus = n-1, is, id;
	 int 		n2, n8, i, j;

	x = z-1;  /* FORTRAN compatibility. */
	is = 1;
	id = 4;
	do
	{
		for (i2=is;i2<=n;i2+=id)
		{
			i1 = i2+1;
			e = x[i2];
			x[i2] = e + x[i1];
			x[i1] = e - x[i1];
		}
		is = (id<<1)-1;
		id <<= 2;
	} while (is<n);

	n2 = 2;
	while (nn>>=1)
	{
		n2 <<= 1;
		n4 = n2>>2;
		n8 = n2>>3;
		is = 0;
		id = n2<<1;
		do
		{
			for (i=is;i<n;i+=id)
			{
				i1 = i+1;
				i2 = i1 + n4;
				i3 = i2 + n4;
				i4 = i3 + n4;
				t1 = x[i4]+x[i3];
				x[i4] -= x[i3];
				x[i3] = x[i1] - t1;
				x[i1] += t1;
				if (n4==1)
					continue;
				i1 += n8;
				i2 += n8;
				i3 += n8;
				i4 += n8;
				t1 = (x[i3]+x[i4])*SQRTHALF;
				t2 = (x[i3]-x[i4])*SQRTHALF;
				x[i4] = x[i2] - t1;
				x[i3] = -x[i2] - t1;
				x[i2] = x[i1] - t2;
				x[i1] += t2;
			}
			is = (id<<1) - n2;
			id <<= 2;
		} while (is<n);
		dil = n/n2;
		a = dil;
		for (j=2;j<=n8;j++)
		{
			a3 = (a+(a<<1))&(nminus);
			cc1 = cn[a];
			ss1 = sn[a];
			cc3 = cn[a3];
			ss3 = sn[a3];
			a = (a+dil)&(nminus);
			is = 0;
			id = n2<<1;
			do
			{
				for (i=is;i<n;i+=id)
				{
					i1 = i+j;
					i2 = i1 + n4;
					i3 = i2 + n4;
					i4 = i3 + n4;
					i5 = i + n4 - j + 2;
					i6 = i5 + n4;
					i7 = i6 + n4;
					i8 = i7 + n4;
					t1 = x[i3]*cc1 + x[i7]*ss1;
					t2 = x[i7]*cc1 - x[i3]*ss1;
					t3 = x[i4]*cc3 + x[i8]*ss3;
					t4 = x[i8]*cc3 - x[i4]*ss3;
					t5 = t1 + t3;
					t6 = t2 + t4;
					t3 = t1 - t3;
					t4 = t2 - t4;
					t2 = x[i6] + t6;
					x[i3] = t6 - x[i6];
					x[i8] = t2;
					t2 = x[i2] - t3;
					x[i7] = -x[i2] - t3;
					x[i4] = t2;
					t1 = x[i1] + t5;
					x[i6] = x[i1] - t5;
					x[i1] = t1;
					t1 = x[i5] + t4;
					x[i5] -= t4;
					x[i2] = t1;
				}
				is = (id<<1) - n2;
				id <<= 2;
			} while (is<n);
		}
	}
}


void
fftinv_hermitian_to_real(
	double 				*z,
	int 	   			n
)
{
	/* Input is {Re(z^[0]),...,Re(z^[n/2),Im(z^[n/2-1]),...,Im(z^[1]).
	 * This is a decimation-in-frequency, split-radix algorithm.
	 */
	 int 		n4;
	 double 	cc1, ss1, cc3, ss3;
	 double 	t1, t2, t3, t4, t5;
	 double 	*x;
	 int		n8, i1, i2, i3, i4, i5, i6, i7, i8,
			   			a, a3, dil;
	double 				e;
	int 	   			nn = n>>1, nminus = n-1, is, id;
	int 	   			n2, i, j;

	x = z-1;
	n2 = n<<1;
	while(nn >>= 1)
	{
		is = 0;
		id = n2;
		n2 >>= 1;
		n4 = n2>>2;
		n8 = n4>>1;
		do
		{
			for (i=is;i<n;i+=id)
			{
				i1 = i+1;
				i2 = i1 + n4;
				i3 = i2 + n4;
				i4 = i3 + n4;
				t1 = x[i1] - x[i3];
				x[i1] += x[i3];
				x[i2] += x[i2];
				x[i3] = t1 - x[i4] - x[i4];
				x[i4] = t1 + x[i4] + x[i4];
				if (n4==1)
					continue;
				i1 += n8;
				i2 += n8;
				i3 += n8;
				i4 += n8;
				t1 = x[i2]-x[i1];
				t2 = x[i4]+x[i3];
				x[i1] += x[i2];
				x[i2] = x[i4]-x[i3];
				x[i3] = -SQRT2*(t2+t1);
				x[i4] = SQRT2*(t1-t2);
			}
			is = (id<<1) - n2;
			id <<= 2;
		} while (is<nminus);
		dil = n/n2;
		a = dil;
		for (j=2;j<=n8;j++)
		{
			a3 = (a+(a<<1))&(nminus);
			cc1 = cn[a];
			ss1 = sn[a];
			cc3 = cn[a3];
			ss3 = sn[a3];
			a = (a+dil)&(nminus);
			is = 0;
			id = n2<<1;
			do
			{
				for (i=is;i<n;i+=id)
				{
					i1 = i+j;
					i2 = i1+n4;
					i3 = i2+n4;
					i4 = i3+n4;
					i5 = i+n4-j+2;
					i6 = i5+n4;
					i7 = i6+n4;
					i8 = i7+n4;
					t1 = x[i1] - x[i6];
					x[i1] += x[i6];
					t2 = x[i5] - x[i2];
					x[i5] += x[i2];
					t3 = x[i8] + x[i3];
					x[i6] = x[i8] - x[i3];
					t4 = x[i4] + x[i7];
					x[i2] = x[i4] - x[i7];
					t5 = t1 - t4;
					t1 += t4;
					t4 = t2 - t3;
					t2 += t3;
					x[i3] = t5*cc1 + t4*ss1;
					x[i7] = -t4*cc1 + t5*ss1;
					x[i4] = t1*cc3 - t2*ss3;
					x[i8] = t2*cc3 + t1*ss3;
				}
				is = (id<<1) - n2;
				id <<= 2;
			} while (is<nminus);
		}
	}
	is = 1;
	id = 4;
	do
	{
		for (i2=is;i2<=n;i2+=id)
		{
			i1 = i2+1;
			e = x[i2];
			x[i2] = e + x[i1];
			x[i1] = e - x[i1];
		}
		is = (id<<1) - 1;
		id <<= 2;
	} while (is<n);
	e = 1/(double)n;
	for (i=0;i<n;i++)
	{
		z[i] *= e;
	}
}


void
square_hermitian(
	double 				*b,
	int 	   			n
)
{
	 int 		k, half = n>>1;
	 double 	c, d;

	b[0] *= b[0];
	b[half] *= b[half];
	for (k=1;k<half;k++)
	{
		c = b[k];
		d = b[n-k];
		b[n-k] = 2.0*c*d;   //im
		b[k] = (c+d)*(c-d); //re
	}
}


void
squareg(
	double 	*x,
	int 	size
)
{
	fft_real_to_hermitian(x, size);
	square_hermitian(x, size);
	fftinv_hermitian_to_real(x, size);
}



/* ------------ Lucas Test - specific routines ------------------- */

void
init_lucas(
	int 	q,
	int 	N
)
{
	int 	j,qn,a,len;
	double 	log2 = log(2.0);

	len = N*sizeof(double);
	two_to_phi = (double *)bbu_malloc(len);
	two_to_minusphi = (double *)bbu_malloc(len);

	low = RINT(exp(floor((double)q/N)*log2));
	high = low+low;
	lowinv = 1.0/low;
	highinv = 1.0/high;
	b = q & (N-1);
	c = N-b;

	two_to_phi[0] = 1.0;
	two_to_minusphi[0] = 1.0;
	qn = q&(N-1);

	for(j=1; j<N; ++j)
	{
		a = N - (( j*qn)&(N-1));
		two_to_phi[j] = exp(a*log2/N);
		two_to_minusphi[j] = 1.0/two_to_phi[j];
	}
}


double addsignal(
	double 				*x,
	int 				N, 
	int 				error_log
)
{
   	 int	 	k,j,bj,bk,sign_flip,NminusOne = N-1;
   	 double 	zz,w, *xptr = x, *xxptr;
   	 double 	hi = high, lo = low, hiinv = highinv, loinv = lowinv;
   	double 				err, maxerr = 0.0;

   	bk  =  0;
  	for (k=0; k<N; ++k) 
  	{
    	if ((zz=*xptr)<0) 
    	{
       		zz = floor(0.5 - zz);
       		sign_flip = 1;
       	}
     	else 
     	{
       		zz = floor(zz+0.5);
       		sign_flip = 0;
       	}
     	if (error_log ) 
     	{
     		if (sign_flip) 
     			err = fabs(zz + *xptr );  
     		else 
     			err = fabs(zz  - *xptr);
	 		if (err > maxerr) 
	 			maxerr = err;
     	}
     	*xptr = 0;
     	j = k;
     	bj = bk;
     	xxptr = xptr++;
     	do 
     	{
         	if (j==N) 
         		j=0;
	  		if (j==0)
	  		{
	  			xxptr = x; 
	  			bj = 0;  
	  			w = floor(zz*hiinv);
	  	    	if (sign_flip) 
	  	    		*xxptr -= (zz-w*hi); 
	  	    	else 
	  	    		*xxptr += (zz-w*hi); 
	  	    }
	  		else if (j==NminusOne) 
	  		{ 
	  			w = floor(zz*loinv);
	            if (sign_flip) 
	            	*xxptr -= (zz-w*lo); 
	            else 
	            	*xxptr += (zz-w*lo); 
	        }
	    	else if (bj >= c) 
	    	{ 
	    		w = floor(zz*hiinv);
	    	    if (sign_flip) 
	    	    	*xxptr -= (zz-w*hi); 
	    	    else 
	    	    	*xxptr += (zz-w*hi); 
	    	}
	       	else 
	       	{ 
	       		w = floor(zz*loinv);
	            if (sign_flip) 
	            	*xxptr -= (zz-w*lo); 
	            else 
	            	*xxptr += (zz-w*lo); 
	        }
          	zz = w;
          	++j;
	  		++xxptr;
          	bj += b; 
          	if (bj>=N) 
          		bj -= N;
         }  while(zz!=0.0);
     
     	bk += b; 
     	if (bk>=N) 
     		bk -= N;
    }
	return(maxerr);
}


void
patch(
	double 				*x,
	int 				N
)
{
	 int 		j,bj,NminusOne = N-1, carry;
	 double 	hi = high, lo = low, highliminv, lowliminv;
	 double 	*px = x, xx;
	double  			highlim,lowlim, lim, inv, base;

	carry = 0;
	highlim = hi*0.5;
	lowlim = lo*0.5;
	highliminv =1.0/highlim;
	lowliminv = 1.0/lowlim;

	xx = *px + carry;
	if (xx >= highlim)
		carry =((int)(xx*highliminv+1))>>1;
	else if (xx<-highlim)
		carry = -(((int)(1-xx*highliminv))>>1);
	else
		carry = 0;

	*(px++) = xx - carry*hi;
	bj = b;
	for (j=1; j<NminusOne; ++j)
	{
		xx = *px + carry;
		if ((bj & NminusOne) >= c)
		{
			if (xx >= highlim)
				carry =((int)(xx*highliminv+1))>>1;
			else if (xx<-highlim)
				carry = -(((int)(1-xx*highliminv))>>1);
			else
				carry = 0;

			*px = xx - carry*hi;
		}
		else
		{
			if (xx >= lowlim)
				carry =((int)(xx*lowliminv+1))>>1;
			else if (xx<-lowlim)
				carry = -(((int)(1-xx*lowliminv))>>1);
			else
				carry = 0;

			*px = xx - carry*lo;
		}
		++px;
		bj += b;
	}

	xx = *px + carry;
	if (xx >= lowlim)
		carry = ((int)(xx*lowliminv+1))>>1;
	else if (xx<-lowlim)
		carry = -(((int)(1-xx*lowliminv))>>1);
	else
		carry = 0;

	*px = xx - carry*lo;
	if (carry)
	{
		j = 0;
		bj = 0;
		px = x;
		while (carry)
		{
			xx = *px + carry;
			if (j==0)
			{
				lim = highlim;
				inv = highliminv;
				base = hi;
			}
			else if (j==NminusOne)
			{
				lim = lowlim;
				inv = lowliminv;
				base = lo;
			}
			else if ((bj & NminusOne) >= c)
			{
				lim = highlim;
				inv = highliminv;
				base = hi;
			}
			else
			{
				lim = lowlim;
				inv = lowliminv;
				base = lo;
			}

			if (xx>=lim)
				carry = ((int)(xx*inv+1))>>1;
			else if (xx<-lim)
				carry = -(((int)(1-xx*inv))>>1);
			else
				carry = 0;

			*(px++) = xx - carry*base;
			bj += b;
			if (++j == N)
			{
				j = 0;
				bj = 0;
				px = x;
			}
		}
	}
}


void
check_balanced(
	double 	*x,
	int 	N
)
{
	int 	j,bj = 0,NminusOne = N-1;
	double 	limit, hilim,lolim,*ptrx = x;

	hilim = high*0.5;
	lolim = low*0.5;
	for (j=0; j<N; ++j)
	{
		if (j==0)
			limit = hilim;
		else if (j==NminusOne)
			limit = lolim;
		else if ((bj & NminusOne) >= c)
			limit = hilim;
		else
			limit = lolim;

		assert ((*ptrx<=limit) && (*ptrx>=-limit));
		++ptrx;
		bj+=b;
	}
}


double
lucas_square(
	double				*x,
	int 				N,
	int 				error_log
)
{
	 int 		j, *perm;
	 double 	err, *ptrx, *ptry, *ptrmphi;
	perm = permute;
	ptry = scrambled;

	for (j=0; j<N; ++j, perm++)
	{
		*(ptry++) = x[*perm] * two_to_phi[*perm];
	}

	squareg(scrambled,N);
	perm = permute;
	ptrx = x;
	ptrmphi = two_to_minusphi;

	for (j=0; j<N; ++j)
	{
		*(ptrx++) = scrambled[*(perm++)] *  *(ptrmphi++);
	}
	err = addsignal(x,N, error_log);
	patch(x,N);
	if (error_log){
		check_balanced(x,N);
	}

	return(err);
}


int
isszero(
	double 				*x,
	int 				N
)
{
	 int 		j;
	 double 	*xp = x;

	for(j=0; j<N; ++j)
	{
		if (RINT(*(xp++)))
			return 0;
	}
	return 1;
}


void balancedtostdrep(
	double 	*x, 
	int 	N
)
{
 	int 	sudden_death = 0, j = 0, NminusOne = N-1, bj = 0;

  	while(1) 
  	{
		if (x[j] < 0) 
	 	{
			--x[(j+1) & NminusOne];
			if (j==0) 
				x[j]+=high;
        	else if (j==NminusOne) 
        		x[j]+=low;
	  		else if ((bj & NminusOne) >=c) 
	  			x[j]+=high;
	    	else 
	    		x[j]+=low;
		}
	 	else if (sudden_death) 
	 		break;
	 	bj+=b;
	 	if (++j==N) 
	 	{
			sudden_death = 1;
			j = 0;
			bj = 0;
		}
	 }
 }


void
printbits(
	double	*x,
	int  	q,
	int  	N,
	int  	totalbits
)
{
	char 	*bits = (char *) bbu_malloc((int) totalbits);
	int  	j, k, i, word;

	j = 0;
	i = 0;
	do
	{
		k = (int)( ceil((double)q*(j+1)/N) - ceil((double)q*j/N));
		if (k>totalbits)
			k = totalbits;
		totalbits -= k;
		word = (int)x[j++];
		while(k--)
		{
			bits[i++] = (char)('0' + (word & 1));
			word>>=1;
		}
	} while (totalbits);
	char *tmp = (char *) bbu_malloc(i+1);
	j = 0;
	while (i--)
	{
		bbu_sprintf((tmp+j++), "%c",bits[i]);
	}
	tmp[j] = 0;
	bbu_printf("%s\n\r", tmp);
	bbu_mfree(bits);
	bbu_mfree(tmp);
}


void
showusage(
	char 		**argv
)
{
	bbu_printf("\n\r"
	           "Usage: %s <q> <N> [n] [err]\n\r", (char *)argv[0]);
	bbu_printf("   q = Mersenne exponent\n\r");
	bbu_printf("   N = fft run-length\n\r");
	bbu_printf("   n = Number of Lucas iterations (or 0 for full test)\n\r");
	bbu_printf("   err = 1 to report maximum errors\n\r");
	bbu_printf("e.g.: %s 4253 1024 0 1\n\r", (char *)argv[0]);
	bbu_printf("Note: If the final result is 0, then 2^q-1 is a Mersenne prime\n\r");
}



/**************************************************************
 *
 *	Main Function
 *
 **************************************************************/

void
lucas_main(
	int 		argc,
	char 		**argv
)
{
	int 		q, n, j;
	double 		*x, err;
	int 		last, errflag=0;

	if (argc<3)
	{
		showusage(argv);
		return;
	}
	q = (int)atol(argv[1]);
	last = q-1;
	n = (int)atol(argv[2]);
	if (argc>3)
	{
		if (atol(argv[3]) > 0)
			last = (int)atol(argv[3]);
		if (argc>4)
			errflag = (int)atol(argv[4]);
	}

	x = (double *)bbu_malloc(n*sizeof(double));
	init_fft(n);
	init_lucas(q,n);

	for (j=0;j<n;j++)
	{
		x[j]=0;
	}
	x[0] = 4.0;

	for(j=1;j<last;j++)
	{
		err = lucas_square(x,n,errflag);
		if (errflag)
		{
			bbu_printf("%ld maxerr: %f\n\r",j,err);
			//fflush(stdout);
		}else{
		   if(j%100 == 0)
		      bbu_putchar('.'); //print a point indicate the test is ongoing.
		}
		x[0] -= 2.0;
	}
	bbu_printf("\n\r");

	bbu_printf("%d  ",q);
	if (isszero(x,n))
	{
		bbu_printf("0\n\r");
	}
	else
	{
		balancedtostdrep(x,n);
		printbits(x,q,n,64);
		bbu_printf("\n\r");
	}
	bbu_mfree((void *)x);
	bbu_mfree((void *)cn);
   bbu_mfree((void *)sn);
   bbu_mfree((void *)permute);
   bbu_mfree((void *)scrambled);
   bbu_mfree((void *)two_to_phi);
   bbu_mfree((void *)two_to_minusphi);	
}
