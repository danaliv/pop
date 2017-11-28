class TestOS < Test
    def test_getenv
        ENV["POP_TEST_GETENV"] = "góðan daginn"
        assert_output '"POP_TEST_GETENV" getenv if then', "góðan daginn\n"
        ENV.delete("POP_TEST_GETENV")
        assert_output '"POP_TEST_GETENV" getenv', "OPT#none\n"
        assert_error "getenv", "Stack underflow\n"
        assert_error "0 getenv", "Wrong type(s) on stack\n"
    end

    def test_puts
        out, err, st = runpop('"hello world" puts')
        assert_equal("hello world\n", out)
        assert_equal("", err)
        assert(st.success?, "exit status not successful")

        out, err, st = runpop('123 puts')
        assert_equal("123\n", out)
        assert_equal("", err)
        assert(st.success?, "exit status not successful")

        assert_error "puts", "Stack underflow\n"
    end
end
