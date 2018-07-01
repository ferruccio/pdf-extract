#ifndef PDF_PAGE_PARSER_HPP
#define PDF_PAGE_PARSER_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include <stack>
#include "units.hpp"

struct TextState {
    bool in_text = false;
    percentage horizontal_scaling = percentage::from_double(100.0);
    text_space_units character_spacing;
    text_space_units word_spacing;
    text_space_units leading;
    text_space_units font_size;
    text_space_units text_rise;
};

class PdfPageParser {
public:
    PdfPageParser(PoDoFo::PdfPage& page) : page(page) {}
    virtual ~PdfPageParser() {}

    void parse();

private:
    PoDoFo::PdfPage& page;
    std::stack<PoDoFo::PdfVariant> operands;
    TextState state;

    enum class pdf_keyword_t : uint32_t;

    static constexpr uint32_t s2kw(char const * s) noexcept;
    static std::string kw2s(pdf_keyword_t keyword) noexcept;

    void process_keyword(char const * keyword);
    void process_string();
    void process_strings();

    PoDoFo::PdfVariant pop() noexcept {
        if (operands.size() == 0) {
            return PoDoFo::PdfVariant();
        } else {
            auto top = operands.top();
            operands.pop();
            return top;
        }
    }

    double pop_real() noexcept {
        auto top = pop();
        return top.IsReal() ? top.GetReal() : 0.0;
    }

    text_space_units pop_ts_units() noexcept {
        return text_space_units::from_units(pop_real());
    }

    percentage pop_percentage() noexcept {
        return percentage::from_double(pop_real());
    }
};

#endif
