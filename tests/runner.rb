require 'open3'
require 'minitest/autorun'

Dir.chdir(File.dirname(__FILE__))

def runpop(ptxt)
    return Open3.capture3('../pop', :stdin_data => ptxt)
end

class Test < Minitest::Test
    def assert_output(ptxt, exp)
        out, err, st = runpop(ptxt + ' DEBUG_puts_all')
        assert_equal("", err)
        assert_equal(exp, out)
        assert(st.success?, "exit status not successful")
    end

    def assert_error(ptxt, exp)
        out, err, st = runpop(ptxt)
        assert_equal("", out)
        assert_equal(exp, err)
        assert(!st.success?, "exit status successful")
    end
end

Dir.glob('test_*.rb').each do |file|
    require(File.join('.', file))
end
