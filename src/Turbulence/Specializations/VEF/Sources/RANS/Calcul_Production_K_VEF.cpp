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
// File:        Calcul_Production_K_VEF.cpp
// Directory:   $TURBULENCE_ROOT/src/Specializations/VEF/Sources
//
//////////////////////////////////////////////////////////////////////////////

#include <Calcul_Production_K_VEF.h>
#include <TRUSTTab.h>
#include <Domaine_VEF.h>
#include <Domaine_Cl_VEF.h>
#include <TRUSTVect.h>
#include <TRUSTTrav.h>
#include <Champ_P1NC.h>
#include <Periodique.h>
#include <Champ_Uniforme.h>
#include <Convection_Diffusion_Concentration.h>

////////////////////////////////////////////////////////////////////////////
//
//    Implementation des fonctions de la classe Calcul_Production_K_VEF
//
/////////////////////////////////////////////////////////////////////////////

DoubleTab& Calcul_Production_K_VEF::
calculer_terme_production_K(const Domaine_VEF& domaine_VEF,const Domaine_Cl_VEF& zcl_VEF,
                            DoubleTab& P,const DoubleTab& K_eps,
                            const DoubleTab& vit,const DoubleTab& visco_turb, const int& interpol_visco) const
{
  // P est discretise comme K et Eps i.e au centre des faces
  //
  // P(elem) = -(2/3)*k(i)*div_U(i) + nu_t(i) * F(u,v,w)
  //
  //                          2          2          2
  //    avec F(u,v,w) = 2[(du/dx)  + (dv/dy)  + (dw/dz) ] +
  //
  //                               2               2               2
  //                  (du/dy+dv/dx) + (du/dz+dw/dx) + (dw/dy+dv/dz)
  //
  // Rqs: On se place dans le cadre incompressible donc on neglige
  //      le terme (2/3)*k(i)*div_U(i)

  P= 0;

  // Calcul de F(u,v,w):
  int nb_elem_tot = domaine_VEF.nb_elem_tot();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  int premiere_face_int = domaine_VEF.premiere_face_int();
  //  const IntTab& les_Polys = domaine.les_elems();
  int fac=0;
  int poly1, poly2;
  int nb_faces_ = domaine_VEF.nb_faces();
  int dimension=Objet_U::dimension;
  //  const DoubleTab& xp = domaine_VEF.xp();    // centre de gravite des elements
  //  const DoubleTab& xv = domaine_VEF.xv();    // centre de gravite des faces

  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);
  // (du/dx du/dy dv/dx dv/dy ...) pour un poly
  gradient_elem=0.;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////////
  //On remplace le calcul precedent par un appel a calcul_duidxj
  ////////////////////////////////////////////////////////////////////////////////

  int n_bord;
  Champ_P1NC::calcul_gradient(vit,gradient_elem,zcl_VEF);

  ///////////////////////////////////////////////
  // On a les gradient_elem par elements
  ///////////////////////////////////////////////

  double du_dx;
  double du_dy;
  double du_dz;
  double dv_dx;
  double dv_dy;
  double dv_dz;
  double dw_dx;
  double dw_dy;
  double dw_dz;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Calcul des du/dx dv/dy et des derivees croisees sur les faces de chaque elements dans le cas 2D
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Boucle sur les bords pour traiter les faces de bord
  // en distinguant le cas periodicite
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              poly2 = face_voisins(fac,1);
              double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
              double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
              double visco_face = 0.0;

              if ( interpol_visco == 0 )
                {
                  visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
                }
              else if ( interpol_visco == 1 )
                {
                  //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else if ( interpol_visco == 2 )
                {
                  //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else
                {
                  Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
                  Process::exit();
                }

              du_dx=a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
              du_dy=a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
              dv_dx=a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
              dv_dy=a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

              // Determination du terme de production

              P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy)
                                    + ((du_dy+dv_dx)*(du_dy+dv_dx) ) );

              if (dimension == 3)
                {
                  du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
                  dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
                  dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
                  dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
                  dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

                  // Determination du terme de production

                  P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                                       + (   (du_dy+dv_dx)*(du_dy+dv_dx)
                                             + (du_dz+dw_dx)*(du_dz+dw_dx)
                                             + (dw_dy+dv_dz)*(dw_dy+dv_dz) ));
                }
            }
        }
      /*   else if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           {
           for (fac=ndeb; fac<nfin; fac++)
           {
           poly1 = face_voisins(fac,0);
           // double visco_face=0.;
           //if (K_eps(fac,1)>1.e-10) visco_face=visco_turb(poly1);

           // Determination du terme de production
           P(fac) = 0.;
           }
           } */
      else
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              double visco_face;
              visco_face=visco_turb(poly1);
              // Cerr << "visco_face autres parois " << visco_face << finl;

              du_dx=gradient_elem(poly1,0,0);
              du_dy=gradient_elem(poly1,0,1);
              dv_dx=gradient_elem(poly1,1,0);
              dv_dy=gradient_elem(poly1,1,1);

              // Determination du terme de production
              P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));


              if (dimension == 3)
                {
                  du_dz=gradient_elem(poly1,0,2);
                  dv_dz=gradient_elem(poly1,1,2);
                  dw_dx=gradient_elem(poly1,2,0);
                  dw_dy=gradient_elem(poly1,2,1);
                  dw_dz=gradient_elem(poly1,2,2);

                  // Determination du terme de production

                  P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                                       + ( (du_dy+dv_dx)*(du_dy+dv_dx)
                                           + (du_dz+dw_dx)*(du_dz+dw_dx)
                                           + (dw_dy+dv_dz)*(dw_dy+dv_dz)));

                }
            }
        }
    }

  // Traitement des faces internes

  for (fac = premiere_face_int; fac<nb_faces_; fac++)
    {
      poly1 = face_voisins(fac,0);
      poly2 = face_voisins(fac,1);
      double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
      double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
      double visco_face = 0.0;

      if ( interpol_visco == 0 )
        {
          visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
        }
      else if ( interpol_visco == 1 )
        {
          //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else if ( interpol_visco == 2 )
        {
          //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else
        {
          Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
          Process::exit();
        }

      du_dx=a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
      du_dy=a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
      dv_dx=a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
      dv_dy=a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

      // Determination du terme de production
      P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));
      if (dimension == 3)
        {
          du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
          dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
          dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
          dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
          dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

          // Determination du terme de production

          P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                               + (   (du_dy+dv_dx)*(du_dy+dv_dx)
                                     + (du_dz+dw_dx)*(du_dz+dw_dx)
                                     + (dw_dy+dv_dz)*(dw_dy+dv_dz) ));
        }
    }
  return P;
}


DoubleTab& Calcul_Production_K_VEF::
calculer_terme_production_K_BiK(const Domaine_VEF& domaine_VEF,const Domaine_Cl_VEF& zcl_VEF,
                                DoubleTab& P,const DoubleTab& K,const DoubleTab& eps,
                                const DoubleTab& vit,const DoubleTab& visco_turb, const int& interpol_visco) const
{
  // P est discretise comme K et Eps i.e au centre des faces
  //
  // P(elem) = -(2/3)*k(i)*div_U(i) + nu_t(i) * F(u,v,w)
  //
  //                          2          2          2
  //    avec F(u,v,w) = 2[(du/dx)  + (dv/dy)  + (dw/dz) ] +
  //
  //                               2               2               2
  //                  (du/dy+dv/dx) + (du/dz+dw/dx) + (dw/dy+dv/dz)
  //
  // Rqs: On se place dans le cadre incompressible donc on neglige
  //      le terme (2/3)*k(i)*div_U(i)

  P= 0;

  // Calcul de F(u,v,w):
  int nb_elem_tot = domaine_VEF.nb_elem_tot();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes = domaine_VEF.volumes();
  int premiere_face_int = domaine_VEF.premiere_face_int();
  //  const IntTab& les_Polys = domaine.les_elems();
  int fac=0;
  int poly1, poly2;
  int nb_faces_ = domaine_VEF.nb_faces();
  int dimension=Objet_U::dimension;
  //  const DoubleTab& xp = domaine_VEF.xp();    // centre de gravite des elements
  //  const DoubleTab& xv = domaine_VEF.xv();    // centre de gravite des faces

  DoubleTab gradient_elem(nb_elem_tot,dimension,dimension);
  // (du/dx du/dy dv/dx dv/dy ...) pour un poly
  gradient_elem=0.;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////////
  //On remplace le calcul precedent par un appel a calcul_duidxj
  ////////////////////////////////////////////////////////////////////////////////

  int n_bord;
  Champ_P1NC::calcul_gradient(vit,gradient_elem,zcl_VEF);

  ///////////////////////////////////////////////
  // On a les gradient_elem par elements
  ///////////////////////////////////////////////

  double du_dx;
  double du_dy;
  double du_dz;
  double dv_dx;
  double dv_dy;
  double dv_dz;
  double dw_dx;
  double dw_dy;
  double dw_dz;

  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Calcul des du/dx dv/dy et des derivees croisees sur les faces de chaque elements dans le cas 2D
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Boucle sur les bords pour traiter les faces de bord
  // en distinguant le cas periodicite
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              poly2 = face_voisins(fac,1);
              double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
              double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
              double visco_face = 0.0;

              if ( interpol_visco == 0 )
                {
                  visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
                }
              else if ( interpol_visco == 1 )
                {
                  //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else if ( interpol_visco == 2 )
                {
                  //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else
                {
                  Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
                  Process::exit();
                }

              du_dx=a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
              du_dy=a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
              dv_dx=a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
              dv_dy=a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

              // Determination du terme de production

              P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy)
                                    + ((du_dy+dv_dx)*(du_dy+dv_dx) ) );

              if (dimension == 3)
                {
                  du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
                  dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
                  dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
                  dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
                  dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

                  // Determination du terme de production

                  P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                                       + (   (du_dy+dv_dx)*(du_dy+dv_dx)
                                             + (du_dz+dw_dx)*(du_dz+dw_dx)
                                             + (dw_dy+dv_dz)*(dw_dy+dv_dz) ));
                }
            }
        }
      /*   else if (sub_type(Dirichlet_paroi_fixe,la_cl.valeur()))
           {
           for (fac=ndeb; fac<nfin; fac++)
           {
           poly1 = face_voisins(fac,0);
           // double visco_face=0.;
           //if (K_eps(fac,1)>1.e-10) visco_face=visco_turb(poly1);

           // Determination du terme de production
           P(fac) = 0.;
           }
           } */
      else
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              double visco_face;
              visco_face=visco_turb(poly1);
              // Cerr << "visco_face autres parois " << visco_face << finl;

              du_dx=gradient_elem(poly1,0,0);
              du_dy=gradient_elem(poly1,0,1);
              dv_dx=gradient_elem(poly1,1,0);
              dv_dy=gradient_elem(poly1,1,1);

              // Determination du terme de production
              P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));


              if (dimension == 3)
                {
                  du_dz=gradient_elem(poly1,0,2);
                  dv_dz=gradient_elem(poly1,1,2);
                  dw_dx=gradient_elem(poly1,2,0);
                  dw_dy=gradient_elem(poly1,2,1);
                  dw_dz=gradient_elem(poly1,2,2);

                  // Determination du terme de production

                  P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                                       + ( (du_dy+dv_dx)*(du_dy+dv_dx)
                                           + (du_dz+dw_dx)*(du_dz+dw_dx)
                                           + (dw_dy+dv_dz)*(dw_dy+dv_dz)));

                }
            }
        }
    }

  // Traitement des faces internes

  for (fac = premiere_face_int; fac<nb_faces_; fac++)
    {
      poly1 = face_voisins(fac,0);
      poly2 = face_voisins(fac,1);
      double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
      double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
      double visco_face = 0.0;

      if ( interpol_visco == 0 )
        {
          visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
        }
      else if ( interpol_visco == 1 )
        {
          //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else if ( interpol_visco == 2 )
        {
          //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else
        {
          Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
          Process::exit();
        }

      du_dx=a*gradient_elem(poly1,0,0) + b*gradient_elem(poly2,0,0);
      du_dy=a*gradient_elem(poly1,0,1) + b*gradient_elem(poly2,0,1);
      dv_dx=a*gradient_elem(poly1,1,0) + b*gradient_elem(poly2,1,0);
      dv_dy=a*gradient_elem(poly1,1,1) + b*gradient_elem(poly2,1,1);

      // Determination du terme de production
      P(fac) = visco_face*( 2*(du_dx*du_dx + dv_dy*dv_dy) + ((du_dy+dv_dx)*(du_dy+dv_dx)));
      if (dimension == 3)
        {
          du_dz=a*gradient_elem(poly1,0,2) + b*gradient_elem(poly2,0,2);
          dv_dz=a*gradient_elem(poly1,1,2) + b*gradient_elem(poly2,1,2);
          dw_dx=a*gradient_elem(poly1,2,0) + b*gradient_elem(poly2,2,0);
          dw_dy=a*gradient_elem(poly1,2,1) + b*gradient_elem(poly2,2,1);
          dw_dz=a*gradient_elem(poly1,2,2) + b*gradient_elem(poly2,2,2);

          // Determination du terme de production

          P(fac) = visco_face*(2*( du_dx*du_dx + dv_dy*dv_dy + dw_dz*dw_dz )
                               + (   (du_dy+dv_dx)*(du_dy+dv_dx)
                                     + (du_dz+dw_dx)*(du_dz+dw_dx)
                                     + (dw_dy+dv_dz)*(dw_dy+dv_dz) ));
        }
    }
  return P;
}

DoubleTab& Calcul_Production_K_VEF::
calculer_terme_production_K_EASM(const Domaine_VEF& domaine_VEF,const Domaine_Cl_VEF& zcl_VEF,
                                 DoubleTab& P,const DoubleTab& K_eps,
                                 const DoubleTab& gradient_elem,const DoubleTab& visco_turb,const DoubleTab& Re, const int& interpol_visco) const
{
  //Cerr << "Calcul_Production_K_VEF::calculer_terme_production_K_EASM" << finl;

  // P : Production
  P= 0;

  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int premiere_face_int = domaine_VEF.premiere_face_int();

  const DoubleVect& volumes = domaine_VEF.volumes();

  int fac=0;
  int poly1, poly2;
  int nb_faces_ = domaine_VEF.nb_faces();
  int dimension=Objet_U::dimension;

  ///////////////////////////////////////////////////////////////////////////////////////////////
  //                        <
  // calcul des gradients;  < [ Ujp*np/vol(j) ]
  //                         j
  ////////////////////////////////////////////////////////////////////////////////////////////////


  ////////////////////////////////////////////////////////////////////////////////
  //On remplace le calcul precedent par un appel a calcul_duidxj
  ////////////////////////////////////////////////////////////////////////////////

  int n_bord;

  DoubleTab gradient_face(nb_faces_,dimension,dimension);
  calcul_tenseur_face(gradient_face,gradient_elem,domaine_VEF,zcl_VEF);
  DoubleTab Re_face(nb_faces_,dimension,dimension);
  calcul_tenseur_face(Re_face,Re,domaine_VEF,zcl_VEF);
  //gradient_face.echange_espace_virtuel();
  //Re_face.echange_espace_virtuel();
  /////////////////////////////////////////////////////////////////////////////////////////////////////////
  // Calcul des du/dx dv/dy et des derivees croisees sur les faces de chaque elements dans le cas 2D
  /////////////////////////////////////////////////////////////////////////////////////////////////////////

  // Boucle sur les bords pour traiter les faces de bord
  // en distinguant le cas periodicite
  for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
    {
      const Cond_lim& la_cl = zcl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              poly2 = face_voisins(fac,1);
              double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
              double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
              double visco_face = 0.0;

              if ( interpol_visco == 0 )
                {
                  visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
                }
              else if ( interpol_visco == 1 )
                {
                  //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else if ( interpol_visco == 2 )
                {
                  //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
                  if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
                    visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
                  // TODO : what happens if visco_turb < 1.0e-10 ??
                }
              else
                {
                  Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
                  Process::exit();
                }

              // Determination du terme de production
              for (int i=0; i<dimension; i++)
                for (int j=0; j<dimension; j++)
                  P(fac) += Re_face(fac,i,j)*gradient_face(fac,i,j);
              P(fac) *= visco_face;

            }
        }
      else
        {
          for (fac=ndeb; fac<nfin; fac++)
            {
              poly1 = face_voisins(fac,0);
              double visco_face;
              visco_face=visco_turb(poly1);

              // Determination du terme de production
              for (int i=0; i<dimension; i++)
                for (int j=0; j<dimension; j++)
                  P(fac) += Re_face(fac,i,j)*gradient_face(fac,i,j);
              P(fac) *= visco_face;

            }
        }
    }

  // Traitement des faces internes
  for (fac = premiere_face_int; fac<nb_faces_; fac++)
    {
      poly1 = face_voisins(fac,0);
      poly2 = face_voisins(fac,1);
      double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
      double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
      double visco_face = 0.0;

      if ( interpol_visco == 0 )
        {
          visco_face=0.5*(visco_turb(poly1)+visco_turb(poly2));
        }
      else if ( interpol_visco == 1 )
        {
          //Moyenne harmonique (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=1./(1./visco_turb(poly1)+1./visco_turb(poly2));
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else if ( interpol_visco == 2 )
        {
          //Moyenne harmonique ponderee pour garantir la continuite du tenseur des contraintes a la face (uniquement utilisee dans le cas du keps realisable) :
          if (visco_turb(poly1) > 1.e-10 && visco_turb(poly2) > 1.e-10)
            visco_face=( visco_turb(poly1)*visco_turb(poly2) )/( b*visco_turb(poly1)+a*visco_turb(poly2) );
          // TODO : what happens if visco_turb < 1.0e-10 ??
        }
      else
        {
          Cerr << "Error in 'interpol_visco' value :" << interpol_visco << finl;
          Process::exit();
        }

      // Determination du terme de production
      for (int i=0; i<dimension; i++)
        for (int j=0; j<dimension; j++)
          P(fac) +=  Re_face(fac,i,j)*gradient_face(fac,i,j);
      P(fac) *= visco_face;

      // Pour Verification -----------------------------------------------------------------
      /*Cerr << "face " << fac << " gradient_elem1 --> " << gradient_elem(poly1,0,0) << " "
      <<  gradient_elem(poly1,0,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " gradient_elem1 --> " << gradient_elem(poly1,1,0) << " "
      <<  gradient_elem(poly1,1,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " visco1 --> " << visco_turb[poly1]
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " Re1 --> " << Re(poly1,0,0) << " " <<  Re(poly1,0,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " Re1 --> " << Re(poly1,1,0) << " " <<  Re(poly1,1,1)
      	   << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " gradient_elem2 --> " << gradient_elem(poly2,0,0) << " "
      <<  gradient_elem(poly2,0,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " gradient_elem2 --> " << gradient_elem(poly2,1,0) << " "
      <<  gradient_elem(poly2,1,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " visco2 --> " << visco_turb[poly2]
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " Re2 --> " << Re(poly2,0,0) << " " <<  Re(poly2,0,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " Re2 --> " << Re(poly2,1,0) << " " <<  Re(poly2,1,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " Re2 --> " << Re(poly2,1,0) << " " <<  Re(poly2,1,1)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << "face " << fac << " K = " << K_eps(fac,0) << " , Eps = " << K_eps(fac,1)
      << finl;
      Cerr << "face " << fac << " --> P = " << P(fac)
      << " " << domaine_VEF.xv(fac,0) << " " << domaine_VEF.xv(fac,1) << finl;
      Cerr << finl;*/

    }
  return P;
}

DoubleTab& Calcul_Production_K_VEF::calculer_terme_destruction_K_gen(
  const Domaine_VEF& domaine_VEF,
  const Domaine_Cl_VEF& zcl_VEF,
  DoubleTab& G,
  const DoubleTab& inconnue,
  const DoubleTab& alpha_turb,
  const Champ_Don& ch_beta,
  const DoubleVect& gravite,
  int nb_consti ) const
{
  // G est discretise comme K et Eps i.e au centre des faces
  // G(face) = beta alpha_t(face) G . gradT(face)

  int nb_elem_tot            = domaine_VEF.nb_elem_tot();
  int nb_faces_              = domaine_VEF.nb_faces();
  int nb_faces_tot           = domaine_VEF.nb_faces_tot();
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  const DoubleVect& volumes  = domaine_VEF.volumes();
  int elem1,elem2,fac=0;
  int n_bord;
  int nb_compo=0;

  int dimension=Objet_U::dimension;
  DoubleVect coef(Objet_U::dimension);

  const DoubleTab& tab_beta = ch_beta.valeurs();
  G = 0;

  if ( ! sub_type(Champ_Uniforme,ch_beta.valeur()) )
    {
      nb_compo=ch_beta.nb_comp() ;
    }

  if (nb_consti==0 || nb_consti==1)
    {
      DoubleTrav u_teta(nb_faces_tot,dimension);
      DoubleTrav gradient_elem(nb_elem_tot,dimension);
      u_teta=0;
      gradient_elem=0;
      // Calcul du gradient de l'inconnue
      Champ_P1NC::calcul_gradient(inconnue,gradient_elem,zcl_VEF);

      // Calcul de u_teta :
      if ( nb_compo==0 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<dimension; i++)
                        {
                          u_teta(fac,i) = a*tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                          + b*tab_beta(0,0)*alpha_turb(elem2)*gradient_elem(elem2,i);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<dimension; i++)
                        u_teta(fac,i)=tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i);
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<dimension; i++)
                    u_teta(fac,i) = a*tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                    + b*tab_beta(0,0)*alpha_turb(elem2)*gradient_elem(elem2,i);
                }
            }
        }
      else if ( nb_compo==1 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<dimension; i++)
                        {
                          u_teta(fac,i) = a*tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                          + b*tab_beta(elem2)*alpha_turb(elem2)*gradient_elem(elem2,i);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<dimension; i++)
                        u_teta(fac,i)=tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i);
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<dimension; i++)
                    u_teta(fac,i) = a*tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                    + b*tab_beta(elem2)*alpha_turb(elem2)*gradient_elem(elem2,i);
                }
            }
        }
      else if ( nb_compo>1 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<dimension; i++)
                        {
                          u_teta(fac,i) = a*tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                          + b*tab_beta(elem2,0)*alpha_turb(elem2)*gradient_elem(elem2,i);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<dimension; i++)
                        u_teta(fac,i)=tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i);
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<dimension; i++)
                    u_teta(fac,i) = a*tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i)
                                    + b*tab_beta(elem2,0)*alpha_turb(elem2)*gradient_elem(elem2,i);
                }
            }
        }
      else
        {
          Cerr << "ERROR: the value of nb_compo is negative !" << finl;
          Process::exit();
        }

      // Calcul de gravite . u_teta
      for (fac=0; fac< nb_faces_; fac++)
        {
          if (dimension == 2)
            G[fac] = gravite(0)*u_teta(fac,0) + gravite(1)*u_teta(fac,1);
          else if (dimension == 3)
            G[fac] = gravite(0)*u_teta(fac,0) + gravite(1)*u_teta(fac,1) + gravite(2)*u_teta(fac,2);
          else
            Process::exit();
        }

    }


  else if (nb_consti>1)
    {
      DoubleTrav u_teta(nb_faces_tot,nb_consti,dimension);
      DoubleTrav gradient_elem(nb_elem_tot,nb_consti,dimension);
      u_teta=0;
      gradient_elem=0;
      // Calcul du gradient de l'inconnue
      Champ_P1NC::calcul_gradient(inconnue,gradient_elem,zcl_VEF);

      if ( nb_compo==0 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k) = a*tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                              + b*tab_beta(0,0)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k)=tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k);
                        }
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<nb_consti; i++)
                    {
                      for (int k=0; k<dimension; k++)
                        u_teta(fac,i,k) = a*tab_beta(0,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                          + b*tab_beta(0,0)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                    }
                }
            }
        }
      else if ( nb_compo==1 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k) = a*tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                              + b*tab_beta(elem2)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k)=tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i,k);
                        }
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<nb_consti; i++)
                    {
                      for (int k=0; k<dimension; k++)
                        u_teta(fac,i,k) = a*tab_beta(elem1)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                          + b*tab_beta(elem2)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                    }
                }
            }
        }
      else if ( nb_compo>1 )
        {
          // we treat the boundaries
          for (n_bord=0; n_bord<domaine_VEF.nb_front_Cl(); n_bord++)
            {
              const Cond_lim& la_cl   = zcl_VEF.les_conditions_limites(n_bord);
              const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
              int ndeb                = le_bord.num_premiere_face();
              int nfin                = ndeb + le_bord.nb_faces();
              if (sub_type(Periodique,la_cl.valeur()))
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      elem2 = face_voisins(fac,1);
                      double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                      double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k) = a*tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                              + b*tab_beta(elem2,0)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                        }
                    }
                }
              else
                {
                  for (fac=ndeb; fac< nfin; fac++)
                    {
                      elem1 = face_voisins(fac,0);
                      for (int i=0; i<nb_consti; i++)
                        {
                          for (int k=0; k<dimension; k++)
                            u_teta(fac,i,k)=tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k);
                        }
                    }
                }
            }
          // we treat the internal faces
          for (; fac<nb_faces_tot; fac++)
            {
              elem1 = face_voisins(fac,0);
              elem2 = face_voisins(fac,1);
              if ((elem1>=0) && (elem2>=0))
                {
                  double a=volumes(elem1)/(volumes(elem1)+volumes(elem2));
                  double b=volumes(elem2)/(volumes(elem1)+volumes(elem2));
                  for (int i=0; i<nb_consti; i++)
                    {
                      for (int k=0; k<dimension; k++)
                        u_teta(fac,i,k) = a*tab_beta(elem1,0)*alpha_turb(elem1)*gradient_elem(elem1,i,k)
                                          + b*tab_beta(elem2,0)*alpha_turb(elem2)*gradient_elem(elem2,i,k);
                    }
                }
            }
        }
      else
        {
          Cerr << "ERROR: the value of nb_compo is negative !" << finl;
          Process::exit();
        }

      // Calcul de gravite . u_teta
      for (fac=0; fac<nb_faces_; fac++)
        {
          for (int k=0; k<nb_consti; k++)
            {
              if (dimension == 2)
                G[fac] = gravite(0)*u_teta(fac,k,0) + gravite(1)*u_teta(fac,k,1);
              else if (dimension == 3)
                G[fac] = gravite(0)*u_teta(fac,k,0) + gravite(1)*u_teta(fac,k,1) + gravite(2)*u_teta(fac,k,2);
              else
                Process::exit();
            }
        }
    }


  else
    {
      Cerr << "ERROR: the value of nb_consti is negative !" << finl;
      Process::exit();
    }

  return G;
}

// Calcul d'un tenseur aux faces a partir d'un tenseur aux elements
DoubleTab& Calcul_Production_K_VEF::calcul_tenseur_face(DoubleTab& Tenseur_face, const DoubleTab& Tenseur_elem,
                                                        const Domaine_VEF& domaine_VEF, const Domaine_Cl_VEF& domaine_Cl_VEF) const
{

  int dimension = Objet_U::dimension;
  const IntTab& face_voisins = domaine_VEF.face_voisins();
  int nb_faces = domaine_VEF.nb_faces();

  const Conds_lim& les_cl = domaine_Cl_VEF.les_conditions_limites();
  int nb_cl=les_cl.size();
  const DoubleVect& volumes = domaine_VEF.volumes();

  for (int n_bord=0; n_bord<nb_cl; n_bord++)
    {
      const Cond_lim& la_cl = domaine_Cl_VEF.les_conditions_limites(n_bord);
      const Front_VF& le_bord = ref_cast(Front_VF,la_cl.frontiere_dis());
      int ndeb = le_bord.num_premiere_face();
      int nfin = ndeb + le_bord.nb_faces();

      if (sub_type(Periodique,la_cl.valeur()))
        {
          for (int fac=ndeb; fac<nfin; fac++)
            {
              int poly1 = face_voisins(fac,0);
              int poly2 = face_voisins(fac,1);
              double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
              double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
              for (int i=0; i<dimension; i++)
                for (int j=0; j<dimension; j++)
                  Tenseur_face(fac,i,j) = a*Tenseur_elem(poly1,i,j) + b*Tenseur_elem(poly2,i,j);
            }
        }
      else
        {
          for (int fac=ndeb; fac<nfin; fac++)
            {
              int poly1 = face_voisins(fac,0);
              for (int i=0; i<dimension; i++)
                for (int j=0; j<dimension; j++)
                  Tenseur_face(fac,i,j) = Tenseur_elem(poly1,i,j);
            }
        }
    }
  int n0 = domaine_VEF.premiere_face_int();
  for (int fac = n0; fac<nb_faces; fac++)
    {
      int poly1 = face_voisins(fac,0);
      int poly2 = face_voisins(fac,1);
      double a=volumes(poly1)/(volumes(poly1)+volumes(poly2));
      double b=volumes(poly2)/(volumes(poly1)+volumes(poly2));
      for (int i=0; i<dimension; i++)
        for (int j=0; j<dimension; j++)
          Tenseur_face(fac,i,j) = a*Tenseur_elem(poly1,i,j) + b*Tenseur_elem(poly2,i,j);
    }

  return Tenseur_face;
}
