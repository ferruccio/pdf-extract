#include "PdfPageParser.hpp"

#include <iostream>

#include <string>

#pragma clang diagnostic ignored "-Wunused-const-variable"

using namespace PoDoFo;

void PdfPageParser::parse() {
    in_text = false;
    PdfContentsTokenizer tok(&page);
    char const * token = nullptr;
    PdfVariant var;
    EPdfContentsType type;
    while (tok.ReadNext(type, token, var)) {
        switch (type) {
            case ePdfContentsType_Keyword:
                process_keyword(token);
                break;
            case ePdfContentsType_Variant:
                params.push(var);
                break;
            case ePdfContentsType_ImageData:
                break;
            default:
                break;
        }
    }
}

namespace {
    constexpr int byte_size = 8;
    constexpr int byte_mask = 0xff;

    std::string kw2s(keyword_t keyword) noexcept {
        std::string s;
        for (unsigned i = 0; i < sizeof(keyword_t); ++i) {
            constexpr int shr = byte_size * (sizeof(keyword_t) - 1);
            char ch = static_cast<char>((keyword >> shr) & byte_mask);
            if (ch != 0)
                s += ch;
            keyword <<= byte_size;
        }
        return s;
    }

    constexpr keyword_t s2kw(char const * s) noexcept {
        keyword_t kw = 0;
        for (unsigned i = 0; *s != 0 && i < sizeof(keyword_t); ++i, ++s)
            kw = (kw << byte_size) | *s;
        return kw;
    }

    namespace keywords {
        constexpr keyword_t
            BT = s2kw("BT"),    ET = s2kw("ET"),    Tc = s2kw("Tc"),
            Tw = s2kw("Tw"),    Ts = s2kw("Ts"),    Tz = s2kw("Tz"),
            Td = s2kw("Td"),    TL = s2kw("TL"),    TD = s2kw("TD"),
            Tstar = s2kw("T*"), Tm = s2kw("Tm"),    Tj = s2kw("Tj"),
            TJ = s2kw("TJ"),    Tf = s2kw("Tf"),    Tr = s2kw("Tr"),
            apos = s2kw("\'"),  quot = s2kw("\"");
    }
}

void PdfPageParser::process_keyword(char const * keyword) {
    keyword_t kw = s2kw(keyword);
    switch (kw) {
        case keywords::BT:
            in_text = true;
            break;
        case keywords::ET:
            in_text = false;
            break;
        default:
            if (in_text) {
                process_text_keyword(kw);
            } else {
                process_other_keyword(kw);
            }
            break;
    }
}

void PdfPageParser::process_text_keyword(keyword_t keyword) {
    std::cout << '.';
}

void PdfPageParser::process_other_keyword(keyword_t keyword) {
    std::cout << kw2s(keyword) << ' ';
}
