#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_encrypt_released();

    void on_pushButton_copyToInput_released();

private:
    Ui::MainWindow *ui;
    bool checkInput(std::string inp, QString& err);
    void showErrorMessage(QString err);
};

#endif // MAINWINDOW_H
