#include "Footer.h"
#include <Wt/WText.h>

Footer::Footer() {
    setStyleClass("footer");
    addWidget(std::make_unique<Wt::WText>("Imagery Business Systems, LLC"));
}
