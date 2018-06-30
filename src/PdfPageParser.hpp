#ifndef PDF_PAGE_PARSER_HPP
#define PDF_PAGE_PARSER_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include <stack>

class PdfPageParser {
public:
    PdfPageParser(PoDoFo::PdfPage& page) : page(page) {}
    virtual ~PdfPageParser() {}

    void parse();

private:

    PoDoFo::PdfPage& page;
    std::stack<PoDoFo::PdfVariant> params;
    bool in_text = false;

    enum class pdf_keyword_t : uint32_t;

    static constexpr uint32_t s2kw(char const * s) noexcept;
    static std::string kw2s(pdf_keyword_t keyword) noexcept;

    void process_keyword(char const * keyword);
    void process_text_keyword(pdf_keyword_t keyword);
    void process_other_keyword(pdf_keyword_t keyword);
};

#endif
