//---
// Copyright (c) 2016 Johan Sköld
// License: https://opensource.org/licenses/ISC
//---

#include "framework.hpp"

#include <cstdint> // uint8_t

#include <sc.h>

//
// Assertion testing
//

extern "C" void (* g_sc_assert) ();

struct CheckAssert {
	bool should_throw;
    bool asserted;

    CheckAssert ()
		: should_throw(true)
		, asserted(false)
	{
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

		if (s_instance->should_throw) {
			throw "assertion";
		}
    }
};

CheckAssert* CheckAssert::s_instance;

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

#if !defined(NDEBUG)

    CheckAssert asserter;

    GIVEN("a null stack buffer") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_context_create(nullptr, SC_MIN_STACK_SIZE, empty_proc));
            REQUIRE(asserter.asserted);
        }
    }

    GIVEN("a stack smaller than `SC_MIN_STACK_SIZE`") {
        IT("should assert") {
            uint8_t buffer[SC_MIN_STACK_SIZE - 1];
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_context_create(buffer, sizeof(buffer), empty_proc));
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

#endif // !defined(NDEBUG)

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

#if !defined(NDEBUG)

	CheckAssert asserter;
	asserter.should_throw = false;

    GIVEN("the current context") {
        IT("should assert") {
            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), destroy_current_proc);
            sc_yield(context, nullptr);
            sc_context_destroy(context);

            REQUIRE(asserter.asserted == true);

        }
    }

    GIVEN("the main context") {
        IT("should assert") {
            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), destroy_main_proc);
            sc_yield(context, nullptr);
            sc_context_destroy(context);

            REQUIRE(asserter.asserted == true);
        }
    }

#endif // !defined(NDEBUG)

    GIVEN("a valid context") {
        IT("should not assert") {
            uint8_t stack[SC_MIN_STACK_SIZE];
            auto context = sc_context_create(stack, sizeof(stack), empty_proc);
            sc_context_destroy(context);
        }
    }

    g_sc_assert = nullptr;
}

//
// sc_yield tests
//

DESCRIBE("sc_yield") {

#if !defined(NDEBUG)

    CheckAssert asserter;

    GIVEN("a null context") {
        IT("should assert") {
            REQUIRE(!asserter.asserted);
            CHECK_THROWS(sc_yield(nullptr, nullptr));
            REQUIRE(asserter.asserted);
        }
    }

#endif // !defined(NDEBUG)

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