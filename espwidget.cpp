/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "espwidget.h"
#include "ui_espwidget.h"

/**
 * @brief Constructor of ESPWidget
 */
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

/**
 * @brief Sets graphically the name of the ESP32 Device
 */
void ESPWidget::setName(QString name)
{
    ui->lblESPName->setText(name);
}

/**
 * @brief Sets graphically the position of the ESP32 Device
 */
void ESPWidget::setPoint(double x, double y)
{
    ui->txtX->setText(QString::number(x));
    ui->txtY->setText(QString::number(y));
}

/**
 * @brief Sets if the data written has been modified
 * Sets italic font it text is modified
 */
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

/**
 * @brief Get the name of the ESP32 Device
 */
QString ESPWidget::getName()
{
    return ui->lblESPName->text();
}

/**
 * @brief Get the x position of the ESP32 Device
 */
float ESPWidget::getX()
{
    return ui->txtX->text().toFloat();
}

/**
 * @brief Get the y position of the ESP32 Device
 */
float ESPWidget::getY()
{
    return ui->txtY->text().toFloat();
}

/**
 * @brief When x is modified, call this function
 */
void ESPWidget::on_txtX_textEdited(const QString &dummy)
{
     setModified(true);
}

/**
 * @brief When y is modified, call this function
 */
void ESPWidget::on_txtY_textEdited(const QString &dummy)
{
    setModified(true);
}
