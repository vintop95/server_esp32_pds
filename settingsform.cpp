#include "settingsform.h"
#include "ui_settingsform.h"

SettingsForm::SettingsForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsForm)
{
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);

    ui->setupUi(this);
    loadValues();
}

SettingsForm::~SettingsForm()
{
    delete ui;
}


// TODO: parametrizzare settingsFile in Settings class
void SettingsForm::loadValues()
{
    QString settingsFile =
                //QApplication::applicationDirPath() + "/" +
                "settings.ini";
    QSettings qs(settingsFile, QSettings::IniFormat);

    int chartPeriod = qs.value("CHART_PERIOD", 1000).toInt();
    int ESP32No = qs.value("ESP32_NO", -1).toInt();

    ui->txtChartPeriod->setText(QString::number(chartPeriod));
    ui->txtESP32No->setText(QString::number(ESP32No));

    QString name;
    for(int i=0; i < ESP32No; ++i){

        name = qs.value(QString("ESP%1/name").arg(i), QString("not_found_%1").arg(i)).toString();
        float x = qs.value(QString(name + "/pos_x"), -1).toFloat();
        float y = qs.value(QString(name + "/pos_y"), -1).toFloat();

        addESPWidget(name, x, y, false);
    }

}

void SettingsForm::saveValues()
{
    QString settingsFile =
                //QApplication::applicationDirPath() + "/" +
                "settings.ini";
    QSettings qs(settingsFile, QSettings::IniFormat);

    qs.setValue("CHART_PERIOD", ui->txtChartPeriod->text().toInt());
    qs.setValue("ESP32_NO", ui->txtESP32No->text().toInt());

    int ESP32No = ui->txtESP32No->text().toInt();

    for(int i=0; i < ESP32No; ++i){
        QString name = QString("ESP%1").arg(i);
        float x = ((ESPWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i)))->getX();
        float y = ((ESPWidget*)ui->listWidget->itemWidget(ui->listWidget->item(i)))->getY();

        qs.setValue(QString(name + "/name"), name);
        qs.setValue(QString(name + "/pos_x"), x);
        qs.setValue(QString(name + "/pos_y"), y);

    }

}

void SettingsForm::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::RestoreDefaults) ){

    }else if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Save) ){
        saveValues();
        QMessageBox::warning(this,"Apply changes","Restart the application to apply changes.");
    }else if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Cancel) ){
        this->close();
    }
}

void SettingsForm::on_btnDecreaseESP_clicked()
{
    //decrease the number of esps
    int n = ui->txtESP32No->text().toInt();
    n--;
    if(n >= 0){
        ui->txtESP32No->setText(QString::number(n));

        //Delete selected item from the listWidget
        delete ui->listWidget->item(ui->listWidget->count() - 1);
    }
}

void SettingsForm::on_btnIncreaseESP_clicked()
{
    //increase the number of esps
    int n = ui->txtESP32No->text().toInt();

    if(n < ESP32_NO_LIMIT){
        addESPWidget("ESP" + QString::number(n), 0, 0);

        n++;
        ui->txtESP32No->setText(QString::number(n));
    }
}

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
