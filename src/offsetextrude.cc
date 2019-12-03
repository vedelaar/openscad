/*
 *  OpenSCAD (www.openscad.org)
 *  Copyright (C) 2009-2011 Clifford Wolf <clifford@clifford.at> and
 *                          Marius Kintel <marius@kintel.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  As a special exception, you have permission to link this program
 *  with the CGAL library and distribute executables, as long as you
 *  follow the requirements of the GNU GPL in regard to all of the
 *  software in the executable aside from CGAL.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "offsetextrudenode.h"

#include "module.h"
#include "ModuleInstantiation.h"
#include "evalcontext.h"
#include "printutils.h"
#include "fileutils.h"
#include "builtin.h"
#include "calc.h"
#include "polyset.h"

#include <sstream>
#include <boost/assign/std/vector.hpp>
using namespace boost::assign; // bring 'operator+=()' into scope

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

class OffsetExtrudeModule : public AbstractModule
{
public:
    OffsetExtrudeModule() { }
    AbstractNode *instantiate(const std::shared_ptr<Context>& ctx, const ModuleInstantiation *inst, const std::shared_ptr<EvalContext>& evalctx) const override;
};

AbstractNode *OffsetExtrudeModule::instantiate(const std::shared_ptr<Context>& ctx, const ModuleInstantiation *inst, const std::shared_ptr<EvalContext>& evalctx) const
{
    auto node = new OffsetExtrudeNode(inst);

    AssignmentList args{Assignment("height"), Assignment("center"), Assignment("slices"), Assignment("r"), Assignment("delta"), Assignment("chamfer")};

    ContextHandle<Context> c{Context::create<Context>(ctx)};
    c->setVariables(evalctx, args);
    inst->scope.apply(evalctx);

    node->fn = c->lookup_variable("$fn")->toDouble();
    node->fs = c->lookup_variable("$fs")->toDouble();
    node->fa = c->lookup_variable("$fa")->toDouble();

    node->delta = 1;
    node->chamfer = false;
    node->join_type = ClipperLib::jtRound;
    node->center = false;
    auto r = c->lookup_variable("r", true);
    auto delta = c->lookup_variable("delta", true);
    auto chamfer = c->lookup_variable("chamfer", true);
    auto height = c->lookup_variable("height", true);
    auto convexity = c->lookup_variable("convexity", true);
    auto center = c->lookup_variable("center", true);
    auto slices = c->lookup_variable("slices", true);

    // if height not given, and first argument is a number,
    // then assume it should be the height.
    if (c->lookup_variable("height")->isUndefined() &&
        evalctx->numArgs() > 0 &&
        evalctx->getArgName(0) == "") {
        auto val = evalctx->getArgValue(0);
        if (val->type() == Value::ValueType::NUMBER) height = val;
    }
    node->height = 100;
    height->getFiniteDouble(node->height);
    node->convexity = static_cast<int>(convexity->toDouble());

    if (node->height <= 0) node->height = 0;

    if (node->convexity <= 0)
        node->convexity = 1;

    double slicesVal = 0;
    slices->getFiniteDouble(slicesVal);
    node->slices = static_cast<int>(slicesVal);
    node->slices = std::max(node->slices, 1);

    if (r->isDefinedAs(Value::ValueType::NUMBER)) {
        r->getDouble(node->delta);
    } else if (delta->isDefinedAs(Value::ValueType::NUMBER)) {
        delta->getDouble(node->delta);
        node->join_type = ClipperLib::jtMiter;
        if (chamfer->isDefinedAs(Value::ValueType::BOOL) && chamfer->toBool()) {
            node->chamfer = true;
            node->join_type = ClipperLib::jtSquare;
        }
    }

    if (center->type() == Value::ValueType::BOOL)
        node->center = center->toBool();

    auto instantiatednodes = inst->instantiateChildren(evalctx);
    node->children.insert(node->children.end(), instantiatednodes.begin(), instantiatednodes.end());

    return node;
}

std::string OffsetExtrudeNode::toString() const
{
    std::stringstream stream;

    bool isRadius = this->join_type == ClipperLib::jtRound;
    auto var = isRadius ? "(r = " : "(delta = ";

    stream  << this->name() << var << std::dec << this->delta;
    if (!isRadius) {
        stream << ", chamfer = " << (this->chamfer ? "true" : "false");
    }
    stream << ", height = " << this->height
           << ", slices = " << this->slices
           << ", $fn = " << this->fn
           << ", $fa = " << this->fa
           << ", $fs = " << this->fs << ")";

    return stream.str();
}

void register_builtin_offset_extrude()
{
    Builtins::init("offset_extrude", new OffsetExtrudeModule(), {
            "offset_extrude(height, r = 1, slices = 1, chamfer = false, center = false[, $fn, $fa, $fs])",
            "offset_extrude(height, delta = 1, slices = 1, chamfer = false, center = false[, $fn, $fa, $fs])",
    });
}
