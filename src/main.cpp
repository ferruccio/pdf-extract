#include <iostream>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

#include "PdfPageParser.hpp"

using std::cout;
using std::endl;
using std::string;
using std::ostream;

using namespace PoDoFo;

void show_page_info(PdfPage& page);

int main(int argc, char **argv) {
    PoDoFo::PdfError::EnableDebug(true);
    PoDoFo::PdfError::EnableLogging(true);
    for (int i = 1; i < argc; ++i) {
        try {
            PdfMemDocument pdf(argv[i]);
            int npages = pdf.GetPageCount();
            cout << "pages: " << npages << endl;
            for (int pageno = 0; pageno < npages; ++pageno) {
                auto& page = *pdf.GetPage(pageno);
                show_page_info(page);
                PdfPageParser ppp(pdf, page);
                ppp.parse();
            }
        } catch (PdfError e) {
            cout << "PDF Error: " << e.what() << endl;
        }
    }
}

ostream& operator<<(ostream& os, PdfRect const& rect) {
    os << "[(" << rect.GetLeft()
        << "," << rect.GetBottom()
        << ") w: " << rect.GetWidth()
        << " h: " << rect.GetHeight()
        << "]";
    return os;
}

void show_page_info(PdfPage& page) {
    cout << "\n  page: " << page.GetPageNumber();
    cout << "\n    size: " << page.GetPageSize();
    cout << "\n    media box: " << page.GetMediaBox();
    cout << "\n    crop box: " << page.GetCropBox();
    cout << "\n    trim box: " << page.GetTrimBox();
    cout << "\n    bleed box: " << page.GetBleedBox();
    cout << "\n    art box: " << page.GetArtBox();
    cout << "\n    rotation: " << page.GetRotation();
    cout << "\n    fields: " << page.GetNumFields();
    cout << "\n";
}