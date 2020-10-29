#include "core/math.h"
#include "core/util_regex.h"

#include "tests.h"

#include <random>

template<typename Ty>
static bool check_string_list(const Ty& v, std::initializer_list<std::string_view> tst) {
    if (v.size() != tst.size()) { return false; }
    auto it = tst.begin();
    for (const auto& el : v) {
        if (!(el == *it++)) { return false; }
    }
    return true;
}

#define CHECK(...) \
    if (!check_string_list(__VA_ARGS__)) { \
        throw std::logic_error(report_error(__FILE__, __LINE__, "string list mismatched")); \
    }

// --------------------------------------------

static void test_0() {
    CHECK(util::split_string("", util::sfind(',')), {""});
    CHECK(util::split_string(",", util::sfind(',')), {"", ""});
    CHECK(util::split_string("1234,", util::sfind(',')), {"1234", ""});
    CHECK(util::split_string("1234,34646", util::sfind(',')), {"1234", "34646"});
    CHECK(util::split_string(",1234,34646,", util::sfind(',')), {"", "1234", "34646", ""});
    CHECK(util::split_string(",1234,34646,,", util::sfind(',')), {"", "1234", "34646", "", ""});
    CHECK(util::split_string(",1234\\,34646,,", util::sfind(',')), {"", "1234\\,34646", "", ""});
    CHECK(util::split_string(",1234,34646,\\,", util::sfind(',')), {"", "1234", "34646", "\\,"});
    CHECK(util::split_string(",1234,34646,,\\", util::sfind(',')), {"", "1234", "34646", "", "\\"});

    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 0, 0) == "");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 1, 1) == "1234");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 2, 2) == "34646");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 3, 3) == "");

    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 1, 2) == "1234,34646");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 0, 2) == ",1234,34646");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 2) == "34646,,");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 4, 4) == "");
    VERIFY(util::string_section(",1234,34646,,", util::sfind(','), 10) == "");
    VERIFY(util::string_section<util::split_flags::kSkipEmpty>(",1234,,,34646,,", util::sfind(','), 0, 1) ==
           "1234,,,34646");

    VERIFY(util::string_section(",1234,34646,,124", util::rsfind(','), 0) == "124");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 0) == "");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 1, 1) == "");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 2, 2) == "34646");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 2, 1) == "34646,");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 2) == "34646,,");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 3) == "1234,34646,,");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 10, 3) == ",1234");
    VERIFY(util::string_section(",1234,34646,,", util::rsfind(','), 10, 10) == "");
    VERIFY(util::string_section<util::split_flags::kSkipEmpty>(",1234,,,34646,,", util::rsfind(','), 1) ==
           "1234,,,34646");
    VERIFY(util::string_section(",1234\\,34646,,", util::rsfind(','), 3) == ",1234\\,34646,,");

    std::string csv = "forename,middlename,surname,phone";
    std::string path = "/usr/local/bin/myapp";  // First field is empty
    std::string data = "forename**middlename**surname**phone";
    std::string line = "forename\tmiddlename  surname \t \t phone";

    VERIFY(util::string_section(csv, util::sfind(','), 2, 2) == "surname");
    VERIFY(util::string_section(path, util::sfind('/'), 3, 4) == "bin/myapp");
    VERIFY(util::string_section<util::split_flags::kSkipEmpty>(path, util::sfind('/'), 3, 3) == "myapp");

    VERIFY(util::string_section(csv, util::rsfind(','), 2, 1) == "middlename,surname");
    VERIFY(util::string_section(path, util::rsfind('/'), 0) == "myapp");

    VERIFY(util::string_section(data, util::sfind("**"), 2, 2) == "surname");
    VERIFY(util::string_section(data, util::rsfind("**"), 2, 1) == "middlename**surname");

    static const std::regex sep("[ \\t]+");
    VERIFY(util::string_section(line, util::sfind(sep), 2, 2) == "surname");
    VERIFY(util::string_section(line, util::rsfind(sep), 2, 1) == "middlename  surname");
}

static void test_1() {
    CHECK(util::separate_words("", ','), {});
    CHECK(util::separate_words("   ", ','), {});

    CHECK(util::separate_words("   ,", ','), {"", ""});
    CHECK(util::separate_words("   ,   ", ','), {"", ""});
    CHECK(util::separate_words(",", ','), {"", ""});

    CHECK(util::separate_words("234", ','), {"234"});
    CHECK(util::separate_words("  234", ','), {"234"});
    CHECK(util::separate_words("234   ", ','), {"234"});
    CHECK(util::separate_words("  234   ", ','), {"234"});

    CHECK(util::separate_words(",234", ','), {"", "234"});
    CHECK(util::separate_words(" ,234", ','), {"", "234"});
    CHECK(util::separate_words(" , 234", ','), {"", "234"});

    CHECK(util::separate_words(",,234", ','), {"", "", "234"});
    CHECK(util::separate_words(" ,,234", ','), {"", "", "234"});
    CHECK(util::separate_words(", ,234", ','), {"", "", "234"});
    CHECK(util::separate_words(",, 234", ','), {"", "", "234"});
    CHECK(util::separate_words(" , ,234", ','), {"", "", "234"});
    CHECK(util::separate_words(", , 234", ','), {"", "", "234"});
    CHECK(util::separate_words(" , , 234", ','), {"", "", "234"});

    CHECK(util::separate_words("234  64", ','), {"234", "64"});
    CHECK(util::separate_words("234,64", ','), {"234", "64"});
    CHECK(util::separate_words("234 ,64", ','), {"234", "64"});
    CHECK(util::separate_words("234, 64", ','), {"234", "64"});
    CHECK(util::separate_words("234 , 64", ','), {"234", "64"});
    CHECK(util::separate_words("234,,64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234 ,,64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234, ,64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234,, 64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234 , ,64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234, , 64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234 , , 64", ','), {"234", "", "64"});
    CHECK(util::separate_words("234,,,64", ','), {"234", "", "", "64"});

    CHECK(util::separate_words("234,", ','), {"234", ""});
    CHECK(util::separate_words("234 ,", ','), {"234", ""});
    CHECK(util::separate_words("234, ", ','), {"234", ""});
    CHECK(util::separate_words("234 , ", ','), {"234", ""});
    CHECK(util::separate_words("234,,", ','), {"234", "", ""});
    CHECK(util::separate_words("234 ,,", ','), {"234", "", ""});
    CHECK(util::separate_words("234, ,", ','), {"234", "", ""});
    CHECK(util::separate_words("234,, ", ','), {"234", "", ""});
    CHECK(util::separate_words("234 , ,", ','), {"234", "", ""});
    CHECK(util::separate_words("234, , ", ','), {"234", "", ""});
    CHECK(util::separate_words("234 , , ", ','), {"234", "", ""});
    CHECK(util::separate_words("234,,,", ','), {"234", "", "", ""});

    CHECK(util::separate_words("  234 , 64,  8765   ,72346,87  ", ','), {"234", "64", "8765", "72346", "87"});
    CHECK(util::separate_words(",   234 ,  644 ,, 6778,", ','), {"", "234", "644", "", "6778", ""});
    CHECK(util::separate_words("  ,   234 ,  644,   , 6778,   ", ','), {"", "234", "644", "", "6778", ""});
    CHECK(util::separate_words("  ,   234 ,  644 ,   ,6778  ,   ", ','), {"", "234", "644", "", "6778", ""});
    CHECK(util::separate_words("  ,   234 ,  644,,   ,,6778  ,   ", ','), {"", "234", "644", "", "", "", "6778", ""});
    CHECK(util::separate_words("  ,   234\\ ,  644\\,\\,   ,,6778  ,   ", ','),
          {"", "234\\ ", "644\\,\\,", "", "6778", ""});
}

static void test_2() {
    CHECK(util::unpack_strings("", ';'), {});
    CHECK(util::unpack_strings(";", ';'), {""});
    CHECK(util::unpack_strings("12;3", ';'), {"12", "3"});
    CHECK(util::unpack_strings("12;3;", ';'), {"12", "3"});
    CHECK(util::unpack_strings("12;3;456", ';'), {"12", "3", "456"});
    CHECK(util::unpack_strings(";12;3;456", ';'), {"", "12", "3", "456"});
    CHECK(util::unpack_strings(";;12;3;;456;;", ';'), {"", "", "12", "3", "", "456", ""});

    CHECK(util::unpack_strings("12\\\\323\\;64567;434553;", ';'), {"12\\323;64567", "434553"});
    CHECK(util::unpack_strings("12\\\\323\\;64567;434553;;;", ';'), {"12\\323;64567", "434553", "", ""});
    CHECK(util::unpack_strings("12\\\\323\\;64567;434553;\\", ';'), {"12\\323;64567", "434553"});
    CHECK(util::unpack_strings("12\\\\323\\;64567;434553;\\\\", ';'), {"12\\323;64567", "434553", "\\"});
    CHECK(util::unpack_strings("12\\\\323\\;\\\\64567;434553\\\\;", ';'), {"12\\323;\\64567", "434553\\"});

    VERIFY(util::pack_strings(util::unpack_strings("12\\\\323\\;64567;434553;\\", ';'), ';') ==
           "12\\\\323\\;64567;434553");
    VERIFY(util::pack_strings(util::unpack_strings("12\\\\323\\;64567;434553;;", ';'), ';') ==
           "12\\\\323\\;64567;434553;;");
}

static void test_3() {
    VERIFY(util::trim_string("asdf") == "asdf");
    VERIFY(util::trim_string("   asdf") == "asdf");
    VERIFY(util::trim_string("   asdf  ") == "asdf");
}

static void test_4() {
    VERIFY(util::sformat("abcdefghi").str() == "abcdefghi");
    VERIFY(util::sformat("%1abcdefghi").arg("A").str() == "Aabcdefghi");
    VERIFY(util::sformat("%1abcdefghi%2").arg("A").arg("B").str() == "AabcdefghiB");
    VERIFY(util::sformat("%1%3abcdefghi%2").arg("A").arg("B").arg("C").str() == "ACabcdefghiB");
    VERIFY(util::sformat("%1%3abcdefghi%2%").arg("A").arg("B").arg("C").str() == "ACabcdefghiB");
    VERIFY(util::sformat("%1%3%abcdefghi%2%").arg("A").arg("B").arg("C").str() == "ACabcdefghiB");
    VERIFY(util::sformat("%1%3%4abcdefghi%2%").arg("A").arg("B").arg("C").arg("D").str() == "ACDabcdefghiB");
    VERIFY(util::sformat("%1%3%4%%abcdefghi%2%%").arg("A").arg("B").arg("C").arg("D").str() == "ACD%abcdefghiB%");

    VERIFY(util::sformat("%1%2%3%4").arg("1").arg("2").arg("3").arg("4").str() == "1234");
    VERIFY(util::sformat("%4%3%2%1").arg("1").arg("2").arg("3").arg("4").str() == "4321");
    VERIFY(util::sformat("%4%3%2%1").arg("1").arg("2").str() == "21");
    VERIFY(util::sformat("%4%3%2%1").str() == "");
    VERIFY(util::sformat("%4%1%3%2").arg("1").arg("2").arg("3").arg("4").str() == "4132");
    VERIFY(util::sformat("%1").arg(1, util::sfield(8, '*')).str() == "*******1");
    VERIFY(util::sformat("%1").arg(2.34, util::sfield(8, '*'), util::scvt_fp::kFixed, 2).str() == "****2.34");
    VERIFY(util::sformat("%1").arg(2.34, util::scvt_fp::kFixed, 2).str() == "2.34");
}

static void test_5() {
    char ch[4];
    uint32_t code = 0;

    VERIFY(util::to_utf8(0x7E, ch) == 1);
    VERIFY(ch[0] == '\x7E');
    VERIFY(util::from_utf8(ch, (char*)ch, &code) == (char*)ch);
    VERIFY(util::from_utf8(ch, (char*)ch + 1, &code) == (char*)ch + 1);
    VERIFY(code == 0x7E);

    VERIFY(util::to_utf8(0x7BC, ch) == 2);
    VERIFY(ch[0] == '\xDE');
    VERIFY(ch[1] == '\xBC');
    VERIFY(util::from_utf8(ch, (char*)ch + 1, &code) == (char*)ch);
    VERIFY(util::from_utf8(ch, (char*)ch + 2, &code) == (char*)ch + 2);
    VERIFY(code == 0x7BC);

    VERIFY(util::to_utf8(0xEF38, ch) == 3);
    VERIFY(ch[0] == '\xEE');
    VERIFY(ch[1] == '\xBC');
    VERIFY(ch[2] == '\xB8');
    VERIFY(util::from_utf8(ch, (char*)ch + 2, &code) == (char*)ch);
    VERIFY(util::from_utf8(ch, (char*)ch + 3, &code) == (char*)ch + 3);
    VERIFY(code == 0xEF38);

    VERIFY(util::to_utf8(0x10EF38, ch) == 4);
    VERIFY(ch[0] == '\xF4');
    VERIFY(ch[1] == '\x8E');
    VERIFY(ch[2] == '\xBC');
    VERIFY(ch[3] == '\xB8');
    VERIFY(util::from_utf8(ch, (char*)ch + 3, &code) == (char*)ch);
    VERIFY(util::from_utf8(ch, (char*)ch + 4, &code) == (char*)ch + 4);
    VERIFY(code == 0x10EF38);

    VERIFY(util::to_utf8(0x110000, ch) == 3);
    VERIFY(ch[0] == '\xEF');
    VERIFY(ch[1] == '\xBF');
    VERIFY(ch[2] == '\xBD');
    VERIFY(util::from_utf8(ch, (char*)ch + 3, &code) == (char*)ch + 3);
    VERIFY(code == 0xFFFD);
}

static void test_6() {
    wchar_t ch[2];
    uint32_t code = 0;

    VERIFY(util::to_utf16(0xD7FE, ch) == 1);
    VERIFY(ch[0] == static_cast<wchar_t>(0xD7FE));
    VERIFY(util::from_utf16(ch, (wchar_t*)ch, &code) == (wchar_t*)ch);
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 1, &code) == (wchar_t*)ch + 1);
    VERIFY(code == 0xD7FE);

    VERIFY(util::to_utf16(0xEEEE, ch) == 1);
    VERIFY(ch[0] == static_cast<wchar_t>(0xEEEE));
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 1, &code) == (wchar_t*)ch + 1);
    VERIFY(code == 0xEEEE);

    VERIFY(util::to_utf16(0xDCFE, ch) == 1);
    VERIFY(ch[0] == static_cast<wchar_t>(0xFFFD));
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 1, &code) == (wchar_t*)ch + 1);
    VERIFY(code == 0xFFFD);

    VERIFY(util::to_utf16(0x110000, ch) == 1);
    VERIFY(ch[0] == static_cast<wchar_t>(0xFFFD));
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 1, &code) == (wchar_t*)ch + 1);
    VERIFY(code == 0xFFFD);

    VERIFY(util::to_utf16(0x10FBFC, ch) == 2);
    VERIFY(ch[0] == static_cast<wchar_t>(0xDBFE));
    VERIFY(ch[1] == static_cast<wchar_t>(0xDFFC));
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 1, &code) == (wchar_t*)ch);
    VERIFY(util::from_utf16(ch, (wchar_t*)ch + 2, &code) == (wchar_t*)ch + 2);
    VERIFY(code == 0x10FBFC);
}

static void test_7() {
    VERIFY(util::from_utf8_to_wide(
               "\xD0\x94\xD0\xBE\xD0\xB1\xD1\x80\xD1\x8B\xD0\xB9\x20\xD0\xB4\xD0\xB5\xD0\xBD\xD1\x8C\x21") ==
           L"\x0414\x043e\x0431\x0440\x044b\x0439\x0020\x0434\x0435\x043d\x044c\x0021");
    VERIFY(util::from_wide_to_utf8(L"\x0414\x043e\x0431\x0440\x044b\x0439\x0020\x0434\x0435\x043d\x044c\x0021") ==
           "\xD0\x94\xD0\xBE\xD0\xB1\xD1\x80\xD1\x8B\xD0\xB9\x20\xD0\xB4\xD0\xB5\xD0\xBD\xD1\x8C\x21");
    VERIFY(util::from_utf8_to_wide("\xE4\xB8\x8B\xE5\x8D\x88\xE5\xA5\xBD") == L"\x4e0b\x5348\x597d");
    VERIFY(util::from_wide_to_utf8(L"\x4e0b\x5348\x597d") == "\xE4\xB8\x8B\xE5\x8D\x88\xE5\xA5\xBD");
}

static void test_8() {
    VERIFY(util::encode_escapes("1234\\467;;", "\\;", "\\;") == "1234\\\\467\\;\\;");
    VERIFY(util::decode_escapes("1234\\\\467\\;\\;", "", "") == "1234\\467;;");
    VERIFY(util::decode_escapes("1234\\\\467\\;\\;\\", "", "") == "1234\\467;;");
    VERIFY(util::decode_escapes("1234\\\\467\\;\\;\\", "", "") == "1234\\467;;");
    VERIFY(util::decode_escapes("\\n\\n1234\\\\467\\;\\;\\", "\n", "n") == "\n\n1234\\467;;");
}

static void test_9() {
    VERIFY(util::replace_strings("1234***2345***678", util::sfind("***"), "abcdef") == "1234abcdef2345abcdef678");
    VERIFY(util::replace_strings("1234***2345***678***", util::sfind("***"), "abcdef") ==
           "1234abcdef2345abcdef678abcdef");
    VERIFY(util::replace_strings("***1234***2345***678***", util::sfind("***"), "abcdef") ==
           "abcdef1234abcdef2345abcdef678abcdef");
}

static void test_10() {
    std::string s;

    VERIFY((s = util::to_string<double>(1.2345672222, util::scvt_fp::kFixed)) == "1.234567");
    VERIFY((s = util::to_string<double>(1.2345677777, util::scvt_fp::kFixed)) == "1.234568");
    VERIFY((s = util::to_string<double>(1.2345, util::scvt_fp::kFixed)) == "1.234500");
    VERIFY((s = util::to_string<double>(0.12345, util::scvt_fp::kFixed)) == "0.123450");
    VERIFY((s = util::to_string<double>(0.00124, util::scvt_fp::kFixed)) == "0.001240");
    VERIFY((s = util::to_string<double>(23.34563444, util::scvt_fp::kFixed)) == "23.345634");
    VERIFY((s = util::to_string<double>(23.34563464, util::scvt_fp::kFixed)) == "23.345635");
    VERIFY((s = util::to_string<double>(23.3456, util::scvt_fp::kFixed)) == "23.345600");
    VERIFY((s = util::to_string<double>(999.9999997, util::scvt_fp::kFixed)) == "1000.000000");
    VERIFY((s = util::to_string<double>(0, util::scvt_fp::kFixed)) == "0.000000");
    VERIFY((s = util::to_string<double>(0, util::scvt_fp::kFixed, 0)) == "0");

    VERIFY((s = util::to_string<double>(1000000000, util::scvt_fp::kFixed)) == "1000000000.000000");
    VERIFY((s = util::to_string<double>(231000000000000, util::scvt_fp::kFixed)) == "231000000000000.000000");

    VERIFY((s = util::to_string<double>(345., util::scvt_fp::kFixed)) == "345.000000");
    VERIFY((s = util::to_string<double>(345.234465876, util::scvt_fp::kFixed)) == "345.234466");
    VERIFY((s = util::to_string<double>(34.5234465876, util::scvt_fp::kFixed)) == "34.523447");
    VERIFY((s = util::to_string<double>(3.45234465876, util::scvt_fp::kFixed)) == "3.452345");
    VERIFY((s = util::to_string<double>(0.345234465876, util::scvt_fp::kFixed)) == "0.345234");
    VERIFY((s = util::to_string<double>(0.0234465876, util::scvt_fp::kFixed)) == "0.023447");
    VERIFY((s = util::to_string<double>(0.00234465876, util::scvt_fp::kFixed)) == "0.002345");
    VERIFY((s = util::to_string<double>(0.000234465876, util::scvt_fp::kFixed)) == "0.000234");
    VERIFY((s = util::to_string<double>(0.0000234465876, util::scvt_fp::kFixed)) == "0.000023");
    VERIFY((s = util::to_string<double>(0.00000234465876, util::scvt_fp::kFixed)) == "0.000002");
    VERIFY((s = util::to_string<double>(0.000000234465876, util::scvt_fp::kFixed)) == "0.000000");

    VERIFY((s = util::to_string<double>(345., util::scvt_fp::kFixed, 0)) == "345");
    VERIFY((s = util::to_string<double>(345.234465876, util::scvt_fp::kFixed, 0)) == "345");
    VERIFY((s = util::to_string<double>(34.5234465876, util::scvt_fp::kFixed, 0)) == "35");
    VERIFY((s = util::to_string<double>(3.45234465876, util::scvt_fp::kFixed, 0)) == "3");
    VERIFY((s = util::to_string<double>(0.345234465876, util::scvt_fp::kFixed, 0)) == "0");
    VERIFY((s = util::to_string<double>(0.0234465876, util::scvt_fp::kFixed, 0)) == "0");

    VERIFY((s = util::to_string<double>(0, util::scvt_fp::kScientific)) == "0.000000e+00");
    VERIFY((s = util::to_string<double>(0, util::scvt_fp::kScientific, 0)) == "0e+00");
    VERIFY((s = util::to_string<double>(0, util::scvt_fp::kGeneral)) == "0");

    VERIFY((s = util::to_string<double>(999999.9999996, util::scvt_fp::kGeneral)) == "1e+06");

    VERIFY((s = util::to_string<double>(0.003, util::scvt_fp::kGeneral)) == "0.003");
    VERIFY((s = util::to_string<double>(0.003000001, util::scvt_fp::kGeneral)) == "0.003");
    VERIFY((s = util::to_string<double>(0.00300001, util::scvt_fp::kGeneral)) == "0.00300001");
    VERIFY((s = util::to_string<double>(0.0030001, util::scvt_fp::kGeneral)) == "0.0030001");
    VERIFY((s = util::to_string<double>(0.003001, util::scvt_fp::kGeneral)) == "0.003001");

    VERIFY((s = util::to_string<double>(0.3, util::scvt_fp::kGeneral)) == "0.3");
    VERIFY((s = util::to_string<double>(0.3000001, util::scvt_fp::kGeneral)) == "0.3");
    VERIFY((s = util::to_string<double>(0.300001, util::scvt_fp::kGeneral)) == "0.300001");
    VERIFY((s = util::to_string<double>(0.30001, util::scvt_fp::kGeneral)) == "0.30001");
    VERIFY((s = util::to_string<double>(0.3001, util::scvt_fp::kGeneral)) == "0.3001");

    VERIFY((s = util::to_string<double>(345., util::scvt_fp::kGeneral)) == "345");
    VERIFY((s = util::to_string<double>(345.0001, util::scvt_fp::kGeneral)) == "345");
    VERIFY((s = util::to_string<double>(345.001, util::scvt_fp::kGeneral)) == "345.001");
    VERIFY((s = util::to_string<double>(345.01, util::scvt_fp::kGeneral)) == "345.01");
    VERIFY((s = util::to_string<double>(345.1, util::scvt_fp::kGeneral)) == "345.1");

    VERIFY((s = util::to_string<double>(3.e23, util::scvt_fp::kGeneral)) == "3e+23");
    VERIFY((s = util::to_string<double>(3.000001e23, util::scvt_fp::kGeneral)) == "3e+23");
    VERIFY((s = util::to_string<double>(3.00001e23, util::scvt_fp::kGeneral)) == "3.00001e+23");
    VERIFY((s = util::to_string<double>(3.0001e23, util::scvt_fp::kGeneral)) == "3.0001e+23");
    VERIFY((s = util::to_string<double>(3.001e23, util::scvt_fp::kGeneral)) == "3.001e+23");

    VERIFY((s = util::to_string<double>(345., util::scvt_fp::kGeneral, 0)) == "3e+02");
    VERIFY((s = util::to_string<double>(345.234465876, util::scvt_fp::kGeneral, 0)) == "3e+02");
    VERIFY((s = util::to_string<double>(34.5234465876, util::scvt_fp::kGeneral, 0)) == "3e+01");
    VERIFY((s = util::to_string<double>(3.45234465876, util::scvt_fp::kGeneral, 0)) == "3");
    VERIFY((s = util::to_string<double>(0.345234465876, util::scvt_fp::kGeneral, 0)) == "0.3");
    VERIFY((s = util::to_string<double>(0.0234465876, util::scvt_fp::kGeneral, 0)) == "0.02");

    VERIFY((s = util::to_string<double>(0.0000006, util::scvt_fp::kFixed)) == "0.000001");
    VERIFY((s = util::to_string<double>(0.000000500001, util::scvt_fp::kFixed)) == "0.000001");
    VERIFY((s = util::to_string<double>(0.0000004, util::scvt_fp::kFixed)) == "0.000000");
    VERIFY((s = util::to_string<double>(0.00000049999, util::scvt_fp::kFixed)) == "0.000000");
    VERIFY((s = util::to_string<double>(0.00000006, util::scvt_fp::kFixed)) == "0.000000");

    double d = 2000000. / 7.;  // 285714.28571428573923185468

    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 17)) == "2.85714285714285739e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 16)) == "2.8571428571428574e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 15)) == "2.857142857142857e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 13)) == "2.8571428571429e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 2)) == "2.86e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 1)) == "2.9e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 0)) == "3e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 18)) == "2.857142857142857390e+05");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kScientific, 20)) == "2.85714285714285739000e+05");

    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 12)) == "285714.285714285739");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 11)) == "285714.28571428574");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 10)) == "285714.2857142857");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 8)) == "285714.28571429");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 2)) == "285714.29");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 1)) == "285714.3");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 0)) == "285714");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 13)) == "285714.2857142857390");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 15)) == "285714.285714285739000");

    d = 285714.;

    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 8)) == "285714.00000000");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 2)) == "285714.00");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 1)) == "285714.0");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 0)) == "285714");

    d = 285714e+20;  // 28571400000000001222639616.00000000

    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 8)) == "28571400000000001200000000.00000000");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 2)) == "28571400000000001200000000.00");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 1)) == "28571400000000001200000000.0");
    VERIFY((s = util::to_string<double>(d, util::scvt_fp::kFixed, 0)) == "28571400000000001200000000");

    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 7)) == "0.0006382");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 6)) == "0.000638");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 5)) == "0.00064");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 4)) == "0.0006");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 3)) == "0.001");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 2)) == "0.00");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 1)) == "0.0");
    VERIFY((s = util::to_string<double>(0.0006382, util::scvt_fp::kFixed, 0)) == "0");

    VERIFY((s = util::to_string<double>(0.6382, util::scvt_fp::kFixed, 4)) == "0.6382");
    VERIFY((s = util::to_string<double>(0.6382, util::scvt_fp::kFixed, 3)) == "0.638");
    VERIFY((s = util::to_string<double>(0.6382, util::scvt_fp::kFixed, 2)) == "0.64");
    VERIFY((s = util::to_string<double>(0.6382, util::scvt_fp::kFixed, 1)) == "0.6");
    VERIFY((s = util::to_string<double>(0.6382, util::scvt_fp::kFixed, 0)) == "1");

    VERIFY((s = util::to_string<double>(0.0004382, util::scvt_fp::kFixed, 3)) == "0.000");
    VERIFY((s = util::to_string<double>(0.4382, util::scvt_fp::kFixed, 0)) == "0");

    VERIFY(util::to_string<int>(10) == "10");
    VERIFY(util::to_string<int>(-25510) == "-25510");

    VERIFY(util::to_string<unsigned>(10) == "10");
    VERIFY(util::to_string<unsigned>(-25510) == "4294941786");

    VERIFY(fabs(util::from_string<float>(util::to_string<float>(0.2355f)) - 0.2355f) < 0.000001);
    VERIFY(fabs(util::from_string<double>(util::to_string<double>(-123.56e-1)) - -123.56e-1) < 0.000001);

    VERIFY(util::to_string<double>(std::numeric_limits<double>::infinity()) == "inf");
    VERIFY(util::to_string<double>(-std::numeric_limits<double>::infinity()) == "-inf");
    VERIFY(util::to_string<double>(std::numeric_limits<double>::quiet_NaN()).find("nan") != std::string::npos);

    VERIFY(std::isinf(util::from_string<double>("inf")));
    VERIFY(std::isinf(util::from_string<double>("+inf")));
    VERIFY(std::isinf(-util::from_string<double>("-inf")));
    VERIFY(std::isnan(util::from_string<double>("nan")));
    VERIFY(std::isnan(util::from_string<double>("+nan")));
    VERIFY(std::isnan(util::from_string<double>("-nan")));

    VERIFY(util::from_string<int>(" \t 10") == 10);
    VERIFY(util::from_string<int>("  \t  -25510") == -25510);
    VERIFY(util::from_string<int>("  \t  +2510") == 2510);

    VERIFY(util::from_string<unsigned>(" \t  10") == 10);
    VERIFY(util::from_string<unsigned>("\t    25510") == 25510);

    VERIFY(fabs(util::from_string<float>(" \t 0.2355") - 0.2355f) < 0.000001);
    VERIFY(fabs(util::from_string<float>(" \t .2355") - 0.2355f) < 0.000001);
    VERIFY(fabs(util::from_string<float>(" \t .3") - 0.3f) < 0.000001);
    VERIFY(fabs(util::from_string<double>("   \t  -123.56e-1") - -123.56e-1) < 0.000001);
    VERIFY(fabs(util::from_string<double>("   \t  -123.56e-0047") - -123.56e-47) < 0.000001);

    VERIFY(util::from_string<bool>(" \t   true") == true);
    VERIFY(util::from_string<bool>(" \t   false") == false);
    VERIFY(util::from_string<bool>(" \t   1") == true);
    VERIFY(util::from_string<bool>(" \t   001") == true);
    VERIFY(util::from_string<bool>(" \t   000") == false);

    std::vector<std::tuple<std::string_view, int, double>> d_tst;
    d_tst.emplace_back(" \t   fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   +fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   -fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   .fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   +.fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   -.fhjjh", 5, 12345.);
    d_tst.emplace_back(" \t   +", 5, 12345.);
    d_tst.emplace_back(" \t   -", 5, 12345.);
    d_tst.emplace_back(" \t   .", 5, 12345.);
    d_tst.emplace_back(" \t   +.", 5, 12345.);
    d_tst.emplace_back(" \t   -.", 5, 12345.);
    d_tst.emplace_back(" \t   .123fhjjh", 9, 0.123);
    d_tst.emplace_back(" \t   +.123fhjjh", 10, 0.123);
    d_tst.emplace_back(" \t   -.123fhjjh", 10, -0.123);
    d_tst.emplace_back(" \t   123wert", 8, 123.);
    d_tst.emplace_back(" \t   +123wert", 9, 123.);
    d_tst.emplace_back(" \t   -123wert", 9, -123.);
    d_tst.emplace_back(" \t   123.wert", 9, 123.);
    d_tst.emplace_back(" \t   123.0wert", 10, 123.);
    d_tst.emplace_back(" \t   123.1wert", 10, 123.1);
    d_tst.emplace_back(" \t   123.1esd", 10, 123.1);
    d_tst.emplace_back(" \t   123.1e1sd", 12, 1231.);

    for (const auto& el : d_tst) {
        double d = 12345;
        VERIFY(util::string_converter<double>::from_string(std::get<0>(el), d) ==
                   std::get<0>(el).data() + std::get<1>(el) &&
               d == std::get<2>(el));
    }

    std::vector<std::tuple<std::string_view, int, int>> i_tst;
    i_tst.emplace_back(" \t   fhjjh", 5, 12345);
    i_tst.emplace_back(" \t   +fhjjh", 5, 12345);
    i_tst.emplace_back(" \t   -fhjjh", 5, 12345);
    i_tst.emplace_back(" \t   +", 5, 12345);
    i_tst.emplace_back(" \t   -", 5, 12345);
    i_tst.emplace_back(" \t   123wert", 8, 123);
    i_tst.emplace_back(" \t   +123wert", 9, 123);
    i_tst.emplace_back(" \t   -123wert", 9, -123);

    for (const auto& el : i_tst) {
        double i = 12345;
        VERIFY(util::string_converter<double>::from_string(std::get<0>(el), i) ==
                   std::get<0>(el).data() + std::get<1>(el) &&
               i == std::get<2>(el));
    }
}

#if defined(_MSC_VER) && (__cplusplus >= 201703L)
#    include <charconv>
#endif

static void test_11() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(1., 10.);
    std::uniform_int_distribution<int> exp_distribution(-324, 308);

    const int N = 1000000000;

    double eps = 0;
    int N_err = 0;

    // auto start = std::clock();

    std::cout << "  0.0%" << std::flush;
    for (int iter = 0, perc0 = 0; iter < N; ++iter) {
        int perc = (1000 * static_cast<int64_t>(iter)) / N;
        if (perc > perc0) {
            std::cout << "\b\b\b\b\b\b" << std::setw(3) << (perc / 10) << "." << std::setw(0) << (perc % 10) << "%"
                      << std::flush;
            perc0 = perc;
        }

        const int prec = 18;

        auto val = distribution(generator) * std::pow(10, exp_distribution(generator));

        char buf[128];
#if defined(_MSC_VER) && (__cplusplus >= 201703L)
        auto result = std::to_chars(buf, buf + sizeof(buf), val, std::chars_format::general, prec);
        std::string s_ref(buf, result.ptr);
#else
        sprintf(buf, "%.*g", prec, val);
        std::string s_ref(buf);
#endif

        auto s = util::to_string(val, util::scvt_fp::kGeneral, prec);
        // if (s == "inf") { printf("inf\n"); }
        if (s != s_ref && s.find('e') != std::string::npos) {
            auto u64 = *reinterpret_cast<uint64_t*>(&val);
            int exp = static_cast<int>((u64 >> 52) & 2047) - 1023;
            u64 &= (1ull << 52) - 1;
            std::cout << std::endl << "iter = " << iter << std::endl;
            std::cout << s << std::endl << s_ref << std::endl;
            std::cout << "mantissa = " << u64 << std::endl;
            std::cout << "exp = " << exp << std::endl;
            VERIFY(++N_err < 50);
        }

        double val1 = 0;
        const char* p = util::string_converter<double>::from_string(s, val1);
        VERIFY(p == s.data() + s.size());
        eps = std::max(std::fabs((val - val) / val), eps);
        if (eps >= 1.e-15) {
            std::cout << std::endl << s << std::endl;
            printf("%.*g\n", prec, val1);
            printf("%.*g\n", prec, val);
            VERIFY(false);
        }
    }

    printf("\neps = %.6e\n", eps);
    std::cout << "\b\b\b\b\b\b" << std::flush;
    // std::cout << (std::clock() - start) << std::endl;
}

static void test_12() {
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(1., 10.);
    std::uniform_int_distribution<int> exp_distribution(-30, 50);

    const int N = 1000000000;

    double eps = 0;

    std::cout << "  0.0%" << std::flush;
    for (int iter = 0, perc0 = 0; iter < N; ++iter) {
        int perc = (1000 * static_cast<int64_t>(iter)) / N;
        if (perc > perc0) {
            std::cout << "\b\b\b\b\b\b" << std::setw(3) << (perc / 10) << "." << std::setw(0) << (perc % 10) << "%"
                      << std::flush;
            perc0 = perc;
        }

        const int prec = 50;

        auto val = distribution(generator) * std::pow(10, exp_distribution(generator));
        auto s = util::to_string(val, util::scvt_fp::kFixed, prec);

        double val1 = 0;
        const char* p = util::string_converter<double>::from_string(s, val1);
        VERIFY(p == s.data() + s.size());
        VERIFY(s.find('e') == std::string::npos);
        eps = std::max(std::fabs((val - val1) / val), eps);
        if (eps >= 1.e-15) {
            std::cout << std::endl << s << std::endl;
            printf("%.*f\n", prec, val1);
            printf("%.*f\n", prec, val);
            VERIFY(false);
        }
    }

    printf("\neps = %.6e\n", eps);
    std::cout << "\b\b\b\b\b\b" << std::flush;
}

static void test_13() {
    util::variant v{std::string{"1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"}};

    VERIFY(v.can_convert(util::variant_id::kMatrix4x4) == true);
    v.convert(util::variant_id::kMatrix4x4);
    VERIFY(v.value<vrc::math::mat4>() == vrc::math::mat4());

    VERIFY(v.can_convert(util::variant_id::kString) == true);
    v.convert(util::variant_id::kString);
    VERIFY(v.value<std::string>() == "1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1")

    util::variant v1{-234.57};

    VERIFY(v1.value<int>() == -234);
    VERIFY(v1.value<unsigned>() == std::numeric_limits<unsigned>::max() - 233u);
    VERIFY(v1.value<double>() == -234.57);
    VERIFY(v1.value<std::string>() == "-234.57");

    v1.convert(util::variant_id::kString);

    auto v2 = std::move(v1);
    VERIFY(v2.value<std::string>() == "-234.57");

    auto v3 = v2;
    VERIFY(v3.value<std::string>() == "-234.57");
}

// --------------------------------------------

std::pair<std::pair<size_t, void (*)()>*, size_t> get_string_tests() {
    static std::pair<size_t, void (*)()> _tests[] = {
        {0, test_0}, {1, test_1}, {2, test_2}, {3, test_3},   {4, test_4},   {5, test_5},   {6, test_6},
        {7, test_7}, {8, test_8}, {9, test_9}, {10, test_10}, {11, test_11}, {12, test_12}, {12, test_13},
    };

    return std::make_pair(_tests, sizeof(_tests) / sizeof(_tests[0]));
}
