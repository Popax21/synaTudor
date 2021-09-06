/*******************************************************************************

License:
This software and/or related materials was developed at the National Institute
of Standards and Technology (NIST) by employees of the Federal Government
in the course of their official duties. Pursuant to title 17 Section 105
of the United States Code, this software is not subject to copyright
protection and is in the public domain.

This software and/or related materials have been determined to be not subject
to the EAR (see Part 734.3 of the EAR for exact details) because it is
a publicly available technology and software, and is freely distributed
to any interested party with no licensing requirements.  Therefore, it is
permissible to distribute this software as a free download from the internet.

Disclaimer:
This software and/or related materials was developed to promote biometric
standards and biometric technology testing for the Federal Government
in accordance with the USA PATRIOT Act and the Enhanced Border Security
and Visa Entry Reform Act. Specific hardware and software products identified
in this software were used in order to perform the software development.
In no case does such identification imply recommendation or endorsement
by the National Institute of Standards and Technology, nor does it imply that
the products and equipment identified are necessarily the best available
for the purpose.

This software and/or related materials are provided "AS-IS" without warranty
of any kind including NO WARRANTY OF PERFORMANCE, MERCHANTABILITY,
NO WARRANTY OF NON-INFRINGEMENT OF ANY 3RD PARTY INTELLECTUAL PROPERTY
or FITNESS FOR A PARTICULAR PURPOSE or for any purpose whatsoever, for the
licensed product, however used. In no event shall NIST be liable for any
damages and/or costs, including but not limited to incidental or consequential
damages of any kind, including economic damage or injury to property and lost
profits, regardless of whether NIST shall be advised, have reason to know,
or in fact shall know of the possibility.

By using this software, you agree to bear all risk relating to quality,
use and performance of the software and/or related materials.  You agree
to hold the Government harmless from any claim arising from your use
of the software.

*******************************************************************************/


/***********************************************************************
      LIBRARY: LFS - NIST Latent Fingerprint System

      FILE:    LINE.C
      AUTHOR:  Michael D. Garris
      DATE:    03/16/1999

      Contains routines that compute contiguous linear trajectories
      between two coordinate points required by the NIST Latent
      Fingerprint System (LFS).

***********************************************************************
               ROUTINES:
                        line_points()
***********************************************************************/

#include <stdio.h>
#include <lfs.h>

/*************************************************************************
**************************************************************************
#cat: line_points - Returns the contiguous coordinates of a line connecting
#cat:               2 specified points.

   Input:
      x1      - x-coord of first point
      y1      - y-coord of first point
      x2      - x-coord of second point
      y2      - y-coord of second point
   Output:
      ox_list - x-coords along line trajectory
      oy_list - y-coords along line trajectory
      onum    - number of points along line trajectory
   Return Code:
      Zero      - successful completion
      Negative  - system error
**************************************************************************/
int line_points(int **ox_list, int **oy_list, int *onum,
                const int x1, const int y1, const int x2, const int y2)
{
   int asize;
   int dx, dy, adx, ady;
   int x_incr, y_incr;
   int i, inx, iny, intx, inty;
   double x_factor, y_factor;
   double rx, ry;
   int ix, iy;
   int *x_list, *y_list;

   /* Compute maximum number of points needed to hold line segment. */
   asize = max(abs(x2-x1)+2, abs(y2-y1)+2);

   /* Allocate x and y-pixel coordinate lists to length 'asize'. */
   x_list = (int *)g_malloc(asize * sizeof(int));
   y_list = (int *)g_malloc(asize * sizeof(int));

   /* Compute delta x and y. */
   dx = x2 - x1;
   dy = y2 - y1;

   /* Set x and y increments. */
   if(dx >= 0)
      x_incr = 1;
   else
      x_incr = -1;

   if(dy >= 0)
      y_incr = 1;
   else
      y_incr = -1;

   /* Compute |DX| and |DY|. */
   adx = abs(dx);
   ady = abs(dy);

   /* Set x-orientation. */
   if(adx > ady)
      inx = 1;
   else
      inx = 0;

   /* Set y-orientation. */
   if(ady > adx)
      iny = 1;
   else
      iny = 0;

   /*  CASE 1: |DX| > |DY|              */
   /*     Increment in X by +-1         */
   /*               in Y by +-|DY|/|DX| */
   /*        inx   =  1                 */
   /*        iny   =  0                 */
   /*        intx  =  1 (inx)           */
   /*        inty  =  0 (iny)           */
   /*  CASE 2: |DX| < |DY|              */
   /*     Increment in Y by +-1         */
   /*               in X by +-|DX|/|DY| */
   /*        inx   =  0                 */
   /*        iny   =  1                 */
   /*        intx  =  0 (inx)           */
   /*        inty  =  1 (iny)           */
   /*  CASE 3: |DX| == |DY|             */
   /*        inx   =  0                 */
   /*        iny   =  0                 */
   /*        intx  =  1                 */
   /*        inty  =  1                 */
   intx = 1 - iny;
   inty = 1 - inx;

   /*                                        DX           */
   /* x_factor = (inx * +-1) +  ( iny * ------------ )    */
   /*                                   max(1, |DY|)      */
   /*                                                     */
   x_factor = (inx * x_incr) + (iny * ((double)dx/max(1, ady)));

   /*                                        DY           */
   /* y_factor = (iny * +-1) +  ( inx * ------------ )    */
   /*                                   max(1, |DX|)      */
   /*                                                     */
   y_factor = (iny * y_incr) + (inx * ((double)dy/max(1, adx)));

   /* Initialize integer coordinates. */
   ix = x1;
   iy = y1;
   /* Set floating point coordinates. */
   rx = (double)x1;
   ry = (double)y1;

   /* Initialize to first point in line segment. */
   i = 0;

   /* Assign first point into coordinate list. */
   x_list[i] = x1;
   y_list[i++] = y1;

   while((ix != x2) || (iy != y2)){

      if(i >= asize){
         fprintf(stderr, "ERROR : line_points : coord list overflow\n");
         g_free(x_list);
         g_free(y_list);
         return(-412);
      }

      rx += x_factor;
      ry += y_factor;

      /* Need to truncate precision so that answers are consistent */
      /* on different computer architectures when truncating doubles. */
      rx = trunc_dbl_precision(rx, TRUNC_SCALE);
      ry = trunc_dbl_precision(ry, TRUNC_SCALE);

      /* Compute new x and y-pixel coords in floating point and  */
      /* then round to the nearest integer.                      */
      ix = (intx * (ix + x_incr)) + (iny * (int)(rx + 0.5));
      iy = (inty * (iy + y_incr)) + (inx * (int)(ry + 0.5));

      /* Assign first point into coordinate list. */
      x_list[i] = ix;
      y_list[i++] = iy;
   }

   /* Set output pointers. */
   *ox_list = x_list;
   *oy_list = y_list;
   *onum = i;

   /* Return normally. */
   return(0);
}
