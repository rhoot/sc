//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include "framework.hpp"

#include <sc.h>
#include <excpt.h>

//
// Assertion testing
//

extern void (* g_sc_assert) ();

struct CheckAssert {
    bool asserted = false;

    CheckAssert () {
        s_instance = this;
        g_sc_assert = on_assert;
    }
    ~CheckAssert () {
        s_instance = nullptr;
        g_sc_assert = nullptr;
    }

private:
    static CheckAssert* s_instance;

    static void on_assert() {
        s_instance->asserted = true;
        throw std::exception("assertion");
    }
};

CheckAssert* CheckAssert::s_instance;

//
// sc_context_create tests
//

DESCRIBE("sc_context_create") {
    CheckAssert asserter;

    GIVEN("a null stack buffer") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_context_create(nullptr, SC_MIN_STACK_SIZE, [](void*) {}));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a stack smaller than `SC_MIN_STACK_SIZE`") {
        IT("should assert") {
            uint8_t buffer[SC_MIN_STACK_SIZE - 1];
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_context_create(buffer, sizeof(buffer), [](void*) {}));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a null proc") {
        IT("should assert") {
            uint8_t buffer[SC_MIN_STACK_SIZE];
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_context_create(buffer, sizeof(buffer), nullptr));
            REQUIRE(asserter.asserted);
        }
    }

    IT("should return a valid context") {
        auto proc = [](void *) {
            uint32_t cafebabe = 0xcafebabe;
            sc_yield(sc_main_context(), &cafebabe);
        };

        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), proc);
        auto result = (uint32_t*)sc_yield(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(*result == 0xcafebabe);
    }

}

//
// sc_context_destroy tests
//

DESCRIBE("sc_context_destroy") {
    static bool s_asserted;

    s_asserted = false;
    g_sc_assert = []() { s_asserted = true; };

    GIVEN("the current context") {
        IT("should assert") {
            auto proc = [](void*) {
                sc_context_destroy(sc_current_context());
                sc_yield(sc_main_context(), nullptr);
            };

            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), proc);
            sc_yield(context, nullptr);
            sc_context_destroy(context);

            REQUIRE(s_asserted);

        }
    }

    GIVEN("the main context") {
        IT("should assert") {
            auto proc = [](void*) {
                sc_context_destroy(sc_main_context());
                sc_yield(sc_main_context(), nullptr);
            };

            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), proc);
            sc_yield(context, nullptr);
            sc_context_destroy(context);

            REQUIRE(s_asserted);
        }
    }

    GIVEN("a valid context") {
        IT("should not assert") {
            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), [](void*) {});
            sc_context_destroy(context);
        }
    }

    g_sc_assert = nullptr;
}

//
// sc_yield tests
//

DESCRIBE("sc_yield") {
    CheckAssert asserter;

    GIVEN("a null context") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_yield(nullptr, nullptr));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a valid context") {
        IT("should switch to that context") {
            auto proc = [](void* executed) {
                *(bool*)executed = true;
                sc_yield(sc_main_context(), nullptr);
            };

            auto executed = false;

            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), proc);
            sc_yield(context, &executed);
            sc_context_destroy(context);

            REQUIRE(executed);
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
        auto proc = [](void*) {
            sc_yield(sc_main_context(), sc_current_context());
        };

        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), proc);
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
        auto proc = [](void*) {
            sc_yield(sc_main_context(), sc_main_context());
        };

        uint8_t stack[SC_MIN_STACK_SIZE];
        auto context = sc_context_create(stack, sizeof(stack), proc);
        auto main = sc_yield(context, nullptr);
        sc_context_destroy(context);

        REQUIRE(sc_main_context() == main);
    }

}