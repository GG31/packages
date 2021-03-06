/*
 * @file vle/extension/decision/Plan.cpp
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


#include <vle/extension/decision/Plan.hpp>
#include <vle/extension/decision/KnowledgeBase.hpp>
#include <vle/utils/Parser.hpp>
#include <vle/utils/i18n.hpp>
#include <vle/utils/DateTime.hpp>
#include <string>
#include <sstream>

namespace vle { namespace extension { namespace decision {

typedef utils::Block UB;
typedef utils::Block::Blocks UBB;
typedef utils::Block::Strings UBS;
typedef utils::Block::Reals UBR;

Plan::Plan(KnowledgeBase& kb, const std::string& buffer)
    : mKb(kb)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

Plan::Plan(KnowledgeBase& kb, std::istream& stream)
    : mKb(kb)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer, const devs::Time& loadTime)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), loadTime);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream, const devs::Time& loadTime)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), loadTime);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const std::string& buffer)
{
    try {
        std::istringstream in(buffer);
        utils::Parser parser(in);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error in %1%")) % e.what());
    }
}

void Plan::fill(std::istream& stream)
{
    try {
        utils::Parser parser(stream);
        fill(parser.root(), 0);
    } catch (const std::exception& e) {
        throw utils::ArgError(fmt(_("Decision plan error: %1%")) % e.what());
    }
}

void Plan::fill(const utils::Block& root, const devs::Time& loadTime)
{
    utils::Block::BlocksResult mainrules, mainactivities, mainprecedences;

    mainrules = root.blocks.equal_range("rules");
    mainactivities = root.blocks.equal_range("activities");
    mainprecedences = root.blocks.equal_range("precedences");

    utils::Block::Blocks::const_iterator it;

    for (it = mainrules.first; it != mainrules.second; ++it) {
        utils::Block::BlocksResult rules;
        rules = it->second.blocks.equal_range("rule");
        fillRules(rules, loadTime);
    }

    for (it = mainactivities.first; it != mainactivities.second; ++it) {
        utils::Block::BlocksResult activities;
        activities = it->second.blocks.equal_range("activity");
        fillActivities(activities, loadTime);
    }

    for (it = mainprecedences.first; it != mainprecedences.second; ++it) {
        utils::Block::BlocksResult precedences;
        precedences = it->second.blocks.equal_range("precedence");
        fillPrecedences(precedences, loadTime);
    }
}

void Plan::fillRules(const utils::Block::BlocksResult& rules, const devs::Time&)
{
    for (UBB::const_iterator it = rules.first; it != rules.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second) {
            throw utils::ArgError(_("Decision: rule needs id"));
        }

        Rule& rule = mRules.add(id.first->second);
        UB::StringsResult preds = block.strings.equal_range("predicates");

        for (UB::Strings::const_iterator jt = preds.first;
             jt != preds.second; ++jt) {
            rule.add((mKb.predicates().get(jt->second))->second);
        }
    }
}

void Plan::fillActivities(const utils::Block::BlocksResult& acts,
        const devs::Time& loadTime)
{
    for (UBB::const_iterator it = acts.first; it != acts.second; ++it) {
        const utils::Block& block = it->second;

        UB::StringsResult id = block.strings.equal_range("id");
        if (id.first == id.second) {
            throw utils::ArgError(_("Decision: activity needs id"));
        }

        Activity& act = mActivities.add(id.first->second, Activity());

        UB::StringsResult rules = block.strings.equal_range("rules");
        for (UBS::const_iterator jt = rules.first; jt != rules.second; ++jt) {
            act.addRule(jt->second, mRules.get(jt->second));
        }

        UB::StringsResult ack = block.strings.equal_range("ack");
        if (ack.first != ack.second) {
            act.addAcknowledgeFunction((mKb.acknowledgeFunctions().get(
                        ack.first->second))->second);
        }

        UB::StringsResult out = block.strings.equal_range("output");
        if (out .first != out.second) {
            act.addOutputFunction((mKb.outputFunctions().get(
                        out.first->second))->second);
        }

        UB::StringsResult upd = block.strings.equal_range("update");
        if (upd.first != upd.second) {
            act.addUpdateFunction((mKb.updateFunctions().get(
                        upd.first->second))->second);
        }

        UB::BlocksResult temporal = block.blocks.equal_range("temporal");
        if (temporal.first != temporal.second) {
            fillTemporal(temporal, act, loadTime);
        }
    }
}

void Plan::fillTemporal(const utils::Block::BlocksResult& temps,
                        Activity& activity,
                        const devs::Time& loadTime)
{
    for (UBB::const_iterator it = temps.first; it != temps.second; ++it) {
        const utils::Block& block = it->second;


        DateResult start = getDate("start",block,loadTime);
        DateResult mins = getDate("minstart",block,loadTime);
        DateResult maxs = getDate("maxstart",block,loadTime);
        DateResult finish = getDate("finish",block,loadTime);
        DateResult minf = getDate("minfinish",block,loadTime);
        DateResult maxf = getDate("maxfinish",block,loadTime);

        if (start.first) {
            if (finish.first) {
                activity.initStartTimeFinishTime(start.second, finish.second);
            } else {
                double vmin, vmax;
                if (minf.first) {
                    if (maxf.first) {
                        vmin = minf.second;
                        vmax = maxf.second;
                    } else {
                        vmin = minf.second;
                        vmax = devs::infinity;
                    }
                } else {
                    if (maxf.first != maxf.second) {
                        vmin = 0;
                        vmax = maxf.second;
                    } else {
                        vmin = 0;
                        vmax = devs::infinity;
                    }
                }
                activity.initStartTimeFinishRange(start.second,vmin,vmax);
            }
        } else {
            double vmin, vmax;
            if (mins.first) {
                vmin = mins.second;
            } else {
                vmin = devs::negativeInfinity;
            }
            if (maxs.first) {
                vmax = maxs.second;
            } else {
                vmax = devs::infinity;
            }
            if (finish.first) {
                activity.initStartRangeFinishTime(vmin, vmax,finish.second);
            } else {
                double vminf, vmaxf;
                if (minf.first) {
                    if (maxf.first) {
                        vminf = minf.second;
                        vmaxf = maxf.second;
                    } else {
                        vminf = minf.second;
                        vmaxf = devs::infinity;
                    }
                } else {
                    if (maxf.first) {
                        vminf = 0;
                        vmaxf = maxf.second;
                    } else {
                        vminf = 0;
                        vmaxf = devs::infinity;
                    }
                }
                activity.initStartRangeFinishRange(
                    vmin, vmax, vminf, vmaxf);
            }
        }
    }
}

void Plan::fillPrecedences(const utils::Block::BlocksResult& preds,
        const devs::Time&)
{
    for (UBB::const_iterator it = preds.first; it != preds.second; ++it) {
        const utils::Block& block = it->second;

        std::string valuefirst, valuesecond;
        double valuemintl = 0.0;
        double valuemaxtl = devs::infinity;

        UB::StringsResult first = block.strings.equal_range("first");
        if (first.first != first.second) {
            valuefirst = first.first->second;
        }

        UB::StringsResult second = block.strings.equal_range("second");
        if (second.first != second.second) {
            valuesecond = second.first->second;
        }

        UB::RealsResult mintl = block.reals.equal_range("mintimelag");
        if (mintl.first != mintl.second) {
            valuemintl = mintl.first->second;
        }

        UB::RealsResult maxtl = block.reals.equal_range("maxtimelag");
        if (maxtl.first != maxtl.second) {
            valuemaxtl = maxtl.first->second;
        }

        UB::StringsResult type = block.strings.equal_range("type");
        if (type.first != type.second) {
            if (type.first->second == "SS") {
                mActivities.addStartToStartConstraint(valuefirst, valuesecond,
                                                      valuemintl, valuemaxtl);
            } else if (type.first->second == "FS") {
                mActivities.addFinishToStartConstraint(valuefirst, valuesecond,
                                                       valuemintl, valuemaxtl);
            } else if (type.first->second == "FF") {
                mActivities.addFinishToFinishConstraint(valuefirst,
                                                        valuesecond,
                                                        valuemintl, valuemaxtl);
            } else {
                throw utils::ArgError(fmt(
                        _("Decision: precendence type `%1%' unknown")) %
                    type.first->second);
            }
        } else {
            throw utils::ArgError(_("Decision: precedences type unknown"));
        }
    }
}

Plan::DateResult Plan::getDate(const std::string& dateName,
               const utils::Block& block, const devs::Time& loadTime) const
{
    UB::RealsResult dateReal = block.reals.equal_range(dateName);
    UB::StringsResult dateString = block.strings.equal_range(dateName);
    UB::RelativeRealsResult dateRelative =
        block.relativeReals.equal_range(dateName);
    bool hasRealDate = dateReal.first != dateReal.second;
    bool hasStringDate = dateString.first != dateString.second;

    bool hasRelativeDate = dateRelative.first != dateRelative.second;
    if((hasRealDate && hasStringDate) ||
            (hasRealDate && hasRelativeDate) ||
            (hasStringDate && hasRelativeDate)) {
        throw utils::ArgError(fmt(_(
         "Decision: date '%1%' should not be given twice ")) % dateName);
    }
    if (hasRealDate){
        return DateResult(true,devs::Time((double) dateReal.first->second));
    } else if (hasStringDate){
        return DateResult(true,devs::Time(
         (int) utils::DateTime::toJulianDayNumber(dateString.first->second)));
        } else if (hasRelativeDate){
        return DateResult(true,
                loadTime + devs::Time((double) dateRelative.first->second));
    } else {
        return DateResult(false,devs::infinity);
    }
}

}}} // namespace vle ext decision
