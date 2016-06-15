//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include "framework.hpp"

#include <sj.h>
#include <excpt.h>

//
// Assertion testing
//

extern void (* g_sj_assert) ();

struct CheckAssert {
    bool asserted = false;

    CheckAssert () {
        s_instance = this;
        g_sj_assert = on_assert;
    }
    ~CheckAssert () {
        s_instance = nullptr;
        g_sj_assert = nullptr;
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
// sj_context_create tests
//

DESCRIBE("sj_context_create") {
    CheckAssert asserter;

    GIVEN("a null stack buffer") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sj_context_create(nullptr, SJ_MIN_STACK_SIZE, [](void*) {}));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a stack smaller than `SJ_MIN_STACK_SIZE`") {
        IT("should assert") {
            uint8_t buffer[SJ_MIN_STACK_SIZE - 1];
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sj_context_create(buffer, sizeof(buffer), [](void*) {}));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a null proc") {
        IT("should assert") {
            uint8_t buffer[SJ_MIN_STACK_SIZE];
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sj_context_create(buffer, sizeof(buffer), nullptr));
            REQUIRE(asserter.asserted);
        }
    }

    IT("should return a valid context") {
        auto proc = [](void *) {
            uint32_t cafebabe = 0xcafebabe;
            sj_yield(sj_main_context(), &cafebabe);
        };

        uint8_t stack[SJ_MIN_STACK_SIZE];
        auto context = sj_context_create(stack, sizeof(stack), proc);
        auto result = (uint32_t*)sj_yield(context, nullptr);
        sj_context_destroy(context);

        REQUIRE(*result == 0xcafebabe);
    }

}

//
// sj_context_destroy tests
//

DESCRIBE("sj_context_destroy") {
    static bool s_asserted;

    s_asserted = false;
    g_sj_assert = []() { s_asserted = true; };

    GIVEN("the current context") {
        IT("should assert") {
            auto proc = [](void*) {
                sj_context_destroy(sj_current_context());
                sj_yield(sj_main_context(), nullptr);
            };

            uint8_t stack[SJ_MIN_STACK_SIZE];
            auto context = sj_context_create(stack, sizeof(stack), proc);
            sj_yield(context, nullptr);
            sj_context_destroy(context);

            REQUIRE(s_asserted);

        }
    }

    GIVEN("the main context") {
        IT("should assert") {
            auto proc = [](void*) {
                sj_context_destroy(sj_main_context());
                sj_yield(sj_main_context(), nullptr);
            };

            uint8_t stack[SJ_MIN_STACK_SIZE];
            auto context = sj_context_create(stack, sizeof(stack), proc);
            sj_yield(context, nullptr);
            sj_context_destroy(context);

            REQUIRE(s_asserted);
        }
    }

    GIVEN("a valid context") {
        IT("should not assert") {
            uint8_t stack[SJ_MIN_STACK_SIZE];
            auto context = sj_context_create(stack, sizeof(stack), [](void*) {});
            sj_context_destroy(context);
        }
    }

    g_sj_assert = nullptr;
}

//
// sj_yield tests
//

DESCRIBE("sj_yield") {
    CheckAssert asserter;

    GIVEN("a null context") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sj_yield(nullptr, nullptr));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a valid context") {
        IT("should switch to that context") {
            auto proc = [](void* executed) {
                *(bool*)executed = true;
                sj_yield(sj_main_context(), nullptr);
            };

            auto executed = false;

            uint8_t stack[SJ_MIN_STACK_SIZE];
            auto context = sj_context_create(stack, sizeof(stack), proc);
            sj_yield(context, &executed);
            sj_context_destroy(context);

            REQUIRE(executed);
        }
    }

    GIVEN("the current context") {
        IT("should return the passed-in value") {
            int marker;
            auto result = sj_yield(sj_current_context(), &marker);
            REQUIRE(result == &marker);
        }
    }

}

//
// sj_current_context tests
//

DESCRIBE("sj_current_context") {

    IT("should return the main context when not context switched") {
        REQUIRE(sj_current_context() == sj_main_context());
    }

    IT("should return the sj_context_t of the currently executing context") {
        auto proc = [](void*) {
            sj_yield(sj_main_context(), sj_current_context());
        };

        uint8_t stack[SJ_MIN_STACK_SIZE];
        auto context = sj_context_create(stack, sizeof(stack), proc);
        auto current = sj_yield(context, nullptr);
        sj_context_destroy(context);

        REQUIRE(context == current);
    }

}

//
// sj_main_context tests
//

DESCRIBE("sj_main_context") {

    IT("should always return the main context") {
        auto proc = [](void*) {
            sj_yield(sj_main_context(), sj_main_context());
        };

        uint8_t stack[SJ_MIN_STACK_SIZE];
        auto context = sj_context_create(stack, sizeof(stack), proc);
        auto main = sj_yield(context, nullptr);
        sj_context_destroy(context);

        REQUIRE(sj_main_context() == main);
    }

}