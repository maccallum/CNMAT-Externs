/* * Copyright (c) 1992 Regents of the University of California. * All rights reserved. * * Redistribution and use in source and binary forms are permitted * provided that the above copyright notice and this paragraph are * duplicated in all such forms and that any documentation, * advertising materials, and other materials related to such * distribution and use acknowledge that the software was developed * by the University of California, Berkeley.  The name of the * University may not be used to endorse or promote products derived * from this software without specific prior written permission. * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. *//* * fft.h * real and complex forward and inverse fft's */#ifndef TRUEtypedef int Boolean;#define TRUE 1#define FALSE 0#endif#ifndef PI#define PI 3.14159265358979323#endif#define NMAX 12#define CMAX 8 // number of cached twidle factor arraysvoid fftComplex(int n, float *a,  Boolean notinverse, float* PtrMemory);void realfft(int n, float *a, Boolean notinverse, float* PtrMemory);void fftRealfast(int n, float *r, float* PtrMemory);void ifftRealfast(int n, float *rc, float* PtrMemory);