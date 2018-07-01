#include "PdfPageParser.hpp"

#include <iostream>
#include <string>

#pragma clang diagnostic ignored "-Wunused-const-variable"

using namespace PoDoFo;

void PdfPageParser::parse() {
    state.in_text = false;
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

// Compile-time conversion of a character string to pdf keyword type.
// This is a perfect hash as long as keywords are no longer than the number
// of bytes in an uint_32_t (nominally: 4 bytes).
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
        // begin/end text
        case pdf_keyword_t::BT: state.in_text = true; break;
        case pdf_keyword_t::ET: state.in_text = false; break;
        // text operators
        case pdf_keyword_t::Tc: state.character_spacing = pop_ts_units(); break;
        case pdf_keyword_t::Tw: state.word_spacing = pop_ts_units(); break;
        case pdf_keyword_t::Ts: state.text_rise = pop_ts_units(); break;
        case pdf_keyword_t::Tz: state.horizontal_scaling = pop_percentage(); break;
        case pdf_keyword_t::Td: break;
        case pdf_keyword_t::TL: state.leading = pop_ts_units(); break;
        case pdf_keyword_t::TD: break;
        case pdf_keyword_t::Tstar: break;
        case pdf_keyword_t::Tm: break;
        case pdf_keyword_t::Tj: process_string(); break;
        case pdf_keyword_t::TJ: process_strings(); break;
        case pdf_keyword_t::Tf: select_font(); break;
        case pdf_keyword_t::Tr: break;
        case pdf_keyword_t::apos: break;
        case pdf_keyword_t::quot: break;
        default: break;
    }
}

void PdfPageParser::process_string() {
    auto var = pop();
    switch (var.GetDataType()) {
        case ePdfDataType_String:
        case ePdfDataType_HexString: {
            auto str = var.GetString().GetStringUtf8();
            break;
        }
        default: break;
    }
}

void PdfPageParser::process_strings() {
    auto var = pop();
    if (var.IsArray()) {
        for (auto& item : var.GetArray()) {
            switch (item.GetDataType()) {
                case ePdfDataType_String:
                case ePdfDataType_HexString: {
                    auto str = item.GetString().GetStringUtf8();
                    break;
                }
                case ePdfDataType_Number: {
                    long n = item.GetNumber();
                    break;
                }
                default: break;
            }
        }
    }
}

void PdfPageParser::select_font() {
    state.font_scale = pop_n();
    std::string name = pop_name();

    auto iter = fonts.find(name);
    if (iter != fonts.end()) {
        state.current_font = &iter->second;
        return;
    }

    auto resources = page.GetResources();
    if (resources == nullptr) return;
    auto page_dict = resources->GetDictionary();
    auto fontkey = page_dict.GetKey("Font");
    if (fontkey == nullptr) return;
    auto font_dict = fontkey->GetDictionary();
    auto fontnamekey = font_dict.GetKey(name);
    if (fontnamekey == nullptr) return;
    auto fontref = pdf.GetObjects()->GetObject(fontnamekey->GetReference());
    if (fontref == nullptr) return;
    auto fontref_dict = fontref->GetDictionary();
    auto first_char_var = fontref_dict.GetKey("FirstChar");
    auto last_char_var = fontref_dict.GetKey("LastChar");
    auto widths_var = fontref_dict.GetKey("Widths");
    if (first_char_var == nullptr || last_char_var == nullptr || widths_var == nullptr) {
        state.current_font = nullptr;
    } else {
        FontInfo font_info;
        font_info.first_char = var2long(first_char_var);
        font_info.last_char = var2long(last_char_var);
        auto widths = widths_var->GetArray();
        for (const auto wt : widths) {
            auto width = text_space_milliunits::from_milliunits(var2n(wt));
            font_info.widths.push_back(width);
        }
        fonts[name] = font_info;
        auto iter = fonts.find(name);
        state.current_font = &iter->second;
    }
}