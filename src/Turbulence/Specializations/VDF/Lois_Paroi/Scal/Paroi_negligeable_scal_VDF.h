/****************************************************************************
* Copyright (c) 2015 - 2016, CEA
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
* 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
* 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
* IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*****************************************************************************/
//////////////////////////////////////////////////////////////////////////////
//
// File:        Paroi_negligeable_scal_VDF.h
// Directory:   $TURBULENCE_ROOT/src/Specializations/VDF/Lois_Paroi/Scal
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Paroi_negligeable_scal_VDF_included
#define Paroi_negligeable_scal_VDF_included

#include <Paroi_scal_hyd_base_VDF.h>

/*! @brief classe Paroi_negligeable_scal_VDF
 *
 *
 *
 * .SECTION  voir aussi
 *   Paroi_negligeable_VDF
 *
 */
class Paroi_negligeable_scal_VDF : public Paroi_scal_hyd_base_VDF
{
  Declare_instanciable(Paroi_negligeable_scal_VDF);

public:
  /* virtual int init_lois_paroi(); */
  int calculer_scal(Champ_Fonc_base& ) override ;
  bool use_equivalent_distance() const override;


};


#endif

