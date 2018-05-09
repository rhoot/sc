/*
 * Copyright (c) 2017 Johan Sköld
 * License: https://opensource.org/licenses/ISC
 */

#define SC_META_INCLUDED

#if defined(_WIN32)

    #if defined(__i386__)
        #include "state_i386_ms_pe_gas.S"
    #elif defined(__x86_64__)
        #include "state_x86_64_ms_pe_gas.S"
    #else
        #error "Unsupported architecture"
    #endif

#elif defined(__APPLE__)

    #if defined(__x86_64__)
        #include "state_x86_64_sysv_macho_gas.S"
    #endif

#endif

#undef SC_META_INCLUDED
