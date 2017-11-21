class TestLink < Test
    def test_link_pop
        assert_output '.link "./link/lib" lib.sayhi', "hi\nloaded!\n"
        assert_output '.link "./link/lib" .link "./link/doublelink"', "loaded!\n"
    end

    def test_link_so
        assert system("(cd link; make > make.log 2>&1)"), "make failed - check tests/link/make.log"
        assert_output '.link "./link/ext" "hello" ext.test', "532\n"
    end

    def test_search_path
        ENV['POPPATH'] = './nonexistent:./link'
        assert_output '.link "lib"', "loaded!\n"
        ENV.delete('POPPATH')
        assert_error '.link "lib"', "(stdin):1: Link failed\n"
    end
end
