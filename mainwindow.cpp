#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "desencryptor.cpp"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::checkInput(std::string input, QString& err)
{
    if(input.size() != 8)
    {
        err = "Wrong input size";
        return false;
    }
    for(unsigned int i = 0; i < input.size(); i++)
    {
        QChar ch(input[i]);

        if(ch.unicode() >= 128)
        {
            err = "Non-ascii character";
            return false;
        }
    }
    return true;
}

void MainWindow::showErrorMessage(QString errorMsg)
{
    QMessageBox errorBox;
    errorBox.critical(this, "Error", errorMsg);
}

void MainWindow::on_pushButton_encrypt_released()
{
    QString input = ui->lineEdit_input->text();
    QString key = ui->lineEdit_key->text();

    QString error;
    if(!checkInput(input.toStdString(),error) || !checkInput(key.toStdString(),error))
    {
        showErrorMessage(error);
        return;
    }

    DESencryptor enc;
    QString encryptMessage = enc.encrypt(input, key);

    ui->lineEdit_output->setText(encryptMessage);

    return;
}

void MainWindow::on_pushButton_copyToInput_released()
{
    if(ui->lineEdit_output->text() == "")
        return;

    ui->lineEdit_input->setText(ui->lineEdit_output->text());
}
