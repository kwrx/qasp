#include "Grounder.hpp"
#include "GringoGrounder.hpp"

using namespace qasp::grounder;


static Grounder* __instance = nullptr;

Grounder* Grounder::instance() {

    if(__instance == nullptr) {
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