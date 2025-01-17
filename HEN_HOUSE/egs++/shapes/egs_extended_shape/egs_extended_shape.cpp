/*
###############################################################################
#
#  EGSnrc egs++ extended shape
#  Copyright (C) 2015 National Research Council Canada
#
#  This file is part of EGSnrc.
#
#  EGSnrc is free software: you can redistribute it and/or modify it under
#  the terms of the GNU Affero General Public License as published by the
#  Free Software Foundation, either version 3 of the License, or (at your
#  option) any later version.
#
#  EGSnrc is distributed in the hope that it will be useful, but WITHOUT ANY
#  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
#  FOR A PARTICULAR PURPOSE.  See the GNU Affero General Public License for
#  more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with EGSnrc. If not, see <http://www.gnu.org/licenses/>.
#
###############################################################################
#
#  Author:          Iwan Kawrakow, 2005
#
#  Contributors:    Hannah Gallop
#
###############################################################################
*/


/*! \file egs_extended_shape.cpp
 *  \brief An extended shape
 *  \IK
 */

#include "egs_extended_shape.h"
#include "egs_input.h"
#include "egs_functions.h"

static bool EGS_EXTENDED_SHAPE_LOCAL inputSet = false;
static shared_ptr<EGS_BlockInput> EGS_EXTENDED_SHAPE_LOCAL shapeBlockInput = make_shared<EGS_BlockInput>("shape");

extern "C" {

    static void setInputs() {
        inputSet = true;
        shapeBlockInput->addSingleInput("library", true, "The type of shape, loaded by shared library in egs++/dso.", {"EGS_Extended_Shape"});
        shapeBlockInput->addSingleInput("extension", true, "Adds delta z to the z-component of the position vector (z1, z2)");

        auto shapePtr = shapeBlockInput->addBlockInput("shape");
        setShapeInputs(shapePtr);
    }

    EGS_EXTENDED_SHAPE_EXPORT string getExample() {
        string example;
        example = {
            R"(
    # Example of egs_extended_shape
    #:start shape:
        library = egs_extended_shape
        :start shape:
            definition of the shape to be 'extended'
        :stop shape:
        extension = z1 z2
)"};
        return example;
    }

    EGS_EXTENDED_SHAPE_EXPORT shared_ptr<EGS_BlockInput> getInputs() {
        if(!inputSet) {
            setInputs();
        }
        return shapeBlockInput;
    }

    EGS_EXTENDED_SHAPE_EXPORT EGS_BaseShape *createShape(EGS_Input *input,
            EGS_ObjectFactory *f) {
        if (!input) {
            egsWarning("createShape(extended shape): null input?\n");
            return 0;
        }
        vector<EGS_Float> h;
        int err = input->getInput("extension",h);
        if (err || h.size() != 2) {
            egsWarning("createShape(extended shape): wrong/missing "
                       "'extension' input\n");
            return 0;
        }
        EGS_Input *ishape = input->takeInputItem("shape",false);
        EGS_BaseShape *shape=0;;
        if (ishape) {
            shape = EGS_BaseShape::createShape(ishape);
            delete ishape;
        }
        if (!shape) {
            string shape_name;
            int err = input->getInput("shape name",shape_name);
            if (err) {
                egsWarning("createShape(extended shape): no inline shape definition"
                           " and no 'shape name' keyword\n");
                return 0;
            }
            shape = EGS_BaseShape::getShape(shape_name);
            if (!shape) {
                egsWarning("createShape(extended shape): no shape named %s "
                           "exists\n",shape_name.c_str());
                return 0;
            }
        }
        EGS_ExtendedShape *s = new EGS_ExtendedShape(shape,h[0],h[1],"",f);
        s->setName(input);
        s->setTransformation(input);
        return s;
    }

}
