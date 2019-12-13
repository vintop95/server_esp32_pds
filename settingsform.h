/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
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

/**
 * This class models the graphical form
 * for changing the settings (preferences)
 * LIFECYCLE OF SETTINGS:
 * At start call loadSettingsFromIni() and use it for the rest of
 * the program
 * If you want to change the settings, change it from the ui
 * and restart, because we do:
 *  loadValuesFromIniToWidget()
 *  saveValuesFromWidgetToIni()
 */
class SettingsForm : public QDialog
{
    Q_OBJECT

private:
    void addEspWidget(QString name, double x, double y, bool modified = true);
    void loadValuesFromIniToWidget();
    void saveValuesFromWidgetToIni();
    Ui::SettingsForm *ui;
    QString settingsPath;

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_btnDecreaseESP_clicked();
    void on_btnIncreaseESP_clicked();

public:
    explicit SettingsForm(QString path, QWidget *parent = nullptr);
    ~SettingsForm();
};

#endif // SETTINGSFORM_H
