class TestComparators < Test
    def test_equality
        assert_output '"a" "a" =', "1\n"
        assert_output '"a" "A" =', "0\n"
        assert_output '"a" "b" =', "0\n"
        assert_output '"4" 4 =', "0\n"
        assert_output '4 4 =', "1\n"
        assert_output '4 "4" =', "0\n"
        assert_output '4 2 =', "0\n"
        assert_output '.var x x x =', "1\n"
        assert_output '.var x x 0 =', "0\n"
        assert_error '=', "Not enough items on the stack\n"
        assert_error '1 =', "Not enough items on the stack\n"
    end
end
