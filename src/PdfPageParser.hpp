#ifndef PDF_PAGE_PARSER_HPP
#define PDF_PAGE_PARSER_HPP

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include <stack>

using keyword_t = uint32_t;

class PdfPageParser {
public:
    PdfPageParser(PoDoFo::PdfPage& page) : page(page) {}
    virtual ~PdfPageParser() {}

    void parse();

private:

    PoDoFo::PdfPage& page;
    std::stack<PoDoFo::PdfVariant> params;
    bool in_text = false;

    void process_keyword(char const * keyword);
    void process_text_keyword(keyword_t keyword);
    void process_other_keyword(keyword_t keyword);
};

#endif
