class TestMath < Test
    def test_add
        assert_output "1 2 +", "3\n"
    end

    def test_subtract
        assert_output "1 2 -", "-1\n"
    end

    def test_multiply
        assert_output "2 4 *", "8\n"
    end

    def test_divide
        assert_output "12 3 /", "4\n"
        assert_error "1 0 /", "Division by zero\n"
    end
end
