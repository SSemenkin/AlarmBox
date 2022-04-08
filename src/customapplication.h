#ifndef CUSTOMAPPLICATION_H
#define CUSTOMAPPLICATION_H

#include <QApplication>
#include <QObject>

class CustomApplication : public QApplication
{
    Q_OBJECT
public:
    CustomApplication(int argc,
                      char **argv);

private:
    void applyFontToAllWidgets(const QFont& font);
    void applyPaletteToAllWidgets(const QPalette &palette);
};

#endif // CUSTOMAPPLICATION_H
