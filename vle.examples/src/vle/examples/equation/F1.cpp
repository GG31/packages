/*
 * @file vle/examples/equation/F1.cpp
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

/*
 * @@tagdepends: vle.extension.difference-equation @@endtagdepends
 */

#include <vle/examples/equation/F1.hpp>

namespace vle { namespace examples { namespace equation {

void F1::compute(const devs::Time& /* time */)
{
    a = a(-1) + 1;
    b = b(-1) + d(-1) - g() + 1;
    c = c(-1) + b(-1) + 1;
}

void F1::initValue(const devs::Time& /* time */)
{
    a = 1.0;
    b = a(0) - 3;
//    c = 0.0; use default value
}

}}} // namespace vle examples equation
