#ifndef GSWIDGET_H
#define GSWIDGET_H

#include <QWidget>

namespace Ui {
class GSWidget;
}

class GSWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GSWidget(QWidget *parent = nullptr);
    ~GSWidget() override;

private:
    Ui::GSWidget *ui;
};

#endif // GSWIDGET_H
