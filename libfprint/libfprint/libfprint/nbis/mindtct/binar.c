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

      FILE:    BINAR.C
      AUTHOR:  Michael D. Garris
      DATE:    03/16/1999
      UPDATED: 10/04/1999 Version 2 by MDG
      UPDATED: 03/16/2005 by MDG

      Contains routines responsible for binarizing a grayscale image based
      on an arbitrarily-sized image and its precomputed direcitonal ridge
      flow (IMAP) as part of the NIST Latent Fingerprint System (LFS).

***********************************************************************
               ROUTINES:
                        binarize()
                        binarize_V2()
			binarize_image()
			binarize_image_V2()
                        dirbinarize()
                        isobinarize()

***********************************************************************/

#include <stdio.h>
#include <lfs.h>

/*************************************************************************
**************************************************************************
#cat: binarize - Takes a padded grayscale input image and its associated ridge
#cat:              direction flow NMAP and produces a binarized version of the
#cat:              image.  It then fills horizontal and vertical "holes" in the
#cat:              binary image results.

   Input:
      pdata    - padded input grayscale image
      pw       - padded width (in pixels) of input image
      ph       - padded height (in pixels) of input image
      nmap     - 2-D vector of IMAP directions and other codes
      mw       - width (in blocks) of the NMAP
      mh       - height (in blocks) of the NMAP
      dirbingrids - set of rotated grid offsets used for directional
              binarization
      lfsparms - parameters and thresholds for controlling LFS
   Output:
      optr  - points to created (unpadded) binary image
      ow    - width of binary image
      oh    - height of binary image
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/

/*************************************************************************
**************************************************************************
#cat: binarize_V2 - Takes a padded grayscale input image and its associated
#cat:              Direction Map and produces a binarized version of the
#cat:              image.  It then fills horizontal and vertical "holes" in
#cat:              the binary image results.  Note that the input image must
#cat:              be padded sufficiently to contain in memory rotated
#cat:              directional binarization grids applied to pixels along the
#cat:              perimeter of the input image.

   Input:
      pdata       - padded input grayscale image
      pw          - padded width (in pixels) of input image
      ph          - padded height (in pixels) of input image
      direction_map - 2-D vector of discrete ridge flow directions
      mw          - width (in blocks) of the map
      mh          - height (in blocks) of the map
      dirbingrids - set of rotated grid offsets used for directional
                    binarization
      lfsparms    - parameters and thresholds for controlling LFS
   Output:
      odata - points to created (unpadded) binary image
      ow    - width of binary image
      oh    - height of binary image
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/
int binarize_V2(unsigned char **odata, int *ow, int *oh,
          unsigned char *pdata, const int pw, const int ph,
          int *direction_map, const int mw, const int mh,
          const ROTGRIDS *dirbingrids, const LFSPARMS *lfsparms)
{
   unsigned char *bdata;
   int i, bw, bh, ret; /* return code */

   /* 1. Binarize the padded input image using directional block info. */
   if((ret = binarize_image_V2(&bdata, &bw, &bh, pdata, pw, ph,
                            direction_map, mw, mh,
                            lfsparms->blocksize, dirbingrids))){
      return(ret);
   }

   /* 2. Fill black and white holes in binary image. */
   /* LFS scans the binary image, filling holes, 3 times. */
   for(i = 0; i < lfsparms->num_fill_holes; i++)
      fill_holes(bdata, bw, bh);

   /* Return binarized input image. */
   *odata = bdata;
   *ow = bw;
   *oh = bh;
   return(0);
}

/*************************************************************************
**************************************************************************
#cat: binarize_image - Takes a grayscale input image and its associated
#cat:                  NMAP and generates a binarized version of the image.

   Input:
      pdata - padded input grayscale image
      pw    - padded width (in pixels) of input image
      ph    - padded height (in pixels) of input image
      nmap  - 2-D vector of IMAP directions and other codes
      mw    - width (in blocks) of the NMAP
      mh    - height (in blocks) of the NMAP
      imap_blocksize - dimension (in pixels) of each NMAP block
      dirbingrids - set of rotated grid offsets used for directional
              binarization
      isobin_grid_dim - dimension (in pixels) of grid used for isotropic
              binarization
   Output:
      optr  - points to binary image results
      ow    - points to binary image width
      oh    - points to binary image height
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/

/*************************************************************************
**************************************************************************
#cat: binarize_image_V2 - Takes a grayscale input image and its associated
#cat:              Direction Map and generates a binarized version of the
#cat:              image.  Note that there is no "Isotropic" binarization
#cat:              used in this version.

   Input:
      pdata       - padded input grayscale image
      pw          - padded width (in pixels) of input image
      ph          - padded height (in pixels) of input image
      direction_map - 2-D vector of discrete ridge flow directions
      mw          - width (in blocks) of the map
      mh          - height (in blocks) of the map
      blocksize   - dimension (in pixels) of each NMAP block
      dirbingrids - set of rotated grid offsets used for directional
                    binarization
   Output:
      odata  - points to binary image results
      ow     - points to binary image width
      oh     - points to binary image height
   Return Code:
      Zero     - successful completion
      Negative - system error
**************************************************************************/
int binarize_image_V2(unsigned char **odata, int *ow, int *oh,
                   unsigned char *pdata, const int pw, const int ph,
                   const int *direction_map, const int mw, const int mh,
                   const int blocksize, const ROTGRIDS *dirbingrids)
{
   int ix, iy, bw, bh, bx, by, mapval;
   unsigned char *bdata, *bptr;
   unsigned char *pptr, *spptr;

   /* Compute dimensions of "unpadded" binary image results. */
   bw = pw - (dirbingrids->pad<<1);
   bh = ph - (dirbingrids->pad<<1);

   bdata = (unsigned char *)g_malloc(bw * bh * sizeof(unsigned char));

   bptr = bdata;
   spptr = pdata + (dirbingrids->pad * pw) + dirbingrids->pad;
   for(iy = 0; iy < bh; iy++){
      /* Set pixel pointer to start of next row in grid. */
      pptr = spptr;
      for(ix = 0; ix < bw; ix++){

         /* Compute which block the current pixel is in. */
         bx = (int)(ix/blocksize);
         by = (int)(iy/blocksize);
         /* Get corresponding value in Direction Map. */
         mapval = *(direction_map + (by*mw) + bx);
         /* If current block has has INVALID direction ... */
         if(mapval == INVALID_DIR)
            /* Set binary pixel to white (255). */
            *bptr = WHITE_PIXEL;
         /* Otherwise, if block has a valid direction ... */
         else /*if(mapval >= 0)*/
            /* Use directional binarization based on block's direction. */
            *bptr = dirbinarize(pptr, mapval, dirbingrids);

         /* Bump input and output pixel pointers. */
         pptr++;
         bptr++;
      }
      /* Bump pointer to the next row in padded input image. */
      spptr += pw;
   }

   *odata = bdata;
   *ow = bw;
   *oh = bh;
   return(0);
}

/*************************************************************************
**************************************************************************
#cat: dirbinarize - Determines the binary value of a grayscale pixel based
#cat:               on a VALID IMAP ridge flow direction.

   CAUTION: The image to which the input pixel points must be appropriately
            padded to account for the radius of the rotated grid.  Otherwise,
            this routine may access "unkown" memory.

   Input:
      pptr        - pointer to current grayscale pixel
      idir        - IMAP integer direction associated with the block the
                    current is in
      dirbingrids - set of precomputed rotated grid offsets
   Return Code:
      BLACK_PIXEL - pixel intensity for BLACK
      WHITE_PIXEL - pixel intensity of WHITE
**************************************************************************/
int dirbinarize(const unsigned char *pptr, const int idir,
                const ROTGRIDS *dirbingrids)
{
   int gx, gy, gi, cy;
   int rsum, gsum, csum = 0;
   int *grid;
   double dcy;

   /* Assign nickname pointer. */
   grid = dirbingrids->grids[idir];
   /* Calculate center (0-oriented) row in grid. */
   dcy = (dirbingrids->grid_h-1)/(double)2.0;
   /* Need to truncate precision so that answers are consistent */
   /* on different computer architectures when rounding doubles. */
   dcy = trunc_dbl_precision(dcy, TRUNC_SCALE);
   cy = sround(dcy);
   /* Initialize grid's pixel offset index to zero. */
   gi = 0;
   /* Initialize grid's pixel accumulator to zero */
   gsum = 0;

   /* Foreach row in grid ... */
   for(gy = 0; gy < dirbingrids->grid_h; gy++){
      /* Initialize row pixel sum to zero. */
      rsum = 0;
      /* Foreach column in grid ... */
      for(gx = 0; gx < dirbingrids->grid_w; gx++){
         /* Accumulate next pixel along rotated row in grid. */
         rsum += *(pptr+grid[gi]);
         /* Bump grid's pixel offset index. */
         gi++;
      }
      /* Accumulate row sum into grid pixel sum. */
      gsum += rsum;
      /* If current row is center row, then save row sum separately. */
      if(gy == cy)
         csum = rsum;
   }

   /* If the center row sum treated as an average is less than the */
   /* total pixel sum in the rotated grid ...                      */
   if((csum * dirbingrids->grid_h) < gsum)
      /* Set the binary pixel to BLACK. */
      return(BLACK_PIXEL);
   else
      /* Otherwise set the binary pixel to WHITE. */
      return(WHITE_PIXEL);
}

/*************************************************************************
**************************************************************************
#cat: isobinarize - Determines the binary value of a grayscale pixel based
#cat:               on comparing the grayscale value with a surrounding
#cat:               neighborhood grid of pixels.  If the current pixel (treated
#cat:               as an average) is less than the sum of the pixels in
#cat:               the neighborhood, then the binary value is set to BLACK,
#cat:               otherwise it is set to WHITE.  This binarization technique
#cat:               is used when there is no VALID IMAP direction for the
#cat:               block in which the current pixel resides.

   CAUTION: The image to which the input pixel points must be appropriately
            padded to account for the radius of the neighborhood.  Otherwise,
            this routine may access "unkown" memory.

   Input:
      pptr - pointer to curent grayscale pixel
      pw   - padded width (in pixels) of the grayscale image
      ph   - padded height (in pixels) of the grayscale image
      isobin_grid_dim - dimension (in pixels) of the neighborhood
   Return Code:
      BLACK_PIXEL - pixel intensity for BLACK
      WHITE_PIXEL - pixel intensity of WHITE
**************************************************************************/
