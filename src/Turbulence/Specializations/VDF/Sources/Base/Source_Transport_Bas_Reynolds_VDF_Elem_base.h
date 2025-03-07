/****************************************************************************
* Copyright (c) 2022, CEA
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
/////////////////////////////////////////////////////////////////////////////
//
// File      : Source_Transport_Bas_Reynolds_VDF_Elem_base.h
// Directory : $TURBULENCE_ROOT/src/Specializations/VDF/Sources/new
//
/////////////////////////////////////////////////////////////////////////////

#ifndef Source_Transport_Bas_Reynolds_VDF_Elem_base_included
#define Source_Transport_Bas_Reynolds_VDF_Elem_base_included

#include <Source_Transport_VDF_Elem_base.h>
#include <TRUST_Ref.h>

class Transport_K_Eps_Bas_Reynolds;

class Source_Transport_Bas_Reynolds_VDF_Elem_base : public Source_Transport_VDF_Elem_base
{
  Declare_base_sans_constructeur( Source_Transport_Bas_Reynolds_VDF_Elem_base ) ;
public :
  Source_Transport_Bas_Reynolds_VDF_Elem_base(double cs1, double cs2) : Source_Transport_VDF_Elem_base(cs1,cs2) { }
  void ajouter_blocs(matrices_t matrices, DoubleTab& secmem, const tabs_t& semi_impl) const override;
  void associer_pb(const Probleme_base& ) override;

protected :
  REF(Transport_K_Eps_Bas_Reynolds) eqn_keps_bas_re;

private:
  virtual void fill_resu_bas_reyn(const DoubleTrav& , const DoubleTrav& , const DoubleTrav& , const DoubleTrav& , const DoubleTrav& , DoubleTab& ) const { return not_implemented<void>(__func__); }
};

#endif /* Source_Transport_Bas_Reynolds_VDF_Elem_base_included */
