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
// File:        Paroi_std_scal_hyd_VDF.h
// Directory:   $TURBULENCE_ROOT/src/Specializations/VDF/Lois_Paroi/Scal
//
//////////////////////////////////////////////////////////////////////////////


#ifndef Paroi_std_scal_hyd_VDF_included
#define Paroi_std_scal_hyd_VDF_included

#include <Paroi_scal_hyd_base_VDF.h>
#include <Table.h>

/*! @brief classe Paroi_std_scal_hyd_VDF
 *
 *
 *
 * .SECTION  voir aussi
 *   Paroi_std_hyd_VDF
 *
 */
class Paroi_std_scal_hyd_VDF : public Paroi_scal_hyd_base_VDF
{

  Declare_instanciable_sans_constructeur(Paroi_std_scal_hyd_VDF);

public:
  int init_lois_paroi() override;
  int calculer_scal(Champ_Fonc_base& ) override ;

protected:
  int init_lois_paroi_scalaire();
};

/*! @brief cette classe permet de specifier des options a la loi de paroi standard.
 *
 * Elle est reservee aux experts.
 *
 */
class Loi_expert_scalaire_VDF : public Paroi_std_scal_hyd_VDF
{
  Declare_instanciable(Loi_expert_scalaire_VDF);
};

#endif

