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
// File:        Paroi_scal_hyd_base_VEF.h
// Directory:   $TURBULENCE_ROOT/src/Specializations/VEF/Lois_Paroi/Scal
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Paroi_scal_hyd_base_VEF_included
#define Paroi_scal_hyd_base_VEF_included

#include <Turbulence_paroi_scal_base.h>
#include <TRUSTVects.h>
#include <TRUST_Ref.h>

class Domaine_Cl_VEF;
class Domaine_VEF;
//
class Paroi_scal_hyd_base_VEF  : public Turbulence_paroi_scal_base
{

  Declare_base(Paroi_scal_hyd_base_VEF);

public:
  void associer(const Domaine_dis& ,const Domaine_Cl_dis& ) override;
  int init_lois_paroi() override;
  void imprimer_nusselt(Sortie&) const override;

  inline const DoubleVect& tab_d_reel() const;
  inline DoubleVect& tab_d_reel();
  DoubleVect& equivalent_distance_name(DoubleVect& d_eq, const Nom& nom_bord) const override ;
protected :
  REF(Domaine_VEF) le_dom_VEF;
  REF(Domaine_Cl_VEF) le_dom_Cl_VEF;

  DoubleVect tab_d_reel_;   // tableau des distances (dimenssionnelles) du
  // point y+ = 1

};


#endif

