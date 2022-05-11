#ifndef PDF_PAGE_PARSER_HPP
#define PDF_PAGE_PARSER_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include "units.hpp"

struct FontInfo {
    long first_char = 0;
    long last_char = 0;
    std::vector<text_space_milliunits> widths;
};

struct TextState {
    bool in_text = false;
    percentage horizontal_scaling = percentage::from_double(100.0);
    text_space_units character_spacing;
    text_space_units word_spacing;
    text_space_units leading;
    text_space_units font_size;
    text_space_units text_rise;
    double font_scale = 1.0;
    FontInfo const* current_font = nullptr;
};

class PdfPageParser {
public:
    PdfPageParser(PoDoFo::PdfDocument& pdf, PoDoFo::PdfPage& page) : pdf(pdf), page(page) {}
    virtual ~PdfPageParser() {}

    static constexpr uint32_t s2kw(char const * s) noexcept;

    void parse();

private:
    PoDoFo::PdfDocument& pdf;
    PoDoFo::PdfPage& page;

    std::stack<PoDoFo::PdfVariant> operands;
    TextState state;
    std::unordered_map<std::string, FontInfo> fonts;

    enum class pdf_keyword_t : uint32_t;

    static std::string kw2s(pdf_keyword_t keyword) noexcept;

    void process_keyword(char const * keyword);
    void process_string();
    void process_strings();
    void select_font();

    static double var2n(PoDoFo::PdfVariant const& v) {
        if (v.IsReal()) return v.GetReal();
        if (v.IsNumber()) return v.GetNumber();
        return 0.0;
    }

    static long var2long(PoDoFo::PdfVariant const& v) {
        return v.IsNumber() ? v.GetNumber() : 0;
    }

    static long var2long(PoDoFo::PdfVariant const* v) {
        return v != nullptr && v->IsNumber() ? v->GetNumber() : 0;
    }

    PoDoFo::PdfVariant pop() noexcept {
        if (operands.size() == 0) {
            return PoDoFo::PdfVariant();
        } else {
            auto top = operands.top();
            operands.pop();
            return top;
        }
    }

    double pop_n() noexcept {
        return var2n(pop());
    }

    text_space_units pop_ts_units() noexcept {
        return text_space_units::from_units(pop_n());
    }

    percentage pop_percentage() noexcept {
        return percentage::from_double(pop_n());
    }

    std::string pop_name() noexcept {
        auto top = pop();
        return top.IsName() ? top.GetName().GetName() : std::string("");
    }
};

// useful for debugging

inline std::ostream& operator<<(std::ostream& os, TextState const& ts) {
    os << "TS[cs=" << ts.character_spacing
        << " ws=" << ts.word_spacing
        << " hs=" << ts.horizontal_scaling
        << " ld=" << ts.leading
        << " fs=" << ts.font_size
        << " tr=" << ts.text_rise
        << "]";
    return os;
}

inline std::ostream& operator<<(std::ostream& os, FontInfo const& fi) {
    os << "FI[ f=" << fi.first_char
        << " l=" << fi.last_char
        << " w=";
    for (const auto& w : fi.widths) {
        os << w << ' ';
    }
    os << "]";
    return os;
}

#define PODOFO_OUTPUT_OPERATOR(type) \
inline std::ostream& operator<<(std::ostream& os, PoDoFo::type const& t) { \
    std::string s; \
    t.ToString(s); \
    os << s; \
    return os; \
} \
inline std::ostream& operator<<(std::ostream& os, PoDoFo::type const* t) { \
    os << *t; \
    return os; \
} \

PODOFO_OUTPUT_OPERATOR(PdfObject)
PODOFO_OUTPUT_OPERATOR(PdfVariant)

// these don't have a ToString method so cast them to PdfVariants

#undef PODOFO_OUTPUT_OPERATOR
#define PODOFO_OUTPUT_OPERATOR(type) \
inline std::ostream& operator<<(std::ostream& os, PoDoFo::type const& t) { \
    os << PoDoFo::PdfVariant(t); \
    return os; \
} \
inline std::ostream& operator<<(std::ostream& os, PoDoFo::type const* t) { \
    os << PoDoFo::PdfVariant(*t); \
    return os; \
} \

PODOFO_OUTPUT_OPERATOR(PdfString)
PODOFO_OUTPUT_OPERATOR(PdfName)
PODOFO_OUTPUT_OPERATOR(PdfReference)
PODOFO_OUTPUT_OPERATOR(PdfArray)
PODOFO_OUTPUT_OPERATOR(PdfDictionary)
PODOFO_OUTPUT_OPERATOR(PdfData)

#undef PODOFO_OUTPUT_OPERATOR

#endif
