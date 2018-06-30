#include <iostream>
#include <string>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#include <podofo/podofo.h>
#pragma clang diagnostic pop

using std::cout;
using std::endl;
using std::string;

using namespace PoDoFo;


int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        cout << argv[i] << endl;

        try {
            PdfMemDocument pdf(argv[i]);
            int npages = pdf.GetPageCount();
            cout << "  pages: " << npages << endl;
            for (int pageno = 0; pageno < npages; ++pageno) {
                auto& page = *pdf.GetPage(pageno);
                auto size = page.GetPageSize();
                cout << "    page: " << pageno + 1;
                cout << " w: " << size.GetWidth() << " h: " << size.GetHeight() << endl;
            }
        } catch (PdfError e) {
            cout << "PDF Error: " << e.what() << endl;
        }
    }
}