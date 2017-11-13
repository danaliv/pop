class TestCompiler < Test
    def test_definition
        assert_output ': hello "hello world" . ;', ""
    end

    def test_call
        assert_output ': hello "hello world" . ; hello', "hello world\n"
    end

    def test_dangling_definition
        assert_error ":", "Program ends abruptly\n"
        assert_error ': hello "hello world" .', "Program ends abruptly\n"
    end

    def test_nested_definition
        assert_error ": hello :", "line 1: Can't use : inside a word definition\n"
    end

    def test_stray_semicolon
        assert_error ";", "line 1: Can't use ; outside a word definition\n"
    end

    def test_dangling_string
        assert_error '"hello', "line 1: String has no end quote\n"
    end

    def test_unseparated_string
        assert_error '"hello".', "line 1: No space after string\n"
    end

    def test_unknown_word
        assert_error "blorp", "line 1: Unrecognized word\n"
    end
end
