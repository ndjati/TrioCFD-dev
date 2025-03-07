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

#ifndef OpDiffTurbIJK_H
#define OpDiffTurbIJK_H
#include <Operateur_IJK_base.h>
#include <Boundary_Conditions.h>

/*
 * Options for CASE
 *
 *          Yes_Turb: the flux is turbulent_mu * (grad u + grad^T u)  -  2/3 * k  +  molecular_mu * grad u)'
 *          Yes_M_Grad: the flux is 'molecular_mu * grad u'
 *          Yes_M_Trans: the flux is 'molecular_mu * (grad u + grad^T u)'
 *          Yes_M_Div: the flux is 'molecular_mu * (grad u + grad^T u - 2/3 * div u * Id)'
 *          Yes_M_GradAnisotropic: the flux is 'molecular_mu^a * grad^a u' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_TransAnisotropic: the flux is 'molecular_mu^a * (grad^a u + grad^a^T u)' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_DivAnisotropic: the flux is 'molecular_mu^a * (grad^a u + grad^a^T u - 2/3 * div^a u * Id)' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_GradTensorial: the flux is 'molecular_mu_tensor * grad u'
 *          Yes_M_TransTensorial: the flux is 'molecular_mu_tensor * (grad u + grad^T u)'
 *          Yes_M_DivTensorial: the flux is 'molecular_mu_tensor * (grad u + grad^T u - 2/3 * div u * Id)'
 *          Yes_M_GradTensorialAnisotropic: the flux is 'molecular_mu_tensor^a * grad^a u' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_TransTensorialAnisotropic: the flux is 'molecular_mu_tensor^a * (grad^a u + grad^a^T u)' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_DivTensorialAnisotropic: the flux is 'molecular_mu_tensor^a * (grad^a u + grad^a^T u - 2/3 * div^a u * Id)' where (grad^a)_i = Delta_i (grad)_i
 *          Yes_M_Struct: the flux is 'structural_model'
 */
class OpDiffIJKFacesGeneric_double : public Operateur_IJK_faces_base_double
{
public:
  OpDiffIJKFacesGeneric_double();

  inline virtual void initialize(const IJK_Splitting& splitting, const Boundary_Conditions& bc)
  {
    perio_k_= splitting.get_grid_geometry().get_periodic_flag(DIRECTION_K);
    channel_data_.initialize(splitting);
    ref_bc_ = bc;
  }

  inline void compute_flux_x_vx(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::X,DIRECTION::X>(resu,k_layer);
  }
  inline void compute_flux_x_vy(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::X,DIRECTION::Y>(resu,k_layer);
  }
  inline void compute_flux_x_vz(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::X,DIRECTION::Z>(resu,k_layer);
  }
  inline void compute_flux_y_vx(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Y,DIRECTION::X>(resu,k_layer);
  }
  inline void compute_flux_y_vy(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Y,DIRECTION::Y>(resu,k_layer);
  }
  inline void compute_flux_y_vz(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Y,DIRECTION::Z>(resu,k_layer);
  }
  inline void compute_flux_z_vx(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Z,DIRECTION::X>(resu,k_layer);
  }
  inline void compute_flux_z_vy(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Z,DIRECTION::Y>(resu,k_layer);
  }
  inline void compute_flux_z_vz(IJK_Field_local_double& resu, const int k_layer) override
  {
    compute_flux_<DIRECTION::Z,DIRECTION::Z>(resu,k_layer);
  }

protected:
  const IJK_Field_local_double& get_v(DIRECTION _DIR_);
  const IJK_Field_local_double& get_molecular_nu_tensor(DIRECTION _COMPO1_, DIRECTION _COMPO2_);
  const IJK_Field_local_double& get_molecular_nu();

  const IJK_Field_local_double& get_turbulent_nu();
  const IJK_Field_local_double& get_turbulent_k_energy();
  const IJK_Field_local_double& get_divergence();
  const IJK_Field_local_double& get_structural_model(DIRECTION _COMPO1_, DIRECTION _COMPO2_);

  template <DIRECTION _DIR_, DIRECTION _VCOMPO_>
  inline void compute_flux_(IJK_Field_local_double& resu, const int k_layer);

  template <DIRECTION _DIR_, DIRECTION _VCOMPO_>
  inline void flux_loop_(IJK_Field_local_double& resu, int k_layer, int top_wall = 0, int bottom_wall =  0);

  template <DIRECTION _DIR_, DIRECTION _VCOMPO_>
  inline void flux_loop_same_dir_compo_(int i, double surface, double inv_distance_DIR, double inv_distance_COMPO,
                                        const ConstIJK_double_ptr& vCOMPO_ptr, const ConstIJK_double_ptr& vDIR_ptr,
                                        const ConstIJK_double_ptr& molecular_nu, const ConstIJK_double_ptr& div_ptr,
                                        const ConstIJK_double_ptr& turbulent_nu, const ConstIJK_double_ptr& turbulent_k_energy,
                                        const ConstIJK_double_ptr& structural_model, Simd_double& flux);

  template<DIRECTION _DIR_, DIRECTION _VCOMPO_>
  inline void flux_loop_different_dir_compo_(int i, double surface, double inv_distance_DIR, double inv_distance_COMPO,
                                             int top_wall, int bottom_wall,
                                             const ConstIJK_double_ptr& vCOMPO_ptr, const ConstIJK_double_ptr& vDIR_ptr,
                                             const ConstIJK_double_ptr& molecular_nu, const ConstIJK_double_ptr& div_ptr,
                                             const ConstIJK_double_ptr& turbulent_nu, const ConstIJK_double_ptr& turbulent_k_energy,
                                             const ConstIJK_double_ptr& structural_model, Simd_double& flux);


  Operateur_IJK_data_channel channel_data_;
  REF(Boundary_Conditions) ref_bc_;
  bool perio_k_ ;

  // Pointers to input data (velocity at faces, diffustion coeffs at elements)
  const IJK_Field_local_double *vx_;
  const IJK_Field_local_double *vy_;
  const IJK_Field_local_double *vz_;

  const IJK_Field_local_double *molecular_nu_;
  const IJK_Field_local_double *turbulent_nu_;
  const IJK_Field_local_double *turbulent_k_energy_;

  const IJK_Field_local_double *divergence_;

  const IJK_Field_local_double *molecular_nu_tensor_xx_;
  const IJK_Field_local_double *molecular_nu_tensor_xy_;
  const IJK_Field_local_double *molecular_nu_tensor_xz_;
  const IJK_Field_local_double *molecular_nu_tensor_yx_;
  const IJK_Field_local_double *molecular_nu_tensor_yy_;
  const IJK_Field_local_double *molecular_nu_tensor_yz_;
  const IJK_Field_local_double *molecular_nu_tensor_zx_;
  const IJK_Field_local_double *molecular_nu_tensor_zy_;
  const IJK_Field_local_double *molecular_nu_tensor_zz_;

  const IJK_Field_local_double *structural_model_xx_;
  const IJK_Field_local_double *structural_model_xy_;
  const IJK_Field_local_double *structural_model_xz_;
  const IJK_Field_local_double *structural_model_yx_;
  const IJK_Field_local_double *structural_model_yy_;
  const IJK_Field_local_double *structural_model_yz_;
  const IJK_Field_local_double *structural_model_zx_;
  const IJK_Field_local_double *structural_model_zy_;
  const IJK_Field_local_double *structural_model_zz_;

  bool is_turb_;
  bool is_anisotropic_;
  bool with_divergence_;
  bool with_transpose_;
  bool is_tensorial_;
  bool is_structural_;

};

class OpDiffIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffIJK_double() : OpDiffIJKFacesGeneric_double() {};
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
};

class OpDiffTurbIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffTurbIJK_double() : OpDiffIJKFacesGeneric_double() { is_turb_ = true ; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                const IJK_Field_double& turbulent_nu,
                const IJK_Field_double& turbulent_k_energy,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               const IJK_Field_double& turbulent_nu,
               const IJK_Field_double& turbulent_k_energy,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};



class OpDiffStdWithLaminarTransposeIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeIJK_double() : OpDiffIJKFacesGeneric_double() { with_transpose_ = true; }

  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};

class OpDiffStdWithLaminarTransposeAndDivergenceIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeAndDivergenceIJK_double() : OpDiffIJKFacesGeneric_double() { with_divergence_ = true, with_transpose_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                const IJK_Field_double& divergence,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               const IJK_Field_double& divergence,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};

class OpDiffAnisotropicIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffAnisotropicIJK_double() : OpDiffIJKFacesGeneric_double() { is_anisotropic_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};

class OpDiffStdWithLaminarTransposeAnisotropicIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeAnisotropicIJK_double() : OpDiffIJKFacesGeneric_double() { is_anisotropic_ = true, with_transpose_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffStdWithLaminarTransposeAndDivergenceAnisotropicIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeAndDivergenceAnisotropicIJK_double(): OpDiffIJKFacesGeneric_double() { is_anisotropic_ = true, with_transpose_ = true, with_divergence_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu,
                const IJK_Field_double& divergence,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu,
               const IJK_Field_double& divergence,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};

//FixMe:: for zeroAtWall, where to put the zero and when???
class OpDiffTensorialZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffTensorialZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_tensorial_ = true ; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffStdWithLaminarTransposeTensorialZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeTensorialZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_tensorial_ = true, with_transpose_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffTensorialAnisotropicZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffTensorialAnisotropicZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_tensorial_ = true, is_anisotropic_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffStdWithLaminarTransposeAndDivergenceTensorialZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeAndDivergenceTensorialZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_tensorial_ = true, with_transpose_ = true, with_divergence_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                const IJK_Field_double& divergence,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               const IJK_Field_double& divergence,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};



class OpDiffStdWithLaminarTransposeTensorialAnisotropicZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeTensorialAnisotropicZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_anisotropic_ = true, is_tensorial_ = true, with_transpose_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffStdWithLaminarTransposeAndDivergenceTensorialAnisotropicZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStdWithLaminarTransposeAndDivergenceTensorialAnisotropicZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_anisotropic_ = true, is_tensorial_ = true, with_transpose_ = true, with_divergence_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& molecular_nu_tensor_xx,
                const IJK_Field_double& molecular_nu_tensor_xy,
                const IJK_Field_double& molecular_nu_tensor_xz,
                const IJK_Field_double& molecular_nu_tensor_yx,
                const IJK_Field_double& molecular_nu_tensor_yy,
                const IJK_Field_double& molecular_nu_tensor_yz,
                const IJK_Field_double& molecular_nu_tensor_zx,
                const IJK_Field_double& molecular_nu_tensor_zy,
                const IJK_Field_double& molecular_nu_tensor_zz,
                const IJK_Field_double& divergence,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& molecular_nu_tensor_xx,
               const IJK_Field_double& molecular_nu_tensor_xy,
               const IJK_Field_double& molecular_nu_tensor_xz,
               const IJK_Field_double& molecular_nu_tensor_yx,
               const IJK_Field_double& molecular_nu_tensor_yy,
               const IJK_Field_double& molecular_nu_tensor_yz,
               const IJK_Field_double& molecular_nu_tensor_zx,
               const IJK_Field_double& molecular_nu_tensor_zy,
               const IJK_Field_double& molecular_nu_tensor_zz,
               const IJK_Field_double& divergence,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};


class OpDiffStructuralOnlyZeroatwallIJK_double : public OpDiffIJKFacesGeneric_double
{
public:
  OpDiffStructuralOnlyZeroatwallIJK_double() : OpDiffIJKFacesGeneric_double() { is_structural_ = true; }
  void calculer(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
                const IJK_Field_double& structural_model_xx,
                const IJK_Field_double& structural_model_xy,
                const IJK_Field_double& structural_model_xz,
                const IJK_Field_double& structural_model_yx,
                const IJK_Field_double& structural_model_yy,
                const IJK_Field_double& structural_model_yz,
                const IJK_Field_double& structural_model_zx,
                const IJK_Field_double& structural_model_zy,
                const IJK_Field_double& structural_model_zz,
                IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);
  void ajouter(const IJK_Field_double& vx, const IJK_Field_double& vy, const IJK_Field_double& vz,
               const IJK_Field_double& structural_model_xx,
               const IJK_Field_double& structural_model_xy,
               const IJK_Field_double& structural_model_xz,
               const IJK_Field_double& structural_model_yx,
               const IJK_Field_double& structural_model_yy,
               const IJK_Field_double& structural_model_yz,
               const IJK_Field_double& structural_model_zx,
               const IJK_Field_double& structural_model_zy,
               const IJK_Field_double& structural_model_zz,
               IJK_Field_double& dvx, IJK_Field_double& dvy, IJK_Field_double& dvz);

};

#include <OpDiffTurbIJK.tpp>


#endif
