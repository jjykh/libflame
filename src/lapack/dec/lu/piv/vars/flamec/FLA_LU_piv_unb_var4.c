/*

    Copyright (C) 2014, The University of Texas at Austin

    This file is part of libflame and is available under the 3-Clause
    BSD license, which can be found in the LICENSE file at the top-level
    directory, or at http://opensource.org/licenses/BSD-3-Clause

*/

#include "FLAME.h"

#ifdef FLA_ENABLE_NON_CRITICAL_CODE

FLA_Error FLA_LU_piv_unb_var4( FLA_Obj A, FLA_Obj p )
{
  FLA_Obj ATL,   ATR,      A00,  a01,     A02, 
          ABL,   ABR,      a10t, alpha11, a12t,
                           A20,  a21,     A22;

  FLA_Obj pT,              p0,
          pB,              pi1,
                           p2;

  FLA_Obj AB0, aB1, AB2;


  FLA_Part_2x2( A,    &ATL, &ATR,
                      &ABL, &ABR,     0, 0, FLA_TL );

  FLA_Part_2x1( p,    &pT, 
                      &pB,            0, FLA_TOP );

  while ( FLA_Obj_length( ATL ) < FLA_Obj_length( A ) &&
          FLA_Obj_width( ATL ) < FLA_Obj_width( A )){

    FLA_Repart_2x2_to_3x3( ATL, /**/ ATR,       &A00,  /**/ &a01,     &A02,
                        /* ************* */   /* ************************** */
                                                &a10t, /**/ &alpha11, &a12t,
                           ABL, /**/ ABR,       &A20,  /**/ &a21,     &A22,
                           1, 1, FLA_BR );

    FLA_Repart_2x1_to_3x1( pT,                &p0, 
                        /* ** */            /* *** */
                                              &pi1, 
                           pB,                &p2,        1, FLA_BOTTOM );

    /*------------------------------------------------------------*/

    // alpha11 = alpha11 - a10t * a01
    FLA_Dots_external( FLA_MINUS_ONE, a10t, a01, FLA_ONE, alpha11 );

    // a21 = a21 - A20 * a01
    FLA_Gemv_external( FLA_NO_TRANSPOSE, FLA_MINUS_ONE, A20, a01, FLA_ONE, a21 );

    // aB1 = / alpha11 \
    //       \  a21    /
    FLA_Merge_2x1( alpha11,
                   a21,      &aB1 );

    // Determine pivot index
    FLA_Amax_external( aB1, pi1 );

    // Apply pivots to current column
    FLA_Apply_pivots( FLA_LEFT, FLA_NO_TRANSPOSE, pi1, aB1 );

    // AB0 = / a10t \
    //       \ A20  /
    FLA_Merge_2x1( a10t,
                   A20,      &AB0 );

    // AB2 = / a12t \
    //       \ A22  /
    FLA_Merge_2x1( a12t,
                   A22,      &AB2 );

    // Apply pivots to remaining columns
    FLA_Apply_pivots( FLA_LEFT, FLA_NO_TRANSPOSE, pi1, AB0 );
    FLA_Apply_pivots( FLA_LEFT, FLA_NO_TRANSPOSE, pi1, AB2 );

    // a21 = a21 / alpha11 
    FLA_Inv_scal_external( alpha11, a21 );

    // a12t = a12t - a10t * A02
    FLA_Gemv_external( FLA_TRANSPOSE, FLA_MINUS_ONE, A02, a10t, FLA_ONE, a12t );

    /*------------------------------------------------------------*/

    FLA_Cont_with_3x3_to_2x2( &ATL, /**/ &ATR,       A00,  a01,     /**/ A02,
                                                     a10t, alpha11, /**/ a12t,
                            /* ************** */  /* ************************ */
                              &ABL, /**/ &ABR,       A20,  a21,     /**/ A22,
                              FLA_TL );

    FLA_Cont_with_3x1_to_2x1( &pT,                p0, 
                                                  pi1, 
                            /* ** */           /* *** */
                              &pB,                p2,     FLA_TOP );

  }

  return FLA_SUCCESS;
}

#endif
