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
// File:        Problemes_Scalaires_Passifs_Turbulent.cpp
// Directory:   $TURBULENCE_ROOT/src/ThHyd/Incompressible/Problems
//
//////////////////////////////////////////////////////////////////////////////

#include <Problemes_Scalaires_Passifs_Turbulent.h>


Implemente_instanciable(Pb_Thermohydraulique_Turbulent_Scalaires_Passifs,"Pb_Thermohydraulique_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>);
Sortie& Pb_Thermohydraulique_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>::printOn(os); }
Entree& Pb_Thermohydraulique_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent>::readOn(is); }

Implemente_instanciable(Pb_Thermohydraulique_Especes_Turbulent_QC,"Pb_Thermohydraulique_Especes_Turbulent_QC",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>);
Sortie& Pb_Thermohydraulique_Especes_Turbulent_QC::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>::printOn(os); }
Entree& Pb_Thermohydraulique_Especes_Turbulent_QC::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Turbulent_QC>::readOn(is); }

Implemente_instanciable(Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs,"Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>);
Sortie& Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>::printOn(os); }
Entree& Pb_Hydraulique_Concentration_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Hydraulique_Concentration_Turbulent>::readOn(is); }

Implemente_instanciable(Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs,"Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs",TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>);
Sortie& Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs::printOn(Sortie& os) const { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>::printOn(os); }
Entree& Pb_Thermohydraulique_Concentration_Turbulent_Scalaires_Passifs::readOn(Entree& is) { return TRUSTProblem_sup_eqns<Pb_Thermohydraulique_Concentration_Turbulent>::readOn(is); }
