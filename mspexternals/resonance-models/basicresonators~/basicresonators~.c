
/*
	�1988,1989,2007-2013 Adrian Freed

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
NAME: basicresonators~
DESCRIPTION: Parallel bank of resonant filters
AUTHOR: Adrian Freed
COPYRIGHT_YEARS: 1988-2013
VERSION 2.0: second release 
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
*/
#include "ext.h"
#include "ext_obex.h"


#include "z_dsp.h"
#include <math.h>

#ifdef WIN_VERSION
#include <pmmintrin.h>
#endif

void *resonators_class;
#define MAXRESONANCES 1024


typedef  struct dresdesc
{
	double out1, out2;   // state
	double a1,a1prime, b1, b2;
	double o_a1, o_b1, o_b2;
} dresdesc;

/* bank of filters */
typedef struct 
{
	t_pxobject b_obj;
	short b_connected;

	dresdesc *dbase;
	int nres; 
	int nmax;	/* maximum number of filters */

	double samplerate;
	double sampleinterval;

	void *outlet1;
} resbank;
typedef resbank t_resonators;

t_int *resonators_perform(t_int *w);
t_int *resonators2_perform(t_int *w);
void resonators_dsp(t_resonators *x, t_signal **sp, short *connect);
void resonators_float(t_resonators *x, double f);
void resonators_int(t_resonators *x, long n);
void resonators_list(t_resonators *x, t_symbol *s, short argc, t_atom *argv);
void resonators_clear(t_resonators *x);
void resonators_assist(t_resonators *x, void *b, long m, long a, char *s);
void *resonators_new(t_symbol *s, short argc, t_atom *argv);
void resonators_tellmeeverything(t_resonators *x);
void resonators_free(t_resonators *x);



//#define MAXMAXVECTOR 4096
// double precision interpolating (smooth) with input
t_int *diresonators_perform(t_int *w);
t_int *diresonators_perform(t_int *w)
{
	const t_float *in = (t_float *)(w[2]);
	t_float *fout = (t_float *)(w[3]);
	t_resonators *op = (t_resonators *)(w[1]);
	double out[(int)(w[4])];
	int n = (int)(w[4]);
	int nfilters = op->nres;
	register	double yn,yo;
	int i, j;
	double rate = 1.0/n;
	
	if(op->b_obj.z_disabled)
		goto out;
    
#ifdef SQUASH_DENORMALS
	static int sq;
	if(!sq){
		printf("squashing denormals\n");
		sq++;
	}
#if defined( __i386__ ) || defined( __x86_64__ )
    int oldMXCSR = _mm_getcsr(); // read the old MXCSR setting
    int newMXCSR = oldMXCSR | 0x8040; // set DAZ and FZ bits
    _mm_setcsr( newMXCSR );	 // write the new MXCSR setting to the MXCSR
#endif
#endif
	{
		dresdesc *f = op->dbase;
		for(j=0;j<n;++j)
			out[j] = 0.0;
	
		for(i=0;i< nfilters ;++i)
		{
			register double b1=f[i].o_b1, b2=f[i].o_b2, a1=f[i].o_a1;
			double a1inc = (f[i].a1-f[i].o_a1) *  rate;
			double b1inc = (f[i].b1-f[i].o_b1) *  rate;
			double b2inc = (f[i].b2-f[i].o_b2) *  rate;

	
			yo= f[i].out1;
			yn =f[i].out2;
			for(j=0;j<n;++j)
			{
				double x = yo;
				yo = b1*yo + b2*yn + a1*in[j];	

				 out[j] += yo;

				yn = x;
			a1 += a1inc;
			b1 += b1inc;
			b2 += b2inc;
			}

			f[i].o_a1 = f[i].a1;
			f[i].o_b1 = f[i].b1;
			f[i].o_b2 = f[i].b2;
			f[i].out1= yo;
			f[i].out2 = yn;	
		}
	}
	for(j=0;j<n;++j)
	{
		fout[j] = out[j]; 
	}
#ifdef SQUASH_DENORMALS
#if defined( __i386__ ) || defined( __x86_64__ )
	_mm_setcsr(oldMXCSR);
#endif
#endif

out:
	return (w+5);
}

// double precision smoothed without input
t_int *diresonators2_perform(t_int *w);
t_int *diresonators2_perform(t_int *w)
{
	t_float *fout = (t_float *)(w[2]);
	t_resonators *op = (t_resonators *)(w[1]);
	int n = (int)(w[3]);
	double out[(int)(w[4])];
	
	double rate = 1.0/n;
	int nfilters = op->nres;
		double yn,yo;
	int i, j;

	if(op->b_obj.z_disabled)
		goto out;
    
#ifdef SQUASH_DENORMALS
#if defined( __i386__ ) || defined( __x86_64__ )
    int oldMXCSR = _mm_getcsr(); // read the old MXCSR setting
    int newMXCSR = oldMXCSR | 0x8040; // set DAZ and FZ bits
    _mm_setcsr( newMXCSR );	 // write the new MXCSR setting to the MXCSR
#endif
#endif
	{
		dresdesc *f = op->dbase;
		for(j=0;j<n;++j)
			out[j] = 0.0;
	
		for(i=0;i< nfilters ;++i)
		{
			register double b1=f[i].o_b1, b2=f[i].o_b2;

			double b1inc = (f[i].b1-f[i].o_b1) *  rate;
			double b2inc = (f[i].b2-f[i].o_b2) *  rate;
	
			yo= f[i].out1;
			yn =f[i].out2;
			for(j=0;j<n;++j)
			{
				double x = yo;
				yo = b1*yo + b2*yn;	
                out[j] += yo;

				yn = x;

			b1 += b1inc;
			b2 += b2inc;
			}

			f[i].o_b1 = f[i].b1;
			f[i].o_b2 = f[i].b2;
			f[i].out1= yo;
			f[i].out2 = yn;
	
		}
	}
	
	for(j=0;j<n;++j)
	{
		fout[j] = out[j]; 
	}

    ifdef SQUASH_DENORMALS
#if defined( __i386__ ) || defined( __x86_64__ )
	_mm_setcsr(oldMXCSR);
#endif
#endif


out:

	return (w+4);
}


// set the state variables of the filter to 0.0
void resonators_clear(t_resonators *x)
{
	dresdesc *df = x->dbase;
	int i;
		for(i=0;i<x->nres;++i)
		{
			df[i].out1 = df[i].out2 = 0.0;

		}

}

void resonators_dsp(t_resonators *x, t_signal **sp, short *connect)
{
	resonators_clear(x);
	
	{
		if (x->b_connected = connect[1])
		{
				dsp_add(diresonators_perform, 4, x, sp[0]->s_vec, sp[1]->s_vec,  sp[0]->s_n);
			}
		else {
				dsp_add(diresonators2_perform, 3, x,sp[0]->s_vec,  sp[0]->s_n);
		}
	}
}

// This assumes we are single threaded, i.e. that we can never be interrupted by perform routine
void resonators_float(t_resonators *x, double ff)
{
	int i;
		dresdesc *dp = x->dbase;
		for(i=0;i<x->nres;++i)
		{
			dp[i].out2 += dp[i].a1prime*ff;		
		}
}


void resonators_int(t_resonators *x, long n)
{
	resonators_float(x,(double)n);
}

void resonators_bang(t_resonators *x);
void resonators_bang(t_resonators *x)
{
	int i;
	t_atom filterstate[MAXRESONANCES*5+1];

//  output filter state and coefficients to the second outlet
//  should we output the sample rate or normalize the coefficients?
		SETFLOAT(&filterstate[0], x->samplerate);

	for(i=0;i<x->nres;++i)
	{
		atom_setfloat(&filterstate[1+i*5+0], x->dbase[i].out1);
		atom_setfloat(&filterstate[1+i*5+1], x->dbase[i].out2);
		atom_setfloat(&filterstate[1+i*5+2], x->dbase[i].a1);
		atom_setfloat(&filterstate[1+i*5+3], x->dbase[i].b1); // some people think this should be negative
		atom_setfloat(&filterstate[1+i*5+4],x->dbase[i].b2);// and this
	}
	   outlet_list(x->outlet1, 0L, 1+i*5, filterstate);

}

void resonators_list(t_resonators *x, t_symbol *s, short argc, t_atom *argv)
{
	int i;
	// does this overlap stuff work? why dont we buffer a1prime and fastr? 
	

	int nres;
	double srbar = x->sampleinterval;
	dresdesc *dp = x->dbase;
	


	if (argc%3!=0) {
		post("� simpleresonators~:multiple of 3 floats required (frequency amplitude decayRate");
		return;
	}
			
	for(i=0; (i*3)<argc; ++i) {
		if (i >= MAXRESONANCES) {
			post("� simpleresonators~: warning: list has more than %ld resonances; dropping extras",
				 MAXRESONANCES);
			break;
		} else {
            // Here are the filter design equations		
			double f = atom_getfloatarg(i*3,argc,argv);
			double g = 	atom_getfloatarg(i*3+1,argc,argv);
			double rate = atom_getfloatarg(i*3+2,argc,argv);
			double r;
			r =  exp(-rate*srbar);
			if((f<=0.0) || (f>=(0.995*x->samplerate*0.5)) || (r<=0.0) || (r>1.0))
			{
	//				post("Warning parameters out of range");
                dp[i].b1 = dp[i].b1 = 0.0;
				dp[i].a1prime = 0.0;
			}
			else
			{
				double ts;
				f *= 2.0*3.14159265358979323*srbar;
				ts = g;
					ts *= sin(f);
				tb1[i] = r*cos(f)*2.0;
					ta1[i] = ts *  (1.0-r);   //this is one of the relavent L norms
				tb2[i] =  -r*r;
				 dp[i].a1prime = ts/tb2[i];  
					//this is the other norm that establishes the impulse response of the right amplitude (scaled 
					// so that it can be summed into the state variable outside the perform routine 
			}
		}
	}
	/* Now we know how many "good" resonances (freq > 0) were in the list */
	nres = i;

	for(i=0;i<nres;++i)
	{
			dp[i].b1 = tb1[i];
			 dp[i].a1 = ta1[i];
			 dp[i].b2 =  tb2[i];
			if(i>=x->nres) 	/* If there are now more resonances than there were: */ 

			{
			    // Set old a1 to zero so that the input to the new resonators will ramp up over the first signal vector.
			
				dp[i].o_a1 = 0.0;
				dp[i].o_b1 = dp[i].b1;
				dp[i].o_b2 = dp[i].b2;
				// Clear out state variables for these totally new resonances

				dp[i].out1 = dp[i].out2 = 0.0f;
			}
	}
	x->nres = nres;
	
//		post("nres %d x->nres %d", nres, x->nres);
}

void resonators_assist(t_resonators *x, void *b, long m, long a, char *s)
{
       if (m == ASSIST_OUTLET)
               sprintf(s,"(Signal) Resonator bank output");
       else {
             sprintf(s,"(Signal) Resonator bank input and control messages");
      }
}


void *resonators_new(t_symbol *s, short argc, t_atom *argv)
{
    t_resonators *x = (t_resonators *)newobject(resonators_class);
		x->samplerate =  sys_getsr();
		if(x->samplerate<=0.0)
			x->samplerate = 44100.0;
		x->sampleinterval = 1.0/x->samplerate;


	x->dbase = (dresdesc *) sysmem_newptr(MAXRESONANCES*sizeof(dresdesc));
    
  	    if(x->dbase==0)
	    {			post("� basicresonators~: warning: not enough memory. ");
	    	return 0;
	    }

   x->nres = MAXRESONANCES;
    resonators_clear(x); // clears state
    x->nres = 0;
    resonators_list(x,s,argc,argv);
    {

    	dresdesc *df = x->dbase;
		int i;
		for(i=0;i<x->nres;++i)
		{

			df[i].o_a1 = df[i].a1;
			df[i].o_b1 = df[i].b1;
			df[i].o_b2 = df[i].b2;

		}
	}
    	

   x->b_obj.z_misc = Z_NO_INPLACE;
    dsp_setup((t_pxobject *)x,1);
    x->b_obj.z_misc = Z_NO_INPLACE;
 
    x->outlet1 = listout(x);
		
    outlet_new((t_object *)x, "signal");
    return (x);
}

void resonators_free(t_resonators *x) {
  dsp_free(&(x->b_obj));
  sysmem_freeptr(x->dbase);
}

void main(void) {
	resonators_class = class_new("basicresonators~", (method) resonators_new,
                                 (method) resonators_free, (short)sizeof(t_resonators),
                                 0L, A_GIMME, 0);

    post("Copyright (c) 1986, 1987-2013 Adrian Freed. All Rights Reserved");
	post("Maximum number of resonances: %d", MAXRESONANCES);
    
	class_addmethod(resonators_class, (method)resonators_dsp, "dsp", A_CANT, 0);
	class_addmethod(resonators_class, (method)resonators_list, "list", A_GIMME, 0);
	class_addmethod(resonators_class, (method)resonators_clear, "clear", 0);
	class_addmethod(resonators_class, (method)resonators_bang, "bang", 0);
	class_addmethod(resonators_class, (method)resonators_float, "float", A_FLOAT, 0);
	class_addmethod(resonators_class, (method)resonators_int, "int", A_LONG, 0);
	class_addmethod(resonators_class, (method)resonators_assist, "assist", A_CANT, 0);
	class_dspinit(resonators_class);
    class_register(CLASS_BOX, resonators_class);
	return 0;
}

