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

using namespace qasp::grounder;


static Grounder* __instance = nullptr;

Grounder* Grounder::instance() {

    if(unlikely(__instance == nullptr)) {
#ifdef HAVE_GRINGO
        __instance = static_cast<Grounder*>(new GringoGrounder());
#elif HAVE_IDLV
        __instance = static_cast<Grounder*>(new IDLVGrounder());
#else
#error "undefined grounder application"
#endif
    }

    return __instance;

}