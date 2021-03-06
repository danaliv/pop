class TestControlFlow < Test
    def test_if_else_then
        assert_output '1 if "a" then "b"', "b\na\n"
        assert_output '1 if "a" else "b" then "c"', "c\na\n"
        assert_output '0 if "a" then "b"', "b\n"
        assert_output '0 if "a" else "b" then "c"', "c\nb\n"
        assert_output '1 if "a" 1 if "b" then "c" then "d"', "d\nc\nb\na\n"
        assert_output '0 if "a" 1 if "b" then "c" then "d"', "d\n"
        assert_output '1 if "a" 0 if "b" then "c" then "d"', "d\nc\na\n"
        assert_output '0 if "a" 0 if "b" then "c" then "d"', "d\n"
        assert_output '1 if "a" 1 if "b" else "c" then "d" else "e" then "f"', "f\nd\nb\na\n"
        assert_output '0 if "a" 1 if "b" else "c" then "d" else "e" then "f"', "f\ne\n"
        assert_output '1 if "a" 0 if "b" else "c" then "d" else "e" then "f"', "f\nd\nc\na\n"
        assert_output '0 if "a" 0 if "b" else "c" then "d" else "e" then "f"', "f\ne\n"
        assert_error "if then", "Stack underflow\n"
        assert_error '"hi" if then', "Wrong type(s) on stack\n"
        assert_error "else", "(stdin):1: ELSE with no IF\n"
        assert_error "then", "(stdin):1: THEN with no IF\n"
        assert_error "0 if", "(stdin):1: IF with no THEN\n"
        assert_error "1 if else else", "(stdin):1: ELSE with no IF\n"
    end
end
