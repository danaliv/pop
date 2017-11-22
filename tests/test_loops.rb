class TestLoops < Test
    def test_loop
        assert_output "0 begin dup 2 = if break then dup 1 + repeat", "2\n1\n0\n"
        assert_error "begin", "(stdin):1: BEGIN with no REPEAT\n"
        assert_error "break", "(stdin):1: BREAK with no BEGIN\n"
    end

    def test_while_loop
        assert_output "0 begin dup 2 < while dup 1 + repeat", "2\n1\n0\n"
        assert_error "begin while while", "(stdin):1: Multiple WHILE/UNTIL in a single loop\n"
        assert_error "begin while", "(stdin):1: BEGIN with no REPEAT\n"
        assert_error "begin while repeat", "Stack underflow\n"
    end

    def test_until_loop
        assert_output "0 begin dup 2 = until dup 1 + repeat", "2\n1\n0\n"
        assert_error "begin until until", "(stdin):1: Multiple WHILE/UNTIL in a single loop\n"
        assert_error "begin until", "(stdin):1: BEGIN with no REPEAT\n"
        assert_error "begin until repeat", "Stack underflow\n"
    end
end
