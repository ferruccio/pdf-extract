#ifndef PDF_PAGE_PARSER_HPP
#define PDF_PAGE_PARSER_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include <stack>

struct TextState {
    double character_spacing = 0.0;
    double word_spacing = 0.0;
    double horizontal_scaling = 0.0;
    double leading = 0.0;
    double font_size = 0.0;
    double text_rise = 0.0;
};

class PdfPageParser {
public:
    PdfPageParser(PoDoFo::PdfPage& page) : page(page) {}
    virtual ~PdfPageParser() {}

    void parse();

private:
    PoDoFo::PdfPage& page;
    std::stack<PoDoFo::PdfVariant> operands;
    bool in_text = false;
    TextState state;

    enum class pdf_keyword_t : uint32_t;

    static constexpr uint32_t s2kw(char const * s) noexcept;
    static std::string kw2s(pdf_keyword_t keyword) noexcept;

    void process_keyword(char const * keyword);
    void process_text_keyword(pdf_keyword_t keyword);
    void process_other_keyword(pdf_keyword_t keyword);
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
};

#endif
