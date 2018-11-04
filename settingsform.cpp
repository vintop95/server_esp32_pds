/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#include "settingsform.h"
#include "ui_settingsform.h"

/**
 * @brief Constructor of SettingsForm
 * It loads the values from the .ini file
 * and sets the path to the .ini file
 */
SettingsForm::SettingsForm(QString path, QWidget *parent) :
    QDialog(parent), ui(new Ui::SettingsForm), settingsPath(path)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
    ui->setupUi(this);
    loadValues();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}


/**
 * @brief Load the values from the .ini file to the form
 */
void SettingsForm::loadValues()
{
    QString settingsFile = settingsPath;
    QSettings qs(settingsFile, QSettings::IniFormat);

    // Load CHART_PERIOD from the .ini file, if not found set value 1000
    int chartPeriod = qs.value("CHART_PERIOD", 1000).toInt();
    int ESP32No = qs.value("ESP32_NO", 0).toInt();

    ui->txtChartPeriod->setText(QString::number(chartPeriod));
    ui->txtESP32No->setText(QString::number(ESP32No));

    QString name;
    for(int i=0; i < ESP32No; ++i){
        name = qs.value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
        float x = qs.value(QString(name + "/pos_x"), -1).toFloat();
        float y = qs.value(QString(name + "/pos_y"), -1).toFloat();

        addESPWidget(name, x, y, false);
    }
    qs.sync();
}

/**
 * @brief Save the values in the form to the .ini file
 */
void SettingsForm::saveValues()
{
    QString settingsFile = settingsPath;
    QSettings qs(settingsFile, QSettings::IniFormat);

    qs.setValue("CHART_PERIOD", ui->txtChartPeriod->text().toInt());
    qs.setValue("ESP32_NO", ui->txtESP32No->text().toInt());

    int ESP32No = ui->txtESP32No->text().toInt();

    for(int i=0; i < ESP32No; ++i){
        QString name = QString("ESP%1").arg(i);
        float x = ((ESPWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i)))->getX();
        float y = ((ESPWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i)))->getY();

        qs.setValue(QString(name + "/name"), name);
        qs.setValue(QString(name + "/pos_x"), QString::number(x,'f'));
        qs.setValue(QString(name + "/pos_y"), QString::number(y,'f'));
    }
    qs.sync();
}

/**
 * @brief Add into the list of ESP Devices an ESP Device with name and position
 *
 * @param Name of ESP32 Device
 * @param x position
 * @param y position
 * @param if true, the row will be set modified (and the text becomes italic)
 */
void SettingsForm::addESPWidget(QString name, float x, float y, bool modified)
{
    //Creating an object of the designed widget which is to be added to the listwidget
    ESPWidget *espWidget = new ESPWidget;
    espWidget->setName(name);
    espWidget->setPoint(x,y);
    espWidget->setModified(modified);

    QListWidgetItem *listWidgetItem = new QListWidgetItem(ui->listWidget);

    //Making sure that the listWidgetItem has the same size as the TheWidgetItem
    listWidgetItem->setSizeHint(espWidget->sizeHint());

    //Adding the item to the listwidget
    ui->listWidget->addItem(listWidgetItem);

    //Finally adding the itemWidget to the list
    ui->listWidget->setItemWidget(listWidgetItem, espWidget);
}


/**
 * @brief Behavior of the buttons Save, Cancel and RestoreDefaults
 */
void SettingsForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults) ){
        // Not added yet
    }else if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Save) ){
        saveValues();
        QMessageBox::warning(this,"Apply changes","Restart the application to apply changes.");
    }else if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Cancel) ){
        this->close();
    }
}

/**
 * @brief Decrease the number of ESP32 devices
 */
void SettingsForm::on_btnDecreaseESP_clicked()
{
    int n = ui->txtESP32No->text().toInt();
    n--;

    // If we did not reach the limit, delete a row from the ESP32 devices list
    // and decrease the number of ESP32 devices
    if(n >= 0){
        ui->txtESP32No->setText(QString::number(n));

        //Delete selected item from the listWidget
        delete ui->listWidget->item(ui->listWidget->count() - 1);
    }
}

/**
 * @brief Increase the number of ESP32 devices
 */
void SettingsForm::on_btnIncreaseESP_clicked()
{
    int n = ui->txtESP32No->text().toInt();

    // If we did not reach the limit, add a row in the ESP32 devices list
    // and increase the number of ESP32 devices
    if(n < ESP32_NO_LIMIT){
        addESPWidget("ESP" + QString::number(n), 0, 0);
        n++;
        ui->txtESP32No->setText(QString::number(n));
    }
}
