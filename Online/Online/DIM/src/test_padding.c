/*
#define DIMLIB
#include <dim.h>
#include <dic.h>
#include <dis.h>

#ifdef VMS
#	include <cvtdef.h>
#endif
*/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//#ifdef WIN32
//#define unix
//#endif

#if defined(WIN32) || defined(unix)
#define PADD64
#endif

#if defined(aix) || defined (LYNXOS)
#undef PADD64
#endif

#if defined(linux) && !defined (__LP64__)
#undef PADD64
#endif

typedef enum { NOSWAP, SWAPS, SWAPL, SWAPD} SWAP_TYPE;

typedef struct{
	int par_num;
	short par_bytes;
	short flags;     /* bits 0-1 is type of swap, bit 4 id float conversion */
}FORMAT_STR;

#define VAX_FLOAT		0x10
#define IEEE_FLOAT 		0x20
#define AXP_FLOAT		0x30

#define IT_IS_FLOAT		0x1000

#define SIZEOF_CHAR 1
#define SIZEOF_SHORT 2
#define SIZEOF_LONG 4
#define SIZEOF_FLOAT 4
#define SIZEOF_DOUBLE 8

#if defined(OSK) && !defined(_UCC)
#	define inc_pter(p,i) (char *)p += (i)
#else
#	define inc_pter(p,i) p = (void *)((char *)p + (i))
#endif

/*
 *  (Delphi Network Access) implements the network layer for the DIM
 * (Delphi Information Managment) System.
 *
 * Started date   : 10-11-91
 * Written by     : C. Gaspar
 * UNIX adjustment: G.C. Ballintijn
 *
 */

double _swapd( d )
double d;
{
	double	r[2];
	register char	*p, *q;
	register int 	n;

	p = (char *) &r[1];
	q = (char *) &d;
	for( n = sizeof(double)+1; --n; *--p = *q++) ;
	return r[0];
}

int _swapl( l )
int l;
{
	int	r[2];
	register char	*p, *q;

	p = (char *) &r[1];
	q = (char *) &l;
	*--p = *q++;
	*--p = *q++;
	*--p = *q++;
	*--p = *q++;

	return r[0];
}

short _swaps( s )
short s;
{
	short	r[2];
	register char	*p, *q;

	p = (char *) &r[1];
	q = (char *) &s;
	*--p = *q++;
	*--p = *q++;

	return r[0];
}

double _swapd_by_addr( d )
double *d;
{
	double	r[2];
	register char	*p, *q;
	register int 	n;

	p = (char *) &r[1];
	q = (char *) d;
	for( n = sizeof(double)+1; --n; *--p = *q++) ;

	return r[0];
}

int _swapl_by_addr( l )
int *l;
{
	int	r[2];
	register char	*p, *q;

	p = (char *) &r[1];
	q = (char *) l;
	*--p = *q++;
	*--p = *q++;
	*--p = *q++;
	*--p = *q++;

	return r[0];
}

short _swaps_by_addr( s )
short *s;
{
	short	r[2];
	register char	*p, *q;

	p = (char *) &r[1];
	q = (char *) s;
	*--p = *q++;
	*--p = *q++;

	return r[0];
}

void _swaps_buffer( s2, s1, n)
short *s1, *s2;
register int n;
{
	register char *p, *q;
	short r[2];
	register short *s;

	p = (char *) s2;
	q = (char *) s1;
	if( p != q ) {
		p += sizeof(short);
		for( n++; --n; p += 2*sizeof(short)) {
			*--p = *q++;
			*--p = *q++;
		}
	} else {
		for( s = s2, n++; --n; *s++ = r[0]) {
			p = (char *) &r[1] ;
			*--p = *q++;
			*--p = *q++;
		}
	}
}

void _swapl_buffer( s2, s1, n)
int *s1, *s2;
register int n;
{
	register char *p, *q;
	int r[2];
	register int *l;

	p = (char *) s2;
	q = (char *) s1;
	if( p != q ) {
		p += sizeof(int);
		for( n++; --n; p += 2*sizeof(int)) {
			*--p = *q++;
			*--p = *q++;
			*--p = *q++;
			*--p = *q++;
		}
	} else {
		for( l = s2, n++; --n; *l++ = r[0]) {
			p = (char *) &r[1] ;
			*--p = *q++;
			*--p = *q++;
			*--p = *q++;
			*--p = *q++;
		}
	}
}


void _swapd_buffer( s2, s1, n)
double *s1, *s2;
register int n;
{
	register char *p, *q;
	double r[2];
	register double *d;
	register int m;

	p = (char *) s2;
	q = (char *) s1;
	if( p != q ) {
		p += sizeof(double);
		for( n++; --n; p += 2*sizeof(double)) {
			for( m = sizeof(double)+1; --m; *--p = *q++) ;
		}
	} else {
		for( d = s2, n++; --n; *d++ = r[0]) {
			p = (char *) &r[1] ;
			for( m = sizeof(double)+1; --m; *--p = *q++) ;
		}
	}
}


static int get_format_data(format_data, def)
register FORMAT_STR *format_data;
register char *def;
{
	register int index = 0;
	register char code, last_code = 0;
	int num;

	code = *def;
	while(*def)
	{
		if(code != last_code)
		{
			format_data->par_num = 0;
			format_data->flags = 0;
			switch(code)
			{
				case 'i':
				case 'I':
				case 'l':
				case 'L':
					format_data->par_bytes = SIZEOF_LONG;
					format_data->flags |= SWAPL;
					break;
				case 'x':
				case 'X':
					format_data->par_bytes = SIZEOF_DOUBLE;
					format_data->flags |= SWAPD;
					break;
				case 's':
				case 'S':
					format_data->par_bytes = SIZEOF_SHORT;
					format_data->flags |= SWAPS;
					break;
				case 'f':
				case 'F':
					format_data->par_bytes = SIZEOF_LONG;
					format_data->flags |= SWAPL;
#ifdef vms      	
					format_data->flags |= IT_IS_FLOAT;
#endif
					break;
				case 'd':
				case 'D':
					format_data->par_bytes = SIZEOF_DOUBLE;
					format_data->flags |= SWAPD;
#ifdef vms
					format_data->flags |= IT_IS_FLOAT;
#endif
					break;
				case 'c':
				case 'C':
					format_data->par_bytes = SIZEOF_CHAR;
					format_data->flags |= NOSWAP;
					break;
			}
		}
		def++;
		if(*def != ':')
		{
			if(*def)
			{
/*
				printf("Bad service definition parsing\n");
				fflush(stdout);

				error_handler("Bad service definition parsing",2);
*/
				return(0);
			}
			else
				format_data->par_num = 0;
		}
		else
		{
			def++;
			sscanf(def,"%d",&num);
			format_data->par_num += num;
			while((*def != ';') && (*def != '\0'))
				def++;
			if(*def)
				def++;
		}
		last_code = code;
		code = *def;
		if(code != last_code)
			format_data++;
	}
	format_data->par_bytes = 0;
	return(1);
}



static int Dic_padding = 1;
static int Dis_padding = 1;

void dic_disable_padding()
{
	Dic_padding = 0;
}

void dis_disable_padding()
{
	Dis_padding = 0;
}

static int get_curr_bytes(items, bytes_left, item_size)
int items, bytes_left, item_size;
{
	int num;

	if(!(num = items))
	{
		num = bytes_left;
	} 
	else 
	{
		num *= item_size;
	}
	return num;
}

#ifdef vms
		
static int check_vms_out(flags, format, curr_par_num, buff_out)
short flags;
int format, curr_par_num;
void *buff_out;
{
	unsigned int input_code;
	int i;
	int num;
	
	if(	(flags & IT_IS_FLOAT) && ((format & 0xF0) == IEEE_FLOAT) )
	{
		switch(flags & 0x3)
		{
			case SWAPL :
				num = curr_par_num;
				(int *)buff_out -= num;
				for( i = 0; i < num; i++) 
				{
					cvt$convert_float((void *)buff_out, CVT$K_VAX_F, 
									(void *)buff_out, CVT$K_IEEE_S,
									0 );
					((int *)buff_out)++;
				}
				break;
			case SWAPD :
#ifdef __alpha
				input_code = CVT$K_VAX_G;
#else
				input_code = CVT$K_VAX_D;
#endif
				num = curr_par_num;
				(double *)buff_out -= num;
				for( i = 0; i < num; i++ )
				{
					cvt$convert_float((void *)buff_out, input_code,
									(void *)buff_out, CVT$K_IEEE_T,
									0 );
					((double *)buff_out)++;
				}
				break;
		}
	}
}



static int check_vms_in(flags, curr_par_num, curr_par_bytes, buff_out)
short flags;
int curr_par_num, curr_par_bytes;
void *buff_out;
{
	unsigned int input_code, output_code;
	int i;
	int num;
	
	if(flags & 0xF0)
	{
		switch(curr_par_bytes) 
		{
			case SIZEOF_FLOAT :
				if((flags & 0xF0) == IEEE_FLOAT)
				{
					num = curr_par_num;
					(int *)buff_out -= num;
					for( i = 0; i<num; i++)
					{
						cvt$convert_float((void *)buff_out, CVT$K_IEEE_S,
										  (void *)buff_out, CVT$K_VAX_F,
										  0 );
						((int *)buff_out)++;
					}
				}
				break;
			case SIZEOF_DOUBLE :
#ifdef __alpha
				output_code = CVT$K_VAX_G;
#else
				output_code = CVT$K_VAX_D;
#endif
				switch(flags & 0xF0)
				{
					case VAX_FLOAT:
						input_code = CVT$K_VAX_D;
						break;	
					case AXP_FLOAT:
						input_code = CVT$K_VAX_G;
						break;	
					case IEEE_FLOAT:
						input_code = CVT$K_IEEE_T;
						break;	
				}							
				num = curr_par_num;
				(double *)buff_out -= num;
				for( i = 0; i<num; i++)
				{
					cvt$convert_float((void *)buff_out, input_code,
									  (void *)buff_out, output_code,
									  0 );
					((double *)buff_out)++;
				}
				break;
		}
	}
}

#endif

static int check_padding(curr_bytes, item_size)
int curr_bytes, item_size;
{
	int num;

	if(num = curr_bytes % item_size)
	{
		num = item_size - num;
	}
	return num;
}

int copy_swap_buffer_out(format, format_data, buff_out, buff_in, size)
int format;
register int size;
register FORMAT_STR *format_data;
register void *buff_out, *buff_in;
{
	int num, pad_num, curr_size = 0, curr_out = 0;
	int next_par_bytes, curr_par_num;
	
	if(!format_data->par_bytes) {
		if(buff_in != buff_out)
			memcpy( buff_out, buff_in, size );
		return(size);
	}
	next_par_bytes = format_data->par_bytes;
	while(next_par_bytes)
	{
		curr_par_num = format_data->par_num;
		if((curr_size+(curr_par_num * format_data->par_bytes))
		   > size)
		{
			curr_par_num = (size - curr_size)/format_data->par_bytes;
			next_par_bytes = 0;
		}
		switch(format_data->flags & 0x3) 
		{
			case NOSWAP :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_CHAR);

				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPS :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_SHORT);

				if(Dis_padding)
				{
					if(pad_num = check_padding(curr_size, SIZEOF_SHORT))
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPL :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_LONG);

				if(Dis_padding)
				{
					if(pad_num = check_padding(curr_size, SIZEOF_LONG))
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPD :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_DOUBLE);

				if(Dis_padding)
				{
#ifdef PADD64
					if(pad_num = check_padding(curr_size, SIZEOF_DOUBLE))
#else
					if(pad_num = check_padding(curr_size, SIZEOF_LONG))
#endif
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
		}
#ifdef vms
		check_vms_out(format_data->flags, format, curr_par_num, buff_out);
#endif
		curr_size += num;
		format_data++;
		if(next_par_bytes)
			next_par_bytes = format_data->par_bytes;
	}
	return(curr_out);
}

/*
int copy_swap_buffer_out(format, format_data, buff_out, buff_in, size)
int format;
register int size;
register FORMAT_STR *format_data;
register void *buff_out, *buff_in;
{
	register int num, old_par_num, curr_size = 0, curr_out = 0;
	int res;
#ifdef vms
	unsigned int input_code;
	int i;
#endif
	char *ptrc = (char *)buff_out;
	
	if(!format_data->par_bytes) {
		if(buff_in != buff_out)
			memcpy( buff_out, buff_in, size );
		return(size);
	}
	for( ; format_data->par_bytes; format_data++) 
	{
		old_par_num = format_data->par_num;
		if((curr_size+(format_data->par_num * format_data->par_bytes))
		   > size)
		{
			format_data->par_num = (size - curr_size)/format_data->par_bytes;
			(format_data+1)->par_bytes = 0;
		}
		switch(format_data->flags & 0x3) {
			case NOSWAP :
				get_num_items(
				if(!(num = format_data->par_num))
					format_data->par_num = num = size - curr_size;
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
if(Dis_padding)
{
#ifdef unix
				if((format_data + 1)->par_bytes > SIZEOF_SHORT)
				{
				if(res = (curr_size+num) % SIZEOF_LONG)
				{
					res = SIZEOF_LONG - res;
					inc_pter( buff_in, res);
					curr_size += res;
				}
				}
				else if((format_data + 1)->par_bytes == SIZEOF_SHORT)
				if(res = (curr_size+num) % SIZEOF_SHORT) 
				{
					res = SIZEOF_SHORT - res;
					inc_pter( buff_in, res);
					curr_size += res;
				}
#endif
}
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPS :
				if(!(num = format_data->par_num))
				{
					num = size - curr_size;
					format_data->par_num = num/SIZEOF_SHORT;
				} 
				else 
				{
					num *= SIZEOF_SHORT;
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
if(Dis_padding)
{
#ifdef unix
				if((format_data + 1)->par_bytes > SIZEOF_SHORT)
				{
				if(res = (curr_size+num) % SIZEOF_LONG)
				{
					res = SIZEOF_LONG - res;
					inc_pter( buff_in, res);
					curr_size += res;
				}
				}
#endif
}
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPL :
				if(!(num = format_data->par_num))
				{
					num = size - curr_size;
					format_data->par_num = num/SIZEOF_LONG;
				} 
				else 
					num *= SIZEOF_LONG;
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPD :
				if(!(num = format_data->par_num))
				{
					num = size - curr_size;
					format_data->par_num = num/SIZEOF_DOUBLE;
				} 
				else
					num *= SIZEOF_DOUBLE;
if(Dis_padding)
{
#ifdef unix
#ifndef aix
#ifndef linux
#ifndef LYNXOS
				if(res = (curr_size + num)%SIZEOF_DOUBLE)
				{
					res = SIZEOF_DOUBLE - res;
					inc_pter( buff_in, res);
					curr_size += res;
				}
#endif
#endif
#endif
#endif
}
				memcpy( buff_out, buff_in, num );
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
		}
		curr_size += num;
#ifdef vms
		if(	(format_data->flags & IT_IS_FLOAT) && ((format & 0xF0) == IEEE_FLOAT) )
		{
			switch(format_data->flags & 0x3)
			{
				case SWAPL :
					num = format_data->par_num;
					(int *)buff_out -= num;
					for( i = 0; i < num; i++) 
					{
						cvt$convert_float((void *)buff_out, CVT$K_VAX_F, 
										  (void *)buff_out, CVT$K_IEEE_S,
										  0 );
						((int *)buff_out)++;
					}
					break;
				case SWAPD :
#ifdef __alpha
					input_code = CVT$K_VAX_G;
#else
					input_code = CVT$K_VAX_D;
#endif
					num = format_data->par_num;
					(double *)buff_out -= num;
					for( i = 0; i < num; i++ )
					{
						cvt$convert_float((void *)buff_out, input_code,
										  (void *)buff_out, CVT$K_IEEE_T,
										  0 );
						((double *)buff_out)++;
					}
					break;
			}
		}
#endif
		format_data->par_num = old_par_num;
	}
	return(curr_out);
}
*/
/*
int copy_swap_buffer_out(format, format_data, buff_out, buff_in, size)
int format;
register int size;
register FORMAT_STR *format_data;
register void *buff_out, *buff_in;
{
	int num, pad_num, curr_size = 0, curr_out = 0;
	int next_par_bytes, curr_par_num;
	
	if(!format_data->par_bytes) {
		if(buff_in != buff_out)
			memcpy( buff_out, buff_in, size );
		return(size);
	}
	next_par_bytes = format_data->par_bytes;
	while(next_par_bytes)
	{
		curr_par_num = format_data->par_num;
		if((curr_size+(curr_par_num * format_data->par_bytes))
		   > size)
		{
			curr_par_num = (size - curr_size)/format_data->par_bytes;
			next_par_bytes = 0;
		}
		switch(format_data->flags & 0x3) 
		{
			case NOSWAP :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_CHAR);

				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPS :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_SHORT);

				if(Dis_padding)
				{
					if(pad_num = check_padding(curr_size, SIZEOF_SHORT))
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPL :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_LONG);

				if(Dis_padding)
				{
					if(pad_num = check_padding(curr_size, SIZEOF_LONG))
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPD :
				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_DOUBLE);

				if(Dis_padding)
				{
					if(pad_num = check_padding(curr_size, SIZEOF_DOUBLE))
					{
						inc_pter( buff_in, pad_num);
						curr_size += pad_num;
					}
				}
				memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
		}
#ifdef vms
		check_vms_in(format_data->flags, format, curr_par_num, buff_out);
#endif
		curr_size += num;
		format_data++;
		if(next_par_bytes)
			next_par_bytes = format_data->par_bytes;
	}
	return(curr_out);
}
*/

int copy_swap_buffer_in(format_data, buff_out, buff_in, size)
register int size;
register FORMAT_STR *format_data;
register void *buff_out, *buff_in;
{
	int num, pad_num, curr_size = 0, curr_out = 0;
	int next_par_bytes, curr_par_num, curr_par_bytes;
	
	if(!format_data->par_bytes) {
		if(buff_in != buff_out)
			memcpy( buff_out, buff_in, size );
		return(size);
	}
	next_par_bytes = format_data->par_bytes;
	while(next_par_bytes)
	{
		curr_par_num = format_data->par_num;
		curr_par_bytes = format_data->par_bytes;
		if((curr_size+(curr_par_num * curr_par_bytes))
		   > size)
		{
			curr_par_num = (size - curr_size)/curr_par_bytes;
			next_par_bytes = 0;
		}
		switch(format_data->flags & 0x3) 
		{
			case NOSWAP :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, curr_par_bytes);

				if(Dic_padding)
				{
					if(curr_par_bytes == SIZEOF_DOUBLE)
					{
#ifdef PADD64
						if(pad_num = check_padding(curr_out, SIZEOF_DOUBLE))
#else
						if(pad_num = check_padding(curr_out, SIZEOF_LONG))
#endif
						{
							inc_pter( buff_out, pad_num);
							curr_out += pad_num;
						}
					}
					else
					{
						if(pad_num = check_padding(curr_out, curr_par_bytes))
						{
							inc_pter( buff_out, pad_num);
							curr_out += pad_num;
						}
					}
				}

				if(buff_in != buff_out)
					memcpy( buff_out, buff_in, num);
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPS :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_SHORT);

				if(Dic_padding)
				{
					if(pad_num = check_padding(curr_out, SIZEOF_SHORT))
					{
						inc_pter( buff_out, pad_num);
						curr_out += pad_num;
					}
				}
				_swaps_buffer( (short *)buff_out, (short *)buff_in, num/SIZEOF_SHORT) ;
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPL :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_LONG);

				if(Dic_padding)
				{
					if(pad_num = check_padding(curr_out, SIZEOF_LONG))
					{
						inc_pter( buff_out, pad_num);
						curr_out += pad_num;
					}
				}
				_swapl_buffer( (short *)buff_out, (short *)buff_in, num/SIZEOF_LONG) ;
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
			case SWAPD :

				num = get_curr_bytes(curr_par_num,
					size - curr_size, SIZEOF_DOUBLE);

				if(Dic_padding)
				{
#ifdef PADD64
					if(pad_num = check_padding(curr_out, SIZEOF_DOUBLE))
#else
					if(pad_num = check_padding(curr_out, SIZEOF_LONG))
#endif
					{
						inc_pter( buff_out, pad_num);
						curr_out += pad_num;
					}
				}
				_swapd_buffer( (short *)buff_out, (short *)buff_in, num/SIZEOF_DOUBLE) ;
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
				curr_out += num;
				break;
		}
#ifdef vms
		check_vms_in(format_data->flags, curr_par_num, curr_par_bytes, buff_out);
#endif
		curr_size += num;
		format_data++;
		if(next_par_bytes)
			next_par_bytes = format_data->par_bytes;
	}
	return(curr_out);
}
				
/*
int copy_swap_buffer_in(format_data, buff_out, buff_in, size)
register int size;
register FORMAT_STR *format_data;
register void *buff_out, *buff_in;
{
	register int num;
#ifdef unix
	int res;
#endif
	int i, old_par_num;
	int curr_size = 0, full_size = 0;
	register int curr_fill = 0, curr_data = 0;
#ifdef vms
	unsigned int input_code, output_code;
#endif

	if(!format_data->par_bytes)
	{
		if(buff_in != buff_out)
			memcpy( buff_out, buff_in, size );
		return(size);
	}
	for( ; format_data->par_bytes; format_data++)
	{
		full_size += curr_fill;
		curr_fill = curr_data = 0;
		old_par_num = format_data->par_num;
		if((curr_size+(format_data->par_num * format_data->par_bytes))
		    > size)
		{
			format_data->par_num = (size - curr_size)/format_data->par_bytes;
			(format_data+1)->par_bytes = 0;
		}
		switch(format_data->flags & 0x3)
		{
			case NOSWAP :
				if(!(num = (format_data->par_num * format_data->par_bytes)))
					format_data->par_num = num = size - curr_size;
				if(num > (size - curr_size))
					num = size - curr_size;
				if(buff_in != buff_out)
					memcpy( buff_out, buff_in, num );
				inc_pter( buff_in, num);
				inc_pter( buff_out, num);
if(Dic_padding)
{
#ifdef unix
#ifndef aix			    
#ifndef linux
#ifndef LYNXOS
				if((format_data + 1)->par_bytes == SIZEOF_DOUBLE)
				{
					if(res = num % SIZEOF_DOUBLE)
					{
						res = SIZEOF_DOUBLE - res;
						inc_pter( buff_out, res) ;
						curr_fill += res;
					}
				}
				else 				 
#endif
#endif
#endif
				if((format_data + 1)->par_bytes > SIZEOF_SHORT)
				{
				if(res = (full_size+num) % SIZEOF_LONG)
				{
					res = SIZEOF_LONG - res;
					inc_pter( buff_out, res);
					curr_fill += res;
				}
				}
				else
				if((format_data + 1)->par_bytes == SIZEOF_SHORT)
				{
				if(res = (full_size+num) % SIZEOF_SHORT)
				{
					res = SIZEOF_SHORT - res;
					inc_pter( buff_out, res);
					curr_fill += res;
				}
				}
#endif
}
				num = num / format_data->par_bytes;
				break;
			case SWAPS :
				if(!(num = format_data->par_num))
					format_data->par_num = num = (size - curr_size)/SIZEOF_SHORT;
				_swaps_buffer( (short *)buff_out, (short *)buff_in, num) ;
				inc_pter( buff_in, SIZEOF_SHORT * num);
				inc_pter( buff_out, SIZEOF_SHORT * num);
if(Dic_padding)
{
#ifdef unix
				res = ((full_size+(num*SIZEOF_SHORT)) % SIZEOF_LONG);
				if((format_data + 1)->par_bytes > SIZEOF_SHORT)
				{
				if(res)
				{
					res = SIZEOF_LONG - res;
					inc_pter( buff_out, res);
					curr_fill += res;
				}
				}
#endif
}
				break;
			case SWAPL :
				if(!(num = format_data->par_num))
					format_data->par_num = num = (size - curr_size)/SIZEOF_LONG;
				_swapl_buffer( (int *)buff_out, (int *)buff_in, num);
				inc_pter( buff_in, SIZEOF_LONG * num);
				inc_pter( buff_out, SIZEOF_LONG * num);
				break;
			case SWAPD :
				if(!(num = format_data->par_num))
					format_data->par_num = num = (size - curr_size)/SIZEOF_DOUBLE;
if(Dic_padding)
{
#ifdef unix
#ifndef aix
#ifndef linux
#ifndef LYNXOS
				if(res = (full_size % SIZEOF_DOUBLE))
				{
					res = SIZEOF_DOUBLE - res;
					inc_pter( buff_out, res);
					curr_fill += res;
				}
#endif
#endif
#endif
#endif
}
				_swapd_buffer( (double *)buff_out, (double *)buff_in, num);
				inc_pter( buff_in, SIZEOF_DOUBLE * num);
				inc_pter( buff_out, SIZEOF_DOUBLE * num);
				break;
		}
		curr_data = num * format_data->par_bytes;
		curr_size += curr_data;
		full_size += curr_data;
#ifdef vms
		if(format_data->flags & 0xF0)
		{
			switch(format_data->par_bytes) {
				case SIZEOF_FLOAT :
					if((format_data->flags & 0xF0) == IEEE_FLOAT)
					{
						num = format_data->par_num;
						(int *)buff_out -= num;
						for( i = 0; i<num; i++)
						{
							cvt$convert_float((void *)buff_out, CVT$K_IEEE_S,
										  (void *)buff_out, CVT$K_VAX_F,
										  0 );
							((int *)buff_out)++;
						}
					}
					break;
				case SIZEOF_DOUBLE :
#ifdef __alpha
					output_code = CVT$K_VAX_G;
#else
					output_code = CVT$K_VAX_D;
#endif
					switch(format_data->flags & 0xF0)
					{
						case VAX_FLOAT:
							input_code = CVT$K_VAX_D;
							break;	
						case AXP_FLOAT:
							input_code = CVT$K_VAX_G;
							break;	
						case IEEE_FLOAT:
							input_code = CVT$K_IEEE_T;
							break;	
					}							
					num = format_data->par_num;
					(double *)buff_out -= num;
					for( i = 0; i<num; i++)
					{
						cvt$convert_float((void *)buff_out, input_code,
										  (void *)buff_out, output_code,
										  0 );
						((double *)buff_out)++;
					}
					break;
			}
		}
#endif
		format_data->par_num = old_par_num;
	}
	full_size += curr_fill;
	return(full_size);
}
*/
typedef struct T1
{
	char c;
	double d;
}TT1;

typedef struct T2
{
	char c;
	short s;
	long l;
	int i;
	double d;
}TT2;

typedef struct T3
{
//	char c;
	short s;
	long l;
	int i;
	double d;
}TT3;

typedef struct T4
{
	short s;
	char c1;
	char c2;
	char c3;
}TT4;


main()
{
	TT1 sin1, sout1;
	TT2 sin2, sout2;
	TT3 sin3, sout3;
	TT4 sin4, sout4;
	FORMAT_STR format_data[16], format_data_cp[16], *formatp;
	int i, sizein, sizeout;
	char *sout, *ptr;

	sin1.c = 'a';
	sin1.d = 234.567;

	printf("sizeof ptr = %d\n",sizeof(void *));
	get_format_data(format_data,"C:1;D:1");
	memcpy(format_data_cp, format_data, sizeof(format_data_cp));
	sizein = sizeof(TT1);
	sout = malloc(sizein);
	sizeout = copy_swap_buffer_out(0, format_data_cp, sout, &sin1, sizein);
	ptr = (char *)&sin1;
	printf("*** input %d:\n", sizein);
	for(i = 0; i < sizein; i++)
		printf("%08X\n",*(ptr++));
	ptr = sout;
	printf("*** output %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	for(formatp = format_data_cp; formatp->par_bytes; formatp++)
		formatp->flags &= 0xFFF0;    /* NOSWAP */
	sizein = sizeout;
	sizeout = copy_swap_buffer_in(format_data_cp, &sout1, sout, sizein);
	ptr = (char *)&sout1;
	printf("*** result %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	sin2.c = 'a';
	sin2.s = 0x22;
	sin2.l = 0x123456;
	sin2.i = 0x87654321;
	sin2.d = 234.567;

	get_format_data(format_data,"C:1;S:1;L:1;I:1;D:1");
	memcpy(format_data_cp, format_data, sizeof(format_data_cp));
	sizein = sizeof(TT2);
	sout = malloc(sizein);
	sizeout = copy_swap_buffer_out(0, format_data_cp, sout, &sin2, sizein);
	ptr = (char *)&sin2;
	printf("*** input %d:\n", sizein);
	for(i = 0; i < sizein; i++)
		printf("%08X\n",*(ptr++));
	ptr = sout;
	printf("*** output %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	for(formatp = format_data_cp; formatp->par_bytes; formatp++)
		formatp->flags &= 0xFFF0;    /* NOSWAP */
	sizein = sizeout;
	sizeout = copy_swap_buffer_in(format_data_cp, &sout2, sout, sizein);
	ptr = (char *)&sout2;
	printf("*** result %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	//	sin2.c = 'a';
	sin3.s = 0x22;
	sin3.l = 0x123456;
	sin3.i = 0x87654321;
	sin3.d = 234.567;

	get_format_data(format_data,"S:1;L:1;I:1;D:1");
	memcpy(format_data_cp, format_data, sizeof(format_data_cp));
	sizein = sizeof(TT3);
	sout = malloc(sizein);
	sizeout = copy_swap_buffer_out(0, format_data_cp, sout, &sin3, sizein);
	ptr = (char *)&sin3;
	printf("*** input %d:\n", sizein);
	for(i = 0; i < sizein; i++)
		printf("%08X\n",*(ptr++));
	ptr = sout;
	printf("*** output %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	for(formatp = format_data_cp; formatp->par_bytes; formatp++)
		formatp->flags &= 0xFFF0;    /* NOSWAP */
	sizein = sizeout;
	sizeout = copy_swap_buffer_in(format_data_cp, &sout3, sout, sizein);
	ptr = (char *)&sout3;
	printf("*** result %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

	sin4.s = 0x22;
	sin4.c1 = 'a';
	sin4.c2 = 'b';
	sin4.c3 = 'c';

	get_format_data(format_data,"S:1;C:1;C:1;C:1");
	memcpy(format_data_cp, format_data, sizeof(format_data_cp));
	sizein = sizeof(TT4);
	sout = malloc(sizein);
	sizeout = copy_swap_buffer_out(0, format_data_cp, sout, &sin4, sizein);
	ptr = (char *)&sin4;
	printf("*** input %d:\n", sizein);
	for(i = 0; i < sizein; i++)
		printf("%08X\n",*(ptr++));
	ptr = sout;
	printf("*** output %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));
	
	for(formatp = format_data_cp; formatp->par_bytes; formatp++)
		formatp->flags &= 0xFFF0;    /* NOSWAP */
	sizein = sizeout;
	sizeout = copy_swap_buffer_in(format_data_cp, &sout4, sout, sizein);
	ptr = (char *)&sout4;
	printf("*** result %d:\n", sizeout);
	for(i = 0; i < sizeout; i++)
		printf("%08X\n",*(ptr++));

//	while(1);
//		sleep(10);
}
