/***************************************************************************
Copyright (c) 2014, The OpenBLAS Project
All rights reserved.
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
1. Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
notice, this list of conditions and the following disclaimer in
the documentation and/or other materials provided with the
distribution.
3. Neither the name of the OpenBLAS project nor the names of
its contributors may be used to endorse or promote products
derived from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE OPENBLAS PROJECT OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*****************************************************************************/

#define HAVE_KERNEL_16 1
static void sdot_kernel_16( BLASLONG n, FLOAT *x, FLOAT *y , FLOAT *dot) __attribute__ ((noinline));

static void sdot_kernel_16( BLASLONG n, FLOAT *x, FLOAT *y, FLOAT *dot)
{


	BLASLONG register i = 0;


  if ( n < 4096 )
  {

	__asm__  __volatile__
	(
	"vxorps		%%xmm4, %%xmm4, %%xmm4	             \n\t"
	"vxorps		%%xmm5, %%xmm5, %%xmm5	             \n\t"
	"vxorps		%%xmm6, %%xmm6, %%xmm6	             \n\t"
	"vxorps		%%xmm7, %%xmm7, %%xmm7	             \n\t"

	".align 16				             \n\t"
	"1:				             \n\t"
        "vmovups                  (%2,%0,4), %%xmm12         \n\t"  // 4 * x
        "vmovups                16(%2,%0,4), %%xmm13         \n\t"  // 4 * x
        "vmovups                32(%2,%0,4), %%xmm14         \n\t"  // 4 * x
        "vmovups                48(%2,%0,4), %%xmm15         \n\t"  // 4 * x

	"vfmadd231ps       (%3,%0,4), %%xmm12, %%xmm4 \n\t"  // 4 * y
	"vfmadd231ps     16(%3,%0,4), %%xmm13, %%xmm5 \n\t"  // 4 * y
        "vmovups                64(%2,%0,4), %%xmm0          \n\t"  // 4 * x
        "vmovups                80(%2,%0,4), %%xmm1          \n\t"  // 4 * x
	"vfmadd231ps     32(%3,%0,4), %%xmm14, %%xmm6 \n\t"  // 4 * y
	"vfmadd231ps     48(%3,%0,4), %%xmm15, %%xmm7 \n\t"  // 4 * y

        "vmovups                96(%2,%0,4), %%xmm2          \n\t"  // 4 * x
        "vmovups               112(%2,%0,4), %%xmm3          \n\t"  // 4 * x

	"vfmadd231ps     64(%3,%0,4), %%xmm0 , %%xmm4 \n\t"  // 4 * y
	"vfmadd231ps     80(%3,%0,4), %%xmm1 , %%xmm5 \n\t"  // 4 * y
	"vfmadd231ps     96(%3,%0,4), %%xmm2 , %%xmm6 \n\t"  // 4 * y
	"vfmadd231ps    112(%3,%0,4), %%xmm3 , %%xmm7 \n\t"  // 4 * y

	"addq		$32, %0	  	 	             \n\t"
	"subq	        $32, %1			             \n\t"		
	"jnz		1b		             \n\t"

	"vaddps        %%xmm4, %%xmm5, %%xmm4	\n\t"
	"vaddps        %%xmm6, %%xmm7, %%xmm6	\n\t"
	"vaddps        %%xmm4, %%xmm6, %%xmm4	\n\t"

	"vhaddps        %%xmm4, %%xmm4, %%xmm4	\n\t"
	"vhaddps        %%xmm4, %%xmm4, %%xmm4	\n\t"

	"vmovss		%%xmm4,    (%4)		\n\t"

	: 
          "+r" (i),	// 0	
	  "+r" (n)  	// 1
        :
          "r" (x),      // 2
          "r" (y),      // 3
          "r" (dot)     // 4
	: "cc", 
	  "%xmm0", "%xmm1", "%xmm2", "%xmm3", 
	  "%xmm4", "%xmm5", "%xmm6", "%xmm7", 
	  "%xmm12", "%xmm13", "%xmm14", "%xmm15",
	  "memory"
	);
	return;

  }

	__asm__  __volatile__
	(
	"vxorps		%%xmm4, %%xmm4, %%xmm4	             \n\t"
	"vxorps		%%xmm5, %%xmm5, %%xmm5	             \n\t"
	"vxorps		%%xmm6, %%xmm6, %%xmm6	             \n\t"
	"vxorps		%%xmm7, %%xmm7, %%xmm7	             \n\t"

	".align 16				             \n\t"
	"1:				             \n\t"
	"prefetcht0	512(%2,%0,4)		     \n\t"
        "vmovups                  (%2,%0,4), %%xmm12         \n\t"  // 4 * x
        "vmovups                16(%2,%0,4), %%xmm13         \n\t"  // 4 * x
        "vmovups                32(%2,%0,4), %%xmm14         \n\t"  // 4 * x
        "vmovups                48(%2,%0,4), %%xmm15         \n\t"  // 4 * x

	"prefetcht0	512(%3,%0,4)		     \n\t"
	"vfmadd231ps       (%3,%0,4), %%xmm12, %%xmm4 \n\t"  // 4 * y
	"vfmadd231ps     16(%3,%0,4), %%xmm13, %%xmm5 \n\t"  // 4 * y
	"prefetcht0	576(%2,%0,4)		     \n\t"
        "vmovups                64(%2,%0,4), %%xmm0          \n\t"  // 4 * x
        "vmovups                80(%2,%0,4), %%xmm1          \n\t"  // 4 * x
	"prefetcht0	576(%3,%0,4)		     \n\t"
	"vfmadd231ps     32(%3,%0,4), %%xmm14, %%xmm6 \n\t"  // 4 * y
	"vfmadd231ps     48(%3,%0,4), %%xmm15, %%xmm7 \n\t"  // 4 * y

        "vmovups                96(%2,%0,4), %%xmm2          \n\t"  // 4 * x
        "vmovups               112(%2,%0,4), %%xmm3          \n\t"  // 4 * x

	"vfmadd231ps     64(%3,%0,4), %%xmm0 , %%xmm4 \n\t"  // 4 * y
	"vfmadd231ps     80(%3,%0,4), %%xmm1 , %%xmm5 \n\t"  // 4 * y
	"vfmadd231ps     96(%3,%0,4), %%xmm2 , %%xmm6 \n\t"  // 4 * y
	"vfmadd231ps    112(%3,%0,4), %%xmm3 , %%xmm7 \n\t"  // 4 * y

	"addq		$32, %0	  	 	             \n\t"
	"subq	        $32, %1			             \n\t"		
	"jnz		1b		             \n\t"

	"vaddps        %%xmm4, %%xmm5, %%xmm4	\n\t"
	"vaddps        %%xmm6, %%xmm7, %%xmm6	\n\t"
	"vaddps        %%xmm4, %%xmm6, %%xmm4	\n\t"

	"vhaddps        %%xmm4, %%xmm4, %%xmm4	\n\t"
	"vhaddps        %%xmm4, %%xmm4, %%xmm4	\n\t"

	"vmovss		%%xmm4,    (%4)		\n\t"

	: 
          "+r" (i),	// 0	
	  "+r" (n)  	// 1
        :
          "r" (x),      // 2
          "r" (y),      // 3
          "r" (dot)     // 4
	: "cc", 
	  "%xmm0", "%xmm1", "%xmm2", "%xmm3", 
	  "%xmm4", "%xmm5", "%xmm6", "%xmm7", 
	  "%xmm12", "%xmm13", "%xmm14", "%xmm15",
	  "memory"
	);


} 


