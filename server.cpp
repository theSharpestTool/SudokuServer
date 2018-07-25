#include "server.h"

Server::Server(int nPort, QWidget* pwgt /*=0*/) : QWidget(pwgt)
                                                  , m_nNextBlockSize(0)
{
    short int a = 456;
    qDebug() << sizeof(a);
    m_ptcpServer = new QTcpServer(this);
    if (!m_ptcpServer->listen(QHostAddress::Any, nPort)) {
        QMessageBox::critical(0,
                              "Server Error",
                              "Unable to start the server:"
                              + m_ptcpServer->errorString()
                             );
        m_ptcpServer->close();
        return;
    }
    connect(m_ptcpServer, SIGNAL(newConnection()),
            this,         SLOT(slotNewConnection()));

    m_ptxt = new QTextEdit;
    m_ptxt->setReadOnly(true);
    lineEdit = new QLineEdit;

    connect(lineEdit, SIGNAL(returnPressed()), this, SLOT(executeSQL()));

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("D:\\databases\\Records.db");
    db.open();

    printRecords();

    QVBoxLayout* pvbxLayout = new QVBoxLayout;
    pvbxLayout->addWidget(new QLabel("<H1>Server</H1>"));
    pvbxLayout->addWidget(m_ptxt);
    pvbxLayout->addWidget(lineEdit);
    setLayout(pvbxLayout);
}

void Server::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();
    connect(pClientSocket, SIGNAL(disconnected()),
            pClientSocket, SLOT(deleteLater())
           );
    connect(pClientSocket, SIGNAL(readyRead()),
            this,          SLOT(slotReadClient())
           );
}

void Server::slotReadClient()
{
    QTcpSocket* pClientSocket = (QTcpSocket*)sender();
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_8);
    for (;;) {
        if (!m_nNextBlockSize) {
            if (pClientSocket->bytesAvailable() < sizeof(quint16)) {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNextBlockSize) {
            break;
        }

        QTime time;
        QString str;
        in >> time >> str;
        QString strMessage =
        time.toString() + " " + "Client has sent — " + str;
        m_ptxt->append(strMessage);
        m_nNextBlockSize = 0;

        /*in >> message;

        m_nNextBlockSize = 0;

        if(message == "0")
            selectRecords("easy");
        else if(message == "1")
            selectRecords("medium");
        else if(message == "2")
            selectRecords("hard");
        else
            addRecord();
        if(!message.isEmpty())
            sendToClient(pClientSocket, message);*/
    }
}

void Server::sendToClient(QTcpSocket* pSocket, const QString& str)
{
    QByteArray  arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_8);
    out << quint16(0) << str;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));

    pSocket->write(arrBlock);
}

void Server::printRecords()
{
    m_ptxt->append("\n----------------------------------");
    QSqlQuery query;
    query.exec("SELECT record_id, difficulty, time FROM Records "
               "ORDER BY difficulty, time");
    while(query.next())
    {
        QString id = query.value(0).toString();
        QString difficulty = query.value(1).toString();
        QString time = query.value(2).toString();
        m_ptxt->append(id+ "  " +difficulty+ "  " +time+ "\n");
    }
}

void Server::addRecord()
{
    QString difficulty, time;
    int i = 0;
    bool comma = false;
    while(!comma)
    {
        if(message[i] != ",")
            difficulty += message[i];
        else
            comma = true;
        i++;
    }
    for(int j = i; j < message.size(); j++)
        time += message[j];
    QSqlQuery query;
    query.prepare("INSERT INTO Records VALUES (null, :difficulty, :time);");
    query.bindValue(":difficulty", difficulty);
    query.bindValue(":time", time);
    query.exec();
    printRecords();
    message.clear();
}

void Server::executeSQL()
{
    QSqlQuery query;
    query.exec(lineEdit->text());
    printRecords();
}

void Server::selectRecords(QString difficulty)
{
    message.clear();
    QSqlQuery query;
    query.prepare("SELECT time FROM Records WHERE difficulty = :dff ORDER BY time;");
    query.bindValue(":dff", difficulty);
    query.exec();
    int counter = 1;
    while(query.next())
    {
        QString time;
        time += QString::number(query.value(0).toInt()/36000);
        time += QString::number((query.value(0).toInt()/3600)%10);
        time += ":";
        time += QString::number(((query.value(0).toInt()/60)%60)/10);
        time += QString::number((query.value(0).toInt()/60)%10);
        time += ":";
        time += QString::number((query.value(0).toInt()%60)/10);
        time += QString::number(query.value(0).toInt()%10);
        message += QString::number(counter) +  ". " +
                       time + "\n";
        counter++;
    }
}
