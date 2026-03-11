#ifndef SETTINGS_VIEW_H
#define SETTINGS_VIEW_H

#include <Wt/WContainerWidget.h>
#include <Wt/WLineEdit.h>

class SettingsView : public Wt::WContainerWidget {
public:
    SettingsView();

private:
    void buildUI();
    void saveSettings();

    Wt::WLineEdit* apiEndpointInput_;
    Wt::WLineEdit* dateFormatInput_;
    Wt::WLineEdit* currencyInput_;
};

#endif // SETTINGS_VIEW_H
