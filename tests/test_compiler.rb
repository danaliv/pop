class TestCompiler < Test
    def test_definition
        assert_output ': hello "hello world" . ;', ""
        assert_output ': hello "hello world" . ; hello', "hello world\n"
        assert_error ":", "(stdin):1: Unterminated word definition\n"
        assert_error ': hello "hello world" .', "(stdin):1: Unterminated word definition\n"
        assert_error ": hello :", "(stdin):1: Can't use : inside a word definition\n"
        assert_error ";", "(stdin):1: Can't use ; outside a word definition\n"
        assert_error ": x ; : x ;", "(stdin):1: Duplicate name\n"
        assert_error ".var x : x ;", "(stdin):1: Duplicate name\n"
    end

    def test_string
        assert_output '"hello"', "hello\n"
        assert_output '"\"hello\" world"', "\"hello\" world\n"
        assert_output '"hello\nworld"', "hello\nworld\n"
        assert_error '"hello', "(stdin):1: String has no end quote\n"
        assert_error '"hello".', "(stdin):1: No space after string\n"
    end

    def test_unknown_word
        assert_error "blorp", "(stdin):1: Unrecognized word\n"
    end

    def test_comment
        assert_output "1 2 ( this is a comment ) 3 4", "4\n3\n2\n1\n"
        assert_output "1 2 ( this is\na comment ) 3 4", "4\n3\n2\n1\n"
    end
end
