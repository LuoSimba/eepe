#include "downloaddialog.h"
#include "ui_downloaddialog.h"
#include <QMessageBox>
#include <QNetworkAccessManager>
#include <QNetworkProxy>

downloadDialog::downloadDialog(QWidget *parent, QString src, QString tgt) :
    QDialog(parent),
    ui(new Ui::downloadDialog)
{
    ui->setupUi(this);

    file = new QFile(tgt);
    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "eePe",
                              tr("Unable to save the file %1: %2.")
                              .arg(tgt).arg(file->errorString()));
        delete file;
        file = 0;
        this->close();
    }

    reply = qnam.get(QNetworkRequest(QUrl(src)));
    connect(reply, SIGNAL(finished()),
            this, SLOT(httpFinished()));
    connect(reply, SIGNAL(readyRead()),
            this, SLOT(httpReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)),
            this, SLOT(updateDataReadProgress(qint64,qint64)));
}

downloadDialog::~downloadDialog()
{
    delete ui;
}

void downloadDialog::httpFinished()
{
    file->flush();
    file->close();

    if (reply->error())
    {
        file->remove();
        QMessageBox::information(this, tr("eePe"),
                                 tr("Download failed: %1.")
                                 .arg(reply->errorString()));
    }

    reply->deleteLater();
    reply = 0;
    delete file;
    file = 0;
    this->accept();
}

void downloadDialog::httpReadyRead()
{
    if (file)
        file->write(reply->readAll());
}

void downloadDialog::updateDataReadProgress(qint64 bytesRead, qint64 totalBytes)
{
    ui->progressBar->setMaximum(totalBytes);
    ui->progressBar->setValue(bytesRead);
}
