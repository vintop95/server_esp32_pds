#ifndef SETTINGSFORM_H
#define SETTINGSFORM_H

#include <QDialog>
#include <QAbstractButton>
#include <QMessageBox>
#include <QDebug>
#include "settings.h"
#include "espwidget.h"

namespace Ui {
class SettingsForm;
}

class SettingsForm : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsForm(QWidget *parent = nullptr);
    ~SettingsForm();


private slots:
    void on_buttonBox_clicked(QAbstractButton *button);

    void on_btnDecreaseESP_clicked();

    void on_btnIncreaseESP_clicked();

private:
    void addESPWidget(QString name, float x, float y, bool modified = true);
    void loadValues();
    void saveValues();
    Ui::SettingsForm *ui;
};

#endif // SETTINGSFORM_H
