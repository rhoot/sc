//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include "framework.hpp"

#include <cstdint> // uint8_t

#include <sc.h>

//
// Test procs
//

namespace {

    void SC_CALL_DECL empty_proc (void*) {}

    void SC_CALL_DECL set_to_true_proc (void* param) {
        *(bool*)param = true;
        sc_yield(nullptr);
    }

    void SC_CALL_DECL yield_current_proc (void*) {
        sc_yield(sc_current_context());
    }

    void SC_CALL_DECL yield_main_proc (void*) {
        sc_yield(sc_main_context());
    }

    void SC_CALL_DECL yield_parent_proc (void*) {
        sc_yield(sc_parent_context());
    }

    void SC_CALL_DECL recursive_proc (void*) {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), yield_main_proc);
        auto yielded = sc_switch(context, NULL);
        sc_context_destroy(context);
        sc_yield(yielded);
    }

} // namespace

//
// sc_context_create tests
//

DESCRIBE("sc_context_create") {

    IT("should return a valid context") {
        bool result = false;

        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), set_to_true_proc);
        sc_switch(context, &result);
        sc_context_destroy(context);

        REQUIRE(result == true);
    }

}

//
// sc_context_destroy tests
//

DESCRIBE("sc_context_destroy") {

    IT("should not crash") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), empty_proc);
        sc_context_destroy(context);
    }

}

//
// sc_switch tests
//

DESCRIBE("sc_switch") {

    GIVEN("a valid context") {
        IT("should switch to that context") {
            auto executed = false;

            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), set_to_true_proc);
            sc_switch(context, &executed);
            sc_context_destroy(context);

            REQUIRE(executed == true);
        }
    }

    GIVEN("the current context") {
        IT("should return the passed-in value") {
            int marker;
            auto result = sc_switch(sc_current_context(), &marker);
            REQUIRE(result == &marker);
        }
    }

}

//
// sc_yield tests
//

DESCRIBE("sc_yield") {

    IT("should switch to the parent context") {
        uint8_t stack[8192];
        auto context = sc_context_create(stack, sizeof(stack), recursive_proc);
        auto yielded = sc_switch(context, NULL);
        sc_context_destroy(context);

        REQUIRE(yielded == sc_main_context());
    }

}

//
// sc_set_data
//

DESCRIBE("sc_set_data") {

    IT("should store the pointer") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), empty_proc);
        auto pointer = (void*)uintptr_t(0xbadf00d);
        sc_set_data(context, pointer);
        REQUIRE(sc_get_data(context) == pointer);
        sc_context_destroy(context);
    }

}

//
// sc_get_data
//

DESCRIBE("sc_get_data") {

    IT("should default to NULL") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), empty_proc);
        REQUIRE(sc_get_data(context) == nullptr);
        sc_context_destroy(context);
    }

    IT("should get the pointer") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), empty_proc);
        auto pointer = (void*)uintptr_t(0xbadf00d);
        sc_set_data(context, pointer);
        REQUIRE(sc_get_data(context) == pointer);
        sc_context_destroy(context);
    }

}

//
// sc_current_context tests
//

DESCRIBE("sc_current_context") {

    IT("should return the main context when not context switched") {
        REQUIRE(sc_current_context() == sc_main_context());
    }

    IT("should return the sc_context_t of the currently executing context") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), yield_current_proc);
        auto current = sc_switch(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(context == current);
    }

}

//
// sc_parent_context tests
//

DESCRIBE("sc_parent_context") {

    IT("should return the parent context") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), yield_parent_proc);
        auto parent = sc_switch(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(sc_main_context() == parent);
    }

}

//
// sc_main_context tests
//

DESCRIBE("sc_main_context") {

    IT("should always return the main context") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), yield_main_proc);
        auto main = sc_switch(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(sc_main_context() == main);
    }

}
