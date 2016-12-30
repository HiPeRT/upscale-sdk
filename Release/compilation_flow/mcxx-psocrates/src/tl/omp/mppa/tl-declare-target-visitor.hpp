/*--------------------------------------------------------------------
  (C) Copyright 2006-2015 Barcelona Supercomputing Center
                          Centro Nacional de Supercomputacion
  
  This file is part of Mercurium C/C++ source-to-source compiler.
  
  See AUTHORS file in the top level directory for information
  regarding developers and contributors.
  
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 3 of the License, or (at your option) any later version.
  
  Mercurium C/C++ source-to-source compiler is distributed in the hope
  that it will be useful, but WITHOUT ANY WARRANTY; without even the
  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.  See the GNU Lesser General Public License for more
  details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with Mercurium C/C++ source-to-source compiler; if
  not, write to the Free Software Foundation, Inc., 675 Mass Ave,
  Cambridge, MA 02139, USA.
--------------------------------------------------------------------*/

#include "tl-omp-mppa.hpp"
#include "tl-nodecl-visitor.hpp"
#include "tl-nodecl-utils.hpp"
#include "tl-omp-core.hpp"

#include <set>
#include <stdio.h>

namespace TL { namespace MPPA {

class DeclareTargetVisitor : public Nodecl::ExhaustiveVisitor<void>
{
    public:
        DeclareTargetVisitor(Lowering*);
        ~DeclareTargetVisitor();

        virtual void visit(const Nodecl::OpenMP::DeclareTarget& construct);

        TL::ObjectList<TL::Symbol> get_declare_target_functions() const
        {
            return _declare_target_functions;
        }

    private:

        Lowering* _lowering;
        TL::ObjectList<TL::Symbol> _declare_target_functions;
};

} }

