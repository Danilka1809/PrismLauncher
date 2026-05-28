#pragma once

#include <QMainWindow>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onPlayButtonClicked();
    void onSendMessageClicked();
    void onLoginClicked();
    void onRegisterClicked();
    void onLogoutClicked();
    void onSaveProfileClicked();

private:
    void setupUI();
    void loadMessages();
    void saveMessage(QString author, QString text);
    void saveUserData();

    // UI элементы
    QTextEdit *chatDisplay;
    QLineEdit *chatInput;
    QPushButton *sendButton;
    QPushButton *playButton;
    
    QLineEdit *nickInput;
    QLineEdit *passInput;
    QLineEdit *cloudPassInput;
    QPushButton *loginButton;
    QPushButton *registerButton;
    QPushButton *logoutButton;
    QPushButton *saveProfileButton;
    
    QWidget *userPanel;
    QLabel *usernameDisplay;
};
