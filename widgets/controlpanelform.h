#ifndef CONTROLPANELFORM_H
#define CONTROLPANELFORM_H

#include <QWidget>

namespace Ui {
class ControlPanelForm;
}

class ControlPanelForm : public QWidget
{
    Q_OBJECT

public:
    explicit ControlPanelForm(QWidget *parent = nullptr);
    ~ControlPanelForm();

    enum SystemState{
        Working,
        Stopped
    };

    void setProductsLeftCount(const int& leftCount);
    void setSystemState(const SystemState& state);
    void setCurrentProduct(const QString& product);

public slots:
    void processWsData(const QJsonObject& data);

private slots:
    void on_pb_start_stop_clicked();

private:
    Ui::ControlPanelForm *ui;
    SystemState currentState = Stopped;
};

#endif // CONTROLPANELFORM_H
