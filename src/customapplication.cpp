#include "customapplication.h"

#include <QWidget>

CustomApplication::CustomApplication(int argc, char **argv) :
    QApplication(argc, argv)
{
    connect(this, &QApplication::fontChanged, this, &CustomApplication::applyFontToAllWidgets);
    connect(this, &QApplication::paletteChanged, this, &CustomApplication::applyPaletteToAllWidgets);
}

void CustomApplication::applyFontToAllWidgets(const QFont &font) const
{
    auto widgets = allWidgets();
    for (int i = 0; i < widgets.size(); ++i) {
        widgets.at(i)->setFont(font);
    }
}

void CustomApplication::applyPaletteToAllWidgets(const QPalette &palette) const
{
    auto widgets = allWidgets();
    for (int i = 0; i < widgets.size(); ++i) {
        widgets.at(i)->setPalette(palette);
    }
}
