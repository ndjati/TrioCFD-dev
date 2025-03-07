/****************************************************************************
* Copyright (c) 2015, CEA
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
// File:        Champ_Uniforme_Morceaux.cpp
// Directory:   $TRIO_U_ROOT/src/Kernel/Champs
// Version:     /main/29
//
//////////////////////////////////////////////////////////////////////////////

#include <Champ_Uniforme_Morceaux.h>
#include <Champ_Uniforme.h>
#include <Sous_Domaine.h>
#include <Domaine.h>
#include <Interprete.h>
#include <Parser_U.h>
#include <TRUST_Vector.h>

Implemente_instanciable(Champ_Uniforme_Morceaux,"Champ_Uniforme_Morceaux",Champ_Don_base);




/*! @brief Imprime les valeurs du champs sur un flot de sortie
 *
 * @param (Sortie& os) un flot de sortie
 * @return (Sortie&) le flot de sortie modifie
 */
Sortie& Champ_Uniforme_Morceaux::printOn(Sortie& os) const
{
  return os << valeurs();
}


/*! @brief Lit les valeurs du champ uniforme par morceaux a partir d'un flot d'entree.
 *
 *     On lit le nom du domaine (nom_domaine) le nombre de
 *     composantes du champ (nb_comp) la valeur par defaut
 *     du champ ainsi que les valeurs sur les sous domaines.
 *     Format:
 *      Champ_Uniforme_Morceaux nom_domaine nb_comp
 *      { Defaut val_def sous_domaine_1 val_1 ... sous_domaine_i val_i }
 *
 * @param (Entree& is) un flot d'entree
 * @return (Entree&) le champ d'entree modifie
 * @throws accolade ouvrante attendue
 * @throws mot clef "defaut" attendu
 */
Entree& Champ_Uniforme_Morceaux::readOn(Entree& is)
{
  int dim;
  Motcle motlu;
  Nom nom;
  int k, poly;
  is >> nom;
  mon_domaine = ref_cast(Domaine, Interprete::objet(nom));
  Domaine& le_domaine=mon_domaine.valeur();
  const IntTab& les_elems=le_domaine.les_elems();
  const int nb_som_elem = le_domaine.nb_som_elem();
  double x=0,y=0,z=0;
  int num_som;



  dim=lire_dimension(is,que_suis_je());
  fixer_nb_comp(dim);
  valeurs_.resize(0, dim);
  le_domaine.creer_tableau_elements(valeurs_);
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("{") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected a { instead of " << nom << finl;
      exit();
    }
  is >> nom;
  motlu=nom;
  if(motlu != Motcle("defaut") )
    {
      Cerr << "Error while reading a " << que_suis_je() << finl;
      Cerr << "We expected defaut instead of " << nom << finl;
      exit();
    }


  VECT(Parser_U) fxyz(dim);
  for( k=0; k< dim; k++)
    {
      Nom tmp;
      is >> tmp;
      fxyz[k].setNbVar(3);
      fxyz[k].setString(tmp);
      fxyz[k].addVar("x");
      fxyz[k].addVar("y");
      fxyz[k].addVar("z");
      fxyz[k].parseString();
    }

  for( poly=0; poly<le_domaine.nb_elem(); poly++)
    {
      x = le_domaine.coord(les_elems(poly,0),0);
      y = le_domaine.coord(les_elems(poly,0),1);
      if (dimension == 3) z = le_domaine.coord(les_elems(poly,0),2);
      for (int isom = 1; isom<nb_som_elem; isom++)
        {
          num_som = les_elems(poly,isom);
          x += le_domaine.coord(num_som,0);
          y += le_domaine.coord(num_som,1);
          if (dimension == 3) z += le_domaine.coord(num_som,2);
        }
      x = x/nb_som_elem;
      y = y/nb_som_elem;
      z = z/nb_som_elem;
      for( k=0; k< dim; k++)
        {
          fxyz[k].setVar("x",x);
          fxyz[k].setVar("y",y);
          fxyz[k].setVar("z",z);
          valeurs_(poly,k)=fxyz[k].eval();
        }
    }

  is >> nom;
  motlu=nom;
  while (motlu != Motcle("}") )
    {
      REF(Sous_Domaine) refssz=les_sous_domaines.add(le_domaine.ss_domaine(nom));
      Sous_Domaine& ssz = refssz.valeur();
      for( k=0; k< dim; k++)
        {
          Nom tmp;
          is >> tmp;
          fxyz[k].setString(tmp);
          fxyz[k].parseString();
        }
      double vt=0.;
      for( poly=0; poly<ssz.nb_elem_tot(); poly++)
        {
          x = le_domaine.coord(les_elems(ssz(poly),0),0);
          y = le_domaine.coord(les_elems(ssz(poly),0),1);
          if (dimension == 3) z = le_domaine.coord(les_elems(ssz(poly),0),2);
          for (int isom = 1; isom<nb_som_elem; isom++)
            {
              num_som = les_elems(ssz(poly),isom);
              x += le_domaine.coord(num_som,0);
              y += le_domaine.coord(num_som,1);
              if (dimension == 3) z += le_domaine.coord(num_som,2);
            }
          x = x/nb_som_elem;
          y = y/nb_som_elem;
          z = z/nb_som_elem;
          for( k=0; k< dim; k++)
            {
              fxyz[k].setVar("x",x);
              fxyz[k].setVar("y",y);
              fxyz[k].setVar("z",z);
              valeurs_(ssz(poly),k)=fxyz[k].eval();
              vt+=valeurs_(ssz(poly),k);
            }
        }
      is >> nom;
      motlu=nom;
    }
// GF pour une utilisation detournee de ch_unif_ssz ....
  if (valeurs_.dimension(1)==1)
    {
      DoubleTab save(valeurs_);
      valeurs_.set_md_vector(MD_Vector());
      valeurs_.resize(0);

      le_domaine.creer_tableau_elements(valeurs_);
      //valeurs_.resize(valeurs_.dimension_tot(0));
      valeurs_.inject_array(save);
    }
  valeurs().echange_espace_virtuel();
  return is;
}

/*! @brief Renvoie une reference sur le domaine associe.
 *
 * (version const)
 *
 * @return (REF(Domaine)&) reference sur le domaine associe
 */
const REF(Domaine)& Champ_Uniforme_Morceaux::domaine() const
{
  return mon_domaine;
}

/*! @brief Renvoie une reference sur le domaine associe.
 *
 * @return (REF(Domaine)&) reference sur le domaine associe
 */
REF(Domaine)& Champ_Uniforme_Morceaux::domaine()
{
  return mon_domaine;
}

/*! @brief Renvoie la liste des sous_domaines associees.
 *
 * (version const)
 *
 * @return (LIST(REF(Sous_Domaine))&) la liste des sous_domaines associees
 */
const LIST(REF(Sous_Domaine))& Champ_Uniforme_Morceaux::sous_domaines() const
{
  return les_sous_domaines;
}

/*! @brief Renvoie la liste des sous_domaines associees.
 *
 * @return (LIST(REF(Sous_Domaine))&) la liste des sous_domaines associees
 */
LIST(REF(Sous_Domaine))& Champ_Uniforme_Morceaux::sous_domaines()
{
  return les_sous_domaines;
}


/*! @brief NE FAIT RIEN
 *
 * @param (mp_base& ch)
 * @return (Champ_base&)
 */
Champ_base& Champ_Uniforme_Morceaux::affecter_(const Champ_base& ch)
{
  if(sub_type(Champ_Uniforme_Morceaux, ch))
    {
      const Champ_Uniforme_Morceaux& chum=ref_cast( Champ_Uniforme_Morceaux, ch);
      les_sous_domaines=chum.sous_domaines();
      mon_domaine=chum.domaine();
      valeurs_=chum.valeurs();
    }
  else if (sub_type(Champ_Uniforme, ch))
    {
      const Champ_Uniforme& chu=ref_cast( Champ_Uniforme, ch);
      int nb_poly=valeurs_.dimension(0);
      int dim=valeurs_.dimension(1);
      const DoubleTab& ch_val=chu.valeurs();
      int k, poly;
      if(ch_val.nb_dim()==1)
        for(k=0; k<dim; k++)
          for(poly=0; poly<nb_poly; poly++)
            {
              valeurs_(poly,k)=ch_val(k);
            }
      else
        for(k=0; k<dim; k++)
          for(poly=0; poly<nb_poly; poly++)
            {
              valeurs_(poly,k)=ch_val(0,k);
            }
    }
  else
    {
      Cerr << "It is not known to affect a " << ch.que_suis_je()
           << " at a " << que_suis_je() << finl;
      exit();
    }
  return *this;
}


/*! @brief Renvoie la valeur du champ au point specifie par ses coordonnees.
 *
 * @param (DoubleVect& positions) les coordonnees du point de calcul
 * @param (DoubleVect& valeurs) la valeur du champ au point specifie
 * @return (DoubleVect&) la valeur du champ au point specifie
 */
DoubleVect& Champ_Uniforme_Morceaux::valeur_a(const DoubleVect& positions,
                                              DoubleVect& tab_valeurs) const
{
  const Domaine& le_dom = mon_domaine;
  IntVect le_poly(1);
  le_dom.chercher_elements(positions,le_poly);
  return valeur_a_elem(positions,tab_valeurs,le_poly[0]);
}


/*! @brief Renvoie la valeur du champ au point specifie par ses coordonnees, en indiquant que ce point est
 *
 *     situe dans un element specifie.
 *
 * @param (DoubleVect&) les coordonnees du point de calcul
 * @param (DoubleVect& val) la valeur du champ au point specifie
 * @param (int le_poly) l'element dans lequel est situe le point de calcul
 * @return (DoubleVect&) la valeur du champ au point specifie
 * @throws tableau stockant la valeur du champ de taille incorrecte
 */
DoubleVect& Champ_Uniforme_Morceaux::valeur_a_elem(const DoubleVect& ,
                                                   DoubleVect& val,
                                                   int le_poly) const
{
  if (val.size() != nb_compo_)
    {
      Cerr << "Error Trio_U in Champ_Uniforme_Morceaux::valeur_a_elem()" << finl;
      Cerr << "The DoubleVect val doesn't have the correct size" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val(0) = ch(le_poly,0);
  else
    for (int k=0; k<nb_compo_; k++)
      val(k) = ch(le_poly,k);

  return val;
}


/*! @brief Renvoie la valeur d'une composante du champ au point specifie par ses coordonnees, en indiquant que ce point est
 *
 *     situe dans un element specifie.
 *
 * @param (DoubleVect&) les coordonnees du point de calcul
 * @param (int le_poly) l'element dans lequel est situe le point de calcul
 * @param (int ncomp) l'index de la composante du champ a calculer
 * @return (double) la valeur de la composante du champ specifiee au point specifie
 * @throws index de la composante du champ a calculer trop grand
 */
double Champ_Uniforme_Morceaux::valeur_a_elem_compo(const DoubleVect& ,
                                                    int le_poly,
                                                    int ncomp) const
{
  double val;
  if (ncomp > nb_compo_)
    {
      Cerr << "Error Trio_U in Champ_Uniforme_Morceaux::valeur_a_elem_compo()" << finl;
      Cerr << "the integer ncomp is upper than the number of field components" << finl;
      exit();
    }

  const DoubleTab& ch = valeurs();

  if (nb_compo_ == 1)
    val = ch(le_poly,0);
  else
    val = ch(le_poly,ncomp);

  return val;
}


/*! @brief Renvoie les valeurs du champ aux points specifies par leurs coordonnees.
 *
 * @param (DoubleTab& positions) le tableau des coordonnees des points de calcul
 * @param (DoubleTab& valeurs) le tableau des valeurs du champ aux points specifies
 * @return (DoubleTab&) le tableau des valeurs du champ aux points specifies
 */
DoubleTab& Champ_Uniforme_Morceaux::valeur_aux(const DoubleTab& positions,
                                               DoubleTab& tab_valeurs) const
{
  const Domaine& le_dom = mon_domaine;
  IntVect les_polys(le_dom.nb_elem());
  le_dom.chercher_elements(positions,les_polys);
  return valeur_aux_elems(positions,les_polys,tab_valeurs);
}


/*! @brief Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees.
 *
 * @param (DoubleTab& positions) le tableau des coordonnees des points de calcul
 * @param (DoubleVect& valeurs) le tableau des valeurs de la composante du champ aux points specifies
 * @param (int ncomp) l'index de la composante du champ a calculer
 * @return (DoubleVect&) le tableau des valeurs de la composante du champ aux points specifies
 */
DoubleVect& Champ_Uniforme_Morceaux::valeur_aux_compo(const DoubleTab& positions,
                                                      DoubleVect& tab_valeurs, int ncomp) const
{
  const Domaine& le_dom = mon_domaine;
  IntVect les_polys(le_dom.nb_elem());
  le_dom.chercher_elements(positions,les_polys);
  return valeur_aux_elems_compo(positions,les_polys,tab_valeurs,ncomp);
}


/*! @brief Renvoie les valeurs du champ aux points specifies par leurs coordonnees, en indiquant que les points de
 *
 *     calculs sont situes dans les elements indiques.
 *
 * @param (DoubleTab&) le tableau des coordonnees des points de calcul
 * @param (IntVect& les_polys) le tableau des elements dans lesquels sont situes les points de calcul
 * @param (DoubleTab& val) le tableau des valeurs du champ aux points specifies
 * @return (DoubleTab&) le tableau des valeurs du champ aux points specifies
 * @throws le tableau des valeurs a plus de 2 entrees (rang > 2)
 */
DoubleTab& Champ_Uniforme_Morceaux::valeur_aux_elems(const DoubleTab& ,
                                                     const IntVect& les_polys,
                                                     DoubleTab& val) const
{
  if (val.nb_dim() == 1)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(nb_compo_ == 1);
    }
  else if (val.nb_dim() == 2)
    {
      assert((val.dimension(0) == les_polys.size())||(val.dimension_tot(0) == les_polys.size()));
      assert(val.dimension(1) == nb_compo_);
    }
  else
    {
      Cerr << "Error Trio_U in Champ_Uniforme_Morceaux::valeur_aux_elems()" << finl;
      Cerr << "The DoubleTab val has more than 2 entries" << finl;
      exit();
    }

  int le_poly;

  if (nb_compo_ == 1)
    {
      const DoubleTab& ch = valeurs();
      if ((ch.nb_dim() == 1) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                val(rang_poly) = ch(le_poly);
            }
        }
      else if ((ch.nb_dim() == 1) && (val.nb_dim() == 2))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly,0) = 0;
              else
                val(rang_poly,0) = ch(le_poly);
            }
        }
      else if ((ch.nb_dim() == 2) && (val.nb_dim() == 1))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly) = 0;
              else
                val(rang_poly) = ch(le_poly,0);
            }
        }
      else if ((ch.nb_dim() == 2) && (val.nb_dim() == 2))
        {
          for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
            {
              le_poly=les_polys(rang_poly);
              if (le_poly == -1)
                val(rang_poly,0) = 0;
              else
                val(rang_poly,0) = ch(le_poly,0);
            }
        }

    }
  else // nb_compo_ > 1
    {
      const DoubleTab& ch = valeurs();

      for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
        {
          le_poly=les_polys(rang_poly);
          if (le_poly == -1)
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = 0;
          else
            for(int ncomp=0; ncomp<nb_compo_; ncomp++)
              val(rang_poly, ncomp) = ch(le_poly,ncomp);
        }

    }
  return val;
}


/*! @brief Renvoie les valeurs d'une composante du champ aux points specifies par leurs coordonnees, en indiquant que les points de
 *
 *     calculs sont situes dans les elements indiques.
 *
 * @param (DoubleTab&) le tableau des coordonnees des points de calcul
 * @param (IntVect& les_polys) le tableau des elements dans lesquels sont situes les points de calcul
 * @param (DoubleVect& val) le tableau des valeurs de la composante du champ aux points specifies
 * @param (int ncomp) l'index de la composante du champ a calculer
 * @return (DoubleVect&) le tableau des valeurs de la composante du champ aux points specifies
 */
DoubleVect& Champ_Uniforme_Morceaux::
valeur_aux_elems_compo(const DoubleTab&,
                       const IntVect& les_polys,
                       DoubleVect& val,
                       int ncomp) const
{
  assert(val.size() == les_polys.size());
  int le_poly;

  const DoubleTab& ch = valeurs();

  for(int rang_poly=0; rang_poly<les_polys.size(); rang_poly++)
    {
      le_poly=les_polys(rang_poly);
      if (le_poly == -1)
        val(rang_poly) = 0;
      else
        val(rang_poly) = ch(le_poly,ncomp);
    }
  return val;
}



