#include "espwidget.h"
#include "ui_espwidget.h"

ESPWidget::ESPWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ESPWidget)
{
    ui->setupUi(this);
    setModified(false);
}

ESPWidget::~ESPWidget()
{
    delete ui;
}

void ESPWidget::setName(QString name)
{
    ui->lblESPName->setText(name);
}

void ESPWidget::setPoint(float x, float y)
{
    ui->txtX->setText(QString::number(x));
    ui->txtY->setText(QString::number(y));
}

void ESPWidget::setModified(bool m){
    modified = m;
    if(modified){
        ui->txtX->setStyleSheet("font: italic Arial;");
        ui->txtY->setStyleSheet("font: italic Arial;");
    }else{
        ui->txtX->setStyleSheet("font: Arial;");
        ui->txtY->setStyleSheet("font: Arial;");
    }
}

QString ESPWidget::getName()
{
    return ui->lblESPName->text();
}

float ESPWidget::getX()
{
    return ui->txtX->text().toFloat();
}

float ESPWidget::getY()
{
    return ui->txtY->text().toFloat();
}


void ESPWidget::on_txtX_textEdited(const QString &arg1)
{
     setModified(true);
}

void ESPWidget::on_txtY_textEdited(const QString &arg1)
{
    setModified(true);
}
