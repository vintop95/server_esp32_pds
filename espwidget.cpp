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
EspWidget::EspWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ESPWidget)
{
    ui->setupUi(this);
    setModified(false);
}

EspWidget::~EspWidget()
{
    delete ui;
}

/**
 * @brief Sets graphically the name of the ESP32 Device
 */
void EspWidget::setName(QString name)
{
    ui->lblESPName->setText(name);
}

/**
 * @brief Sets graphically the position of the ESP32 Device
 */
void EspWidget::setPoint(double x, double y)
{
    ui->txtX->setText(QString::number(x));
    ui->txtY->setText(QString::number(y));
}

/**
 * @brief Sets if the data written has been modified
 * Sets italic font it text is modified
 */
void EspWidget::setModified(bool m){
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
QString EspWidget::getName()
{
    return ui->lblESPName->text();
}

/**
 * @brief Get the x position of the ESP32 Device
 */
float EspWidget::getX()
{
    return ui->txtX->text().toFloat();
}

/**
 * @brief Get the y position of the ESP32 Device
 */
float EspWidget::getY()
{
    return ui->txtY->text().toFloat();
}

/**
 * @brief When x is modified, call this function
 */
void EspWidget::on_txtX_textEdited(const QString &dummy)
{
     setModified(true);
}

/**
 * @brief When y is modified, call this function
 */
void EspWidget::on_txtY_textEdited(const QString &dummy)
{
    setModified(true);
}
