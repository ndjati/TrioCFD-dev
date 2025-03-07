/****************************************************************************
* Copyright (c) 2021, CEA
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
// File:        Loi_paroi_Ramstorfer.h
// Directory:   $TRUST_ROOT/src/Turbulence/Correlations
// Version:     /main/18
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Loi_paroi_Ramstorfer_included
#define Loi_paroi_Ramstorfer_included
#include <TRUSTTab.h>
#include <TRUSTTab.h>
#include <Correlation_base.h>
#include <Loi_paroi_base.h>
#include <vector>
#include <map>
#include <string>

/*! @brief classe Loi_paroi_Ramstorfer correlation pour une loi de paroi adaptative qui calcule u_tau et du y_plus
 *
 *     Methodes implementees :
 *
 *
 */
class Loi_paroi_Ramstorfer : public Loi_paroi_base
{
  Declare_instanciable(Loi_paroi_Ramstorfer);
public:
  void calc_u_tau_y_plus(const DoubleTab& vit, const DoubleTab& nu_visc) override ;
  void completer() override;
  void mettre_a_jour(double temps) override;
  DoubleTab& get_tab(std::string str) {return valeurs_loi_paroi_[str];};
  double get_y(int f) const {return valeurs_loi_paroi_.at("y")(f,0);};
  double get_utau(int f) const { return valeurs_loi_paroi_.at("u_tau")(f,0);};
  double get_dyp_u_plus(int f) const { return valeurs_loi_paroi_.at("dyp_u_plus")(f,0);};
  double calc_u_tau_loc(double u_par, double nu, double y, const double *d_bulles, const double *alpha);
  int    a_calculer(int f) {return Faces_a_calculer_(f, 0);};

protected:
  double u_plus_de_y_plus(double u_tau, double nu, double y, const double *d_bulles, const double *alpha); // Blended Reichardt model
  double deriv_u_plus_de_y_plus(double u_tau, double nu, double y, const double *d_bulles, const double *alpha); // Blended Reichardt model
  double to_zero(double u_tau, double u_par, double nu, double y, const double *d_bulles, const double *alpha); // fonction for which we are looking for the root
  double d_to_zero(double u_tau, double u_par, double nu, double y, const double *d_bulles, const double *alpha);

  double von_karman_ = 0.41;
  double limiteur_y_p = 0.01; // To prevent numerical issues ; no consequence on the calculation, as it falls in the region where the blending function is zero
  double C_kr_ = .5;

  double eps_u_tau_ = 1.e-10;

  IntTab Faces_a_calculer_;
  std::map<std::string, DoubleTab> valeurs_loi_paroi_; // contient "y_plus", "u_plus", "dyp_u_plus", "u_tau" pour toutes les faces
};

#endif
