#pragma once

#include "stream.h"

#include <array>
#include <unordered_map>

namespace util {

enum class xml_parser_token : int {
    kEof = 0,
    kPlainText,
    kSection,
    kEndOfSection,
    kDeclaration,
    kDoctype,
    kParsingError = -1
};

class xml_parser {
 public:
    explicit xml_parser(std::istream& ins);
    xml_parser_token next_token();

 private:
    struct symb_table_t : public std::array<bool, 256> {
        struct add0_t {};
        explicit symb_table_t(std::string_view symbols) {
            fill(false);
            uint8_t prev_ch = '\0';
            for (auto it = symbols.begin(); it != symbols.end(); ++it) {
                if (*it != '-') {
                    prev_ch = static_cast<uint8_t>(*it);
                    (*this)[prev_ch] = true;
                } else if (++it != symbols.end()) {
                    for (uint8_t ch = prev_ch; ch <= static_cast<uint8_t>(*it); ++ch) { (*this)[ch] = true; }
                }
            }
        }
        explicit symb_table_t(std::string_view symbols, add0_t) : symb_table_t(symbols) { (*this)['\0'] = true; }
    };

    void error(unsigned line, std::string_view description);
    char skip_spaces();
    bool skip_up_to(std::string_view sub);
    bool try_parse_name(char first, std::string& name);
    bool parse_special_character(uint32_t& code);
    bool parse_string(std::string& str);

    std::istream& input_;
    unsigned token_line_ = 1;
    unsigned current_line_ = 1;
    bool is_empty_section_ = false;
    std::string text_;
    std::unordered_map<std::string, std::string> attributes_;
};

}  // namespace util
