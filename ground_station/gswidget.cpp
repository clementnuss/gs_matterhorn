#include "gswidget.h"
#include "ui_gswidget.h"

GSWidget::GSWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::GSWidget)
{
    ui->setupUi(this);
}

GSWidget::~GSWidget()
{
    delete ui;
}
