/*
 * @file vle/gvle/modeling/difference-equation/TimeStep.hpp
 *
 * This file is part of VLE, a framework for multi-modeling, simulation
 * and analysis of complex dynamical systems
 * http://www.vle-project.org
 *
 * Copyright (c) 2003-2007 Gauthier Quesnel <quesnel@users.sourceforge.net>
 * Copyright (c) 2003-2011 ULCO http://www.univ-littoral.fr
 * Copyright (c) 2007-2011 INRA http://www.inra.fr
 *
 * See the AUTHORS or Authors.txt file for copyright owners and contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef VLE_GVLE_MODELING_DIFFERENCEEQUATION_TIMESTEP_HPP
#define VLE_GVLE_MODELING_DIFFERENCEEQUATION_TIMESTEP_HPP

#include <vle/vpz/Condition.hpp>
#include <gtkmm/button.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/frame.h>
#include <gtkmm/spinbutton.h>
#include <gtkmm/builder.h>
#include <gtkmm/checkbutton.h>

namespace vle { namespace gvle { namespace modeling { namespace de {

class TimeStep
{
public:
    TimeStep() { }
    virtual ~TimeStep() { }

    void assign(vpz::Condition& condition);
    Gtk::Widget& build(Glib::RefPtr < Gtk::Builder >& ref);
    void deletePorts(vpz::Condition& condition);
    void fillFields(const vpz::Condition& condition);

private:
    class UnitComboBoxText : public Gtk::ComboBoxText
    {
    public:
	UnitComboBoxText(BaseObjectType* cobject,
                         const Glib::RefPtr < Gtk::Builder >&) :
	    Gtk::ComboBoxText(cobject) { }
	virtual ~UnitComboBoxText() { }
    };

    Gtk::Frame*       m_frame;
    Gtk::SpinButton*  m_spinTime;
    Gtk::CheckButton* m_checkGlobal;
    UnitComboBoxText* m_comboUnit;

    bool validPort(const vpz::Condition& condition,
		   const std::string& name);

    void onClickCheckButton();
};

}}}} // namespace vle gvle modeling de

#endif
