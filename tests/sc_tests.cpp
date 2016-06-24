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

    void SC_CALL_DECL destroy_current_proc (void*) {
        sc_context_destroy(sc_current_context());
        sc_yield(sc_main_context(), nullptr);
    }

    void SC_CALL_DECL destroy_main_proc (void*) {
        sc_context_destroy(sc_main_context());
        sc_yield(sc_main_context(), nullptr);
    }

    void SC_CALL_DECL set_to_true_proc (void* param) {
        *(bool*)param = true;
        sc_yield(sc_main_context(), nullptr);
    }

    void SC_CALL_DECL yield_current_proc (void*) {
        sc_yield(sc_main_context(), sc_current_context());
    }

    void SC_CALL_DECL yield_main_proc (void*) {
        sc_yield(sc_main_context(), sc_main_context());
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
        sc_yield(context, &result);
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
// sc_yield tests
//

DESCRIBE("sc_yield") {

    GIVEN("a valid context") {
        IT("should switch to that context") {
            auto executed = false;

            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), set_to_true_proc);
            sc_yield(context, &executed);
            sc_context_destroy(context);

            REQUIRE(executed == true);
        }
    }

    GIVEN("the current context") {
        IT("should return the passed-in value") {
            int marker;
            auto result = sc_yield(sc_current_context(), &marker);
            REQUIRE(result == &marker);
        }
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
        auto current = sc_yield(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(context == current);
    }

}

//
// sc_main_context tests
//

DESCRIBE("sc_main_context") {

    IT("should always return the main context") {
        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), yield_main_proc);
        auto main = sc_yield(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(sc_main_context() == main);
    }

}
