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

#ifndef Paroi_frottante_loi_included
#define Paroi_frottante_loi_included

#include <TRUSTTab.h>
#include <Frottement_global_impose.h>
#include <Cond_lim_base.h>
#include <TRUST_Ref.h>

class Correlation;

/*! @brief Classe Paroi_frottante_loi Cette condition limite correspond a un flux impose pour une condition aux limites adaptative faible de l'equation de
 *
 *     transport de QDM.
 *     Le coefficient de frottement est calcule a partir de la correlation de loi de paroi adaptative.
 *
 * @sa Neumann
 */
class Paroi_frottante_loi : public Frottement_global_impose
{

  Declare_instanciable(Paroi_frottante_loi);

public :
  void completer() override ;
  virtual int initialiser(double temps) override;
  virtual int avancer(double temps) override {return 1;}; // Avancer ne fait rien car le champ est modifie dans mettre_a_jour
  void mettre_a_jour(double tps) override;
  void me_calculer();
  double calc_flux(double y, double u_tau, double visc);
  virtual double coefficient_frottement(int i) const override;
  virtual double coefficient_frottement(int i,int j) const override;
  virtual double coefficient_frottement_grad(int i) const override;
  virtual double coefficient_frottement_grad(int i,int j) const override;
  virtual void liste_faces_loi_paroi(IntTab&) override;

protected :

  REF(Correlation) correlation_loi_paroi_;
  double deriv_u_plus_de_y_plus(double y_p);

  DoubleTab valeurs_coeff_;
  DoubleTab valeurs_coeff_grad_;

  double von_karman_ = 0.41 ;
  double beta_omega = 0.075;
  double beta_k = 0.09;

  // Initialized at 0. and adapted in the readOn depending on turbulence selected
  double y_p_prod_k_ = -1.e8 ;
  double fac_prod_k_ = -1.e8 ;
  double y_p_prod_k_grand_ = -1.e8 ;
  double fac_prod_k_grand_ = -1.e8 ;
};

#endif
