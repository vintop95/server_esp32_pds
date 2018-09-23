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
class ESPWidget;
}

/**
 * This is the Graphical Row containing the informations
 * about a ESP32 device
 */
class ESPWidget : public QWidget
{
    Q_OBJECT
private:
    bool modified = false;
public:
    explicit ESPWidget(QWidget *parent = nullptr);
    ~ESPWidget();

    void setName(QString name);
    void setPoint(float x, float y);
    void setModified(bool m);

    QString getName();
    float getX();
    float getY();

private slots:
    void on_txtX_textEdited(const QString &arg1);
    void on_txtY_textEdited(const QString &arg1);

private:
    Ui::ESPWidget *ui;
};

#endif // ESPWIDGET_H
