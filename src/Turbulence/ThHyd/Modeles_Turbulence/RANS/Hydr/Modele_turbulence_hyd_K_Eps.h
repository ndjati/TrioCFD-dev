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
// File:        Modele_turbulence_hyd_K_Eps.h
// Directory:   $TURBULENCE_ROOT/src/ThHyd/Modeles_Turbulence/RANS/Hydr
//
//////////////////////////////////////////////////////////////////////////////

#ifndef Modele_turbulence_hyd_K_Eps_included
#define Modele_turbulence_hyd_K_Eps_included

#include <Transport_K_Eps.h>
#include <Modele_Fonc_Bas_Reynolds.h>
/*! @brief Classe Modele_turbulence_hyd_K_Eps Cette classe represente le modele de turbulence (k,eps) pour les
 *
 *     equations de Navier-Stokes.
 *
 * @sa Mod_turb_hyd_base Mod_turb_hyd_ss_maille
 */
class Modele_turbulence_hyd_K_Eps : public Mod_turb_hyd_RANS
{

  Declare_instanciable(Modele_turbulence_hyd_K_Eps);

public:

  void set_param(Param& param) override;
  int lire_motcle_non_standard(const Motcle&, Entree&) override;
  int preparer_calcul() override;
  void verifie_loi_paroi() override;
  bool initTimeStep(double dt) override;
  void mettre_a_jour(double ) override;
  virtual inline Champ_Inc& K_Eps();
  virtual inline const Champ_Inc& K_Eps() const;

  inline int nombre_d_equations() const override;
  inline Transport_K_Eps_base& eqn_transp_K_Eps() override;
  inline const Transport_K_Eps_base& eqn_transp_K_Eps() const override;
  const Equation_base& equation_k_eps(int) const override ;



  const Champ_base& get_champ(const Motcle& nom) const override;
  void get_noms_champs_postraitables(Noms& nom,Option opt=NONE) const override;

protected:
  Transport_K_Eps  eqn_transport_K_Eps;
  virtual Champ_Fonc& calculer_viscosite_turbulente(double temps);

};


/*! @brief Renvoie le champ inconnue du modele de turbulence i.
 *
 * e. : (K,Epsilon). Cette inconnue est portee
 *     par l'equation de transport K-eps porte par le modele.
 *     (version const)
 *
 * @return (Champ_Inc&) le champ inconnue (K,epsilon)
 */
inline const Champ_Inc& Modele_turbulence_hyd_K_Eps::K_Eps() const
{
  return eqn_transport_K_Eps.inconnue();
}


/*! @brief Renvoie le champ inconnue du modele de turbulence i.
 *
 * e. : (K,Epsilon). Cette inconnue est portee
 *     par l'equation de transport K-eps porte par le modele.
 *
 * @return (Champ_Inc&) le champ inconnue (K,epsilon)
 */
inline Champ_Inc& Modele_turbulence_hyd_K_Eps::K_Eps()
{
  return eqn_transport_K_Eps.inconnue();
}

/*! @brief Renvoie l'equation du modele de turbulence i.
 *
 * e. : (K,Epsilon).
 *
 * @return (Transport_K_Eps&) equation (K,epsilon)
 */
inline Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps::eqn_transp_K_Eps()
{
  return eqn_transport_K_Eps;
}

/*! @brief Renvoie l'equation du modele de turbulence i.
 *
 * e. : (K,Epsilon).
 *     (version const)
 *
 * @return (Transport_K_Eps&) equation (K,epsilon)
 */
inline const Transport_K_Eps_base& Modele_turbulence_hyd_K_Eps::eqn_transp_K_Eps() const
{
  return eqn_transport_K_Eps;
}
inline int Modele_turbulence_hyd_K_Eps::nombre_d_equations() const
{
  return 1;
}
#endif
