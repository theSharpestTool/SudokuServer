#ifndef SERVER_H
#define SERVER_H

#include <QtWidgets>
#include <QTcpServer>
#include <QTcpSocket>
#include "QtSql/QSqlDatabase"
#include "QSqlQuery"
#include "QDebug"

class Server : public QWidget {
Q_OBJECT
private:
    QTcpServer* m_ptcpServer;
    QTextEdit*  m_ptxt;
    QLineEdit* lineEdit;
    quint16     m_nNextBlockSize;
    QString message;
    QSqlDatabase db;

private:
    void sendToClient(QTcpSocket* pSocket, const QString& str);
    void printRecords();
    void addRecord();
    void selectRecords(QString diff);

public:
    Server(int nPort, QWidget* pwgt = 0);

public slots:
    virtual void slotNewConnection();
            void slotReadClient   ();
            void executeSQL();
};

#endif // SERVER_H
