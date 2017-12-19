class TestArray < Test
	def test_array
		assert_output '{ "hello" 9 }array', "hello\n9\n"
		assert_output '{ }array', ""
		assert_error '"hello" 9 }array', "Stack underflow\n"
	end
end
