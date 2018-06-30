#include "PdfPageParser.hpp"

#include <iostream>

#include <string>

#pragma clang diagnostic ignored "-Wunused-const-variable"

using namespace PoDoFo;

std::ostream& operator<<(std::ostream& os, TextState const& ts) {
    os << "TS[cs:" << ts.character_spacing
        << " ws:" << ts.word_spacing
        << " hs:" << ts.horizontal_scaling
        << " ld:" << ts.leading
        << " fs:" << ts.font_size
        << " tr:" << ts.text_rise
        << "]";
    return os;
}


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
                operands.push(var);
                break;
            case ePdfContentsType_ImageData:
                break;
            default:
                break;
        }
    }
}

constexpr int byte_size = 8;
constexpr int byte_mask = 0xff;

std::string PdfPageParser::kw2s(pdf_keyword_t keyword) noexcept {
    std::string s;
    for (unsigned i = 0; i < sizeof(pdf_keyword_t); ++i) {
        constexpr int shr = byte_size * (sizeof(pdf_keyword_t) - 1);
        char ch = static_cast<char>((static_cast<uint32_t>(keyword) >> shr) & byte_mask);
        if (ch != 0)
            s += ch;
        keyword = static_cast<pdf_keyword_t>(static_cast<uint32_t>(keyword) << byte_size);
    }
    return s;
}

constexpr uint32_t PdfPageParser::s2kw(char const * s) noexcept {
    uint32_t kw = 0;
    for (unsigned nch = 0; *s != 0 && nch < sizeof(pdf_keyword_t); ++nch, ++s)
        kw = (kw << byte_size) | *s;
    return kw;
}

// these enumerations are computed at compile-time
enum class PdfPageParser::pdf_keyword_t : uint32_t {
    // text keywords
    BT = s2kw("BT"),    ET = s2kw("ET"),    Tc = s2kw("Tc"),
    Tw = s2kw("Tw"),    Ts = s2kw("Ts"),    Tz = s2kw("Tz"),
    Td = s2kw("Td"),    TL = s2kw("TL"),    TD = s2kw("TD"),
    Tstar = s2kw("T*"), Tm = s2kw("Tm"),    Tj = s2kw("Tj"),
    TJ = s2kw("TJ"),    Tf = s2kw("Tf"),    Tr = s2kw("Tr"),
    apos = s2kw("\'"),  quot = s2kw("\""),
};

void PdfPageParser::process_keyword(char const * keyword) {
    pdf_keyword_t kw = static_cast<pdf_keyword_t>(s2kw(keyword));
    switch (kw) {
        case pdf_keyword_t::BT:
            in_text = true;
            break;
        case pdf_keyword_t::ET:
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

void PdfPageParser::process_text_keyword(pdf_keyword_t keyword) {
    switch (keyword) {
        case pdf_keyword_t::Tc: state.character_spacing = pop_double(); break;
        case pdf_keyword_t::Tw: state.word_spacing = pop_double(); break;
        case pdf_keyword_t::Ts: state.text_rise = pop_double(); break;
        case pdf_keyword_t::Tz: state.horizontal_scaling = pop_double(); break;
        case pdf_keyword_t::Td: break;
        case pdf_keyword_t::TL: state.leading = pop_double(); break;
        case pdf_keyword_t::TD: break;
        case pdf_keyword_t::Tstar: break;
        case pdf_keyword_t::Tm: break;
        case pdf_keyword_t::Tj: break;
        case pdf_keyword_t::TJ: break;
        case pdf_keyword_t::Tf: break;
        case pdf_keyword_t::Tr: break;
        case pdf_keyword_t::apos: break;
        case pdf_keyword_t::quot: break;
        default: break;
    }
}

void PdfPageParser::process_other_keyword(pdf_keyword_t keyword) {
    switch (keyword) {
        default: break;
    }
}
