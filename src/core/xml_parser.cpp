#include "core/xml_parser.h"

using namespace util;

xml_parser::xml_parser(std::istream& ins) : input_(ins) {}

xml_parser_token xml_parser::next_token() {
    token_line_ = current_line_;

    if (is_empty_section_) {
        // end of empty section
        is_empty_section_ = false;
        return xml_parser_token::kEndOfSection;
    }

    text_.clear();

    while (true) {
        auto ch = input_.get();
        if (!input_.good()) {
            if (text_.empty()) { return xml_parser_token::kEof; }
            break;
        }

        switch (ch) {
            case '&':  // special character
            {
                uint32_t code = 0;
                if (!parse_special_character(code)) {
                    error(current_line_, "invalid special character");
                    return xml_parser_token::kParsingError;
                }
                to_utf8(code, std::back_inserter(text_));
                continue;
            }

            case '\n':
                ++current_line_;
                text_.push_back(ch);
                continue;

            case '<':  // tag opening
            {
                if (!text_.empty()) {
                    // return plain text first
                    input_.unget();
                    break;
                }

                auto type = xml_parser_token::kSection;

                ch = input_.get();
                if (!ch) {
                    error(current_line_, "unexpected end of file");
                    return xml_parser_token::kParsingError;
                } else if (try_parse_name(ch, text_)) {  // do nothing
                } else if ((ch == '/') || (ch == '?') || (ch == '!')) {
                    auto next = input_.get();
                    if (try_parse_name(next, text_)) {
                        if (ch == '/') {
                            if (input_.get() != '>') {
                                error(current_line_, "expected '/>' here");
                                return xml_parser_token::kParsingError;
                            }
                            // end of not empty section
                            return xml_parser_token::kEndOfSection;
                        } else if (ch == '?') {
                            type = xml_parser_token::kDeclaration;
                        } else {
                            if (text_ == "DOCTYPE") {
                                text_.clear();
                                if (!try_parse_name(ch = skip_spaces(), text_)) {
                                    error(current_line_, "expected DOCTYPE name here");
                                    return xml_parser_token::kParsingError;
                                }
                            }

                            if (!skip_up_to(">")) { return xml_parser_token::kParsingError; }
                            attributes_.clear();
                            return xml_parser_token::kDoctype;
                        }
                    } else if ((ch == '!') && (next == '-')) {
                        if (input_.get() != '-') {
                            error(current_line_, "expected '<!--' here");
                            return xml_parser_token::kParsingError;
                        }
                        if (!skip_up_to("-->")) { return xml_parser_token::kParsingError; }
                        continue;  // read next token
                    } else {
                        error(current_line_, "expected tag name or '<!--' here");
                        return xml_parser_token::kParsingError;
                    }
                } else {
                    error(current_line_, "expected '</', '<?', '<!' or tag name here");
                    return xml_parser_token::kParsingError;
                }

                attributes_.clear();

                // read attributes
                while (true) {
                    std::string name;
                    if (try_parse_name(ch = skip_spaces(), name)) {
                        // attribute found
                        if ((ch = skip_spaces()) != '=') {
                            error(current_line_, "expected '=' here");
                            return xml_parser_token::kParsingError;
                        }
                        if ((ch = skip_spaces()) != '\"') {
                            error(current_line_, "expected string here");
                            return xml_parser_token::kParsingError;
                        }
                        std::string value;
                        if (!parse_string(value)) { return xml_parser_token::kParsingError; }
                        attributes_.emplace(std::move(name), std::move(value));
                    } else if (type == xml_parser_token::kDeclaration) {
                        // end of declaration
                        if ((ch != '?') || (input_.get() != '>')) {
                            error(current_line_, "expected '?>' here");
                            return xml_parser_token::kParsingError;
                        }
                        break;
                    } else if (ch == '/') {
                        if (input_.get() != '>') {
                            error(current_line_, "expected '/>' here");
                            return xml_parser_token::kParsingError;
                        }
                        is_empty_section_ = true;
                        break;
                    } else if (ch != '>') {
                        error(current_line_, "expected '>', '/>' or name here");
                        return xml_parser_token::kParsingError;
                    } else {
                        break;
                    }
                }

                return type;  // section header or declaration
            }

            case '>':  // tag clocing
                error(current_line_, "not expected '>' here");
                return xml_parser_token::kParsingError;

            default: text_.push_back(ch); continue;
        }

        break;
    }

    // plain text token
    return xml_parser_token::kPlainText;
}

void xml_parser::error(unsigned line, std::string_view description) {
    std::cout << description << " (" << line << ')' << std::endl;
}

char xml_parser::skip_spaces() {
    while (true) {
        static symb_table_t _is_blank{" \t\r"};
        auto ch = input_.get();
        while (_is_blank[ch]) { ch = input_.get(); }
        if (!input_.good()) {  // eof found
            return '\0';
        } else if (ch != '\n') {
            return ch;
        }
        // significant character found
        ++current_line_;
    }
}

bool xml_parser::skip_up_to(std::string_view sub) {
    auto it = sub.begin();
    while (it != sub.end()) {
        auto ch = input_.get();
        if (!input_.good()) {
            error(current_line_, "unexpected end of file");
            return false;
        } else if (ch == '\n') {
            ++current_line_;
        } else if (ch == *it) {
            ++it;
        }
    }
    return true;
}

bool xml_parser::try_parse_name(char first, std::string& name) {
    static symb_table_t _is_name_first_char{":_a-zA-Z"};
    if (!_is_name_first_char[static_cast<uint8_t>(first)]) { return false; }
    name.push_back(first);
    while (true) {
        static symb_table_t _is_name_char{"---.:_0-9a-zA-Z"};
        auto ch = input_.get();
        if (!input_.good()) {  // eof found
            break;
        } else if (!_is_name_char[static_cast<uint8_t>(ch)]) {  // end of name found
            input_.unget();
            break;
        }
        name.push_back(ch);
    }
    return true;
}

bool xml_parser::parse_special_character(uint32_t& code) {
    auto ch = input_.get();
    if (!input_.good()) {
        return false;
    } else if (ch == '#') {
        int count = 0;
        code = 0;

        ch = input_.get();
        if ((ch == 'x') || (ch == 'X')) {
            // hex code
            while (true) {
                code <<= 4;
                ch = input_.get();
                if ((ch >= '0') && (ch <= '9')) {
                    code |= ch - '0';
                } else if ((ch >= 'A') && (ch <= 'F')) {
                    code |= ch - 'A' + 10;
                } else if ((ch >= 'a') && (ch <= 'f')) {
                    code |= ch - 'a' + 10;
                } else if (ch == ';') {
                    break;
                } else {
                    return false;
                }
                ++count;
            }
        }

        // decimal code
        while (true) {
            code *= 10;
            ch = input_.get();
            if ((ch >= '0') && (ch <= '9')) {
                code += ch - '0';
            } else if (ch == ';') {
                break;
            } else {
                return false;
            }
            ++count;
        }

        return count != 0;
    }

    std::array<char, 5> name;
    for (size_t n = 0; n < name.size(); ch = input_.get(), ++n) {
        if ((ch >= 'a') && (ch <= 'z')) {
            name[n] = ch;
        } else if (ch == ';') {
            if (std::string_view(name.data(), n) == "lt") {
                code = '<';
                return true;
            } else if (std::string_view(name.data(), n) == "gt") {
                code = '>';
                return true;
            } else if (std::string_view(name.data(), n) == "amp") {
                code = '&';
                return true;
            } else if (std::string_view(name.data(), n) == "apos") {
                code = '\'';
                return true;
            } else if (std::string_view(name.data(), n) == "quot") {
                code = '\"';
                return true;
            }
            break;
        } else {
            break;
        }
    }

    return false;
}

bool xml_parser::parse_string(std::string& str) {
    auto ch = '\0';
    while (true) {
        ch = input_.get();
        if (!input_.good()) { break; }  // eof found
        switch (ch) {
            case '&':  // special character
            {
                uint32_t code = 0;
                if (!parse_special_character(code)) {
                    error(current_line_, "invalid special character");
                    return false;
                }
                to_utf8(code, std::back_inserter(str));
                continue;
            }

            case '\n':
            case '\0': break;        // enexpected end of string
            case '\"': return true;  // end of string

            default: {
                str.push_back(ch);
                continue;
            }
        }

        break;
    }

    error(current_line_, "unexpected end of string literal");
    return false;
}
