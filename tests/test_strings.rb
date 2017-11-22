class TestStrings < Test
    def test_strlen
        assert_output '"hello" strlen', "5\n"
        assert_error "5 strlen", "Wrong type(s) on stack\n"
        assert_error "strlen", "Stack underflow\n"
    end

    def test_strcat
        assert_output '"hello" "world" strcat', "helloworld\n"
        assert_error '5 "world" strcat', "Wrong type(s) on stack\n"
        assert_error '"hello" 5 strcat', "Wrong type(s) on stack\n"
        assert_error '"hello" strcat', "Stack underflow\n"
        assert_error "strcat", "Stack underflow\n"
    end
end
