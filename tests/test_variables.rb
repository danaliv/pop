class TestVariables < Test
    def test_store
        assert_output ".var x 1 x !", ""
    end

    def test_fetch
        assert_output '.var x "hello" x ! x @', "hello\n"
        assert_output ".var x x @", "0\n"
    end
end
