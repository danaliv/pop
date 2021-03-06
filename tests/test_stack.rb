class TestStack < Test
    def test_pop
        assert_output "1 2 pop", "1\n"
        assert_error "pop", "Stack underflow\n"
    end

    def test_swap
        assert_output "1 2 swap", "1\n2\n"
        assert_error "1 swap", "Stack underflow\n"
    end

    def test_dup
        assert_output "1 dup", "1\n1\n"
        assert_error "dup", "Stack underflow\n"
    end

    def test_rot
        assert_output "1 2 3 rot", "1\n3\n2\n"
        assert_error "1 2 rot", "Stack underflow\n"
    end

    def test_rotate
        assert_output "1 2 3 4 5 4 rotate", "2\n5\n4\n3\n1\n"
        assert_error "1 2 3 4 5 6 rotate", "Stack underflow\n"
        assert_error '1 2 3 4 5 "4" rotate', "Wrong type(s) on stack\n"
        assert_error "rotate", "Stack underflow\n"
        assert_error "1 2 3 4 5 -1 rotate", "Integer value out of range\n"
    end

    def test_over
        assert_output "1 2 over", "1\n2\n1\n"
        assert_error "1 over", "Stack underflow\n"
    end

    def test_pick
        assert_output "1 2 3 4 5 4 pick", "2\n5\n4\n3\n2\n1\n"
        assert_error "1 2 3 4 5 6 pick", "Stack underflow\n"
        assert_error '1 2 3 4 5 "4" pick', "Wrong type(s) on stack\n"
        assert_error "pick", "Stack underflow\n"
        assert_error "1 2 3 4 5 -1 pick", "Integer value out of range\n"
    end
end
