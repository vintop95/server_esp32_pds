/**
 * PDS Project - Server ESP32
 * Gianluca D'Alleo
 * Salvatore Di Cara
 * Giorgio Pizzuto
 * Vincenzo Topazio
 */
#ifndef ESPWIDGET_H
#define ESPWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QListWidgetItem>
#include <QString>

namespace Ui {
class EspWidget;
}

/**
 * This is the Graphical Row containing the informations
 * about a ESP32 device
 */
class EspWidget : public QWidget
{
    Q_OBJECT

private:
    Ui::EspWidget *ui;
    bool modified = false;

private slots:
    void on_txtX_textEdited(const QString &arg1);
    void on_txtY_textEdited(const QString &arg1);

public:
    explicit EspWidget(QWidget *parent = nullptr);
    ~EspWidget();

    void setName(QString name);
    void setPoint(double x, double y);
    void setModified(bool m);

    QString getName();
    float getX();
    float getY();
};

#endif // ESPWIDGET_H
