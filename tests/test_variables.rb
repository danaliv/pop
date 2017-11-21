class TestVariables < Test
    def test_store
        assert_output ".var x 1 x !", ""
    end

    def test_fetch
        assert_output '.var x "hello" x ! x @', "hello\n"
        assert_output ".var x x @", "0\n"
    end

    def test_errors
        assert_error ".var x .var x", "(stdin):1: Duplicate name\n"
        assert_error ": x ; .var x", "(stdin):1: Duplicate name\n"
    end
end
