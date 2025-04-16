#ifndef CONTROLPANELFORM_H
#define CONTROLPANELFORM_H

#include "core/httpmanager.h"
#include "qpushbutton.h"
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
    void setControlPanelActive(const bool& active);
    QPushButton* getClearHistoryPbPtr();

public slots:
    void processWsData(const QJsonObject& data);
    void serverWorksChanged(QString devName, bool works);
    void serverAvailableChanged(QString devName, bool available);

private slots:
    void on_pb_start_clicked();
    void on_pb_stop_clicked();

private:
    Ui::ControlPanelForm *ui;
    SystemState currentState = Stopped;
    HttpManager *httpManager;
};

#endif // CONTROLPANELFORM_H
