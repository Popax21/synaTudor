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

      FILE:    FREE.C
      AUTHOR:  Michael D. Garris
      DATE:    03/16/1999

      Contains routines responsible for deallocating
      memories required by the NIST Latent Fingerprint System (LFS).

***********************************************************************
               ROUTINES:
                        free_dir2rad()
                        free_dftwaves()
                        free_rotgrids()
                        free_dir_powers()
***********************************************************************/

#include <stdio.h>
#include <lfs.h>

/*************************************************************************
**************************************************************************
#cat: free_dir2rad - Deallocates memory associated with a DIR2RAD structure

   Input:
      dir2rad - pointer to memory to be freed
*************************************************************************/
void free_dir2rad(DIR2RAD *dir2rad)
{
   g_free(dir2rad->cos);
   g_free(dir2rad->sin);
   g_free(dir2rad);
}

/*************************************************************************
**************************************************************************
#cat: free_dftwaves - Deallocates the memory associated with a DFTWAVES
#cat:                 structure

   Input:
      dftwaves - pointer to memory to be freed
**************************************************************************/
void free_dftwaves(DFTWAVES *dftwaves)
{
   int i;

   for(i = 0; i < dftwaves->nwaves; i++){
       g_free(dftwaves->waves[i]->cos);
       g_free(dftwaves->waves[i]->sin);
       g_free(dftwaves->waves[i]);
   }
   g_free(dftwaves->waves);
   g_free(dftwaves);
}

/*************************************************************************
**************************************************************************
#cat: free_rotgrids - Deallocates the memory associated with a ROTGRIDS
#cat:                 structure

   Input:
      rotgrids - pointer to memory to be freed
**************************************************************************/
void free_rotgrids(ROTGRIDS *rotgrids)
{
   int i;

   for(i = 0; i < rotgrids->ngrids; i++)
      g_free(rotgrids->grids[i]);
   g_free(rotgrids->grids);
   g_free(rotgrids);
}

/*************************************************************************
**************************************************************************
#cat: free_dir_powers - Deallocate memory associated with DFT power vectors

   Input:
      powers - vectors of DFT power values (N Waves X M Directions)
      nwaves - number of DFT wave forms used
**************************************************************************/
void free_dir_powers(double **powers, const int nwaves)
{
   int w;

   for(w = 0; w < nwaves; w++)
      g_free(powers[w]);

   g_free(powers);
}

