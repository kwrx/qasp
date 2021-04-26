/*                                                                      
 * GPLv3 License 
 * 
 * Copyright (C) 2021 Antonino Natale
 * This file is part of QASP.
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
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include "Grounder.hpp"
#include "GringoGrounder.hpp"

#include <memory>

using namespace qasp::grounder;


static std::shared_ptr<Grounder> __instance;

std::shared_ptr<Grounder> Grounder::instance() {

    if(unlikely(!__instance)) {
#if defined(HAVE_GRINGO)
        __instance = std::make_shared<GringoGrounder>();
#elif defined(HAVE_IDLV)
        __instance = std::make_shared<IDLVGrounder>();
#else
#   error "missing grounder application"
#endif
    }

    return __instance;

}