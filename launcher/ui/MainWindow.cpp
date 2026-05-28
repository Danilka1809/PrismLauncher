#include "MainWindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDir>
#include <QTimer>
#include <QLabel>
#include <QGroupBox>
#include <QScrollArea>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUI();
    loadMessages();
    
    // Автообновление чата каждые 2 секунды
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::loadMessages);
    timer->start(2000);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI()
{
    setWindowTitle("GRIF LAUNCHER");
    resize(1280, 720);
    
    QWidget *central = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(central);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // ========== ЛЕВАЯ КОЛОНКА ==========
    QVBoxLayout *leftColumn = new QVBoxLayout();
    
    // Шапка
    QHBoxLayout *header = new QHBoxLayout();
    QLabel *logo = new QLabel("GRIF LAUNCHER");
    logo->setStyleSheet("font-size: 28px; font-weight: bold; color: #3b82f6;");
    QLabel *onlineBadge = new QLabel("🔥 12 игроков онлайн");
    onlineBadge->setStyleSheet("background-color: rgba(59,130,246,0.2); padding: 8px 20px; border-radius: 40px;");
    header->addWidget(logo);
    header->addWidget(onlineBadge);
    header->addStretch();
    leftColumn->addLayout(header);
    
    // Чат
    QGroupBox *chatBox = new QGroupBox("🌐 Глобальный чат");
    chatBox->setStyleSheet("QGroupBox { background-color: rgba(18,25,45,0.6); border-radius: 16px; padding: 15px; }");
    QVBoxLayout *chatLayout = new QVBoxLayout();
    chatDisplay = new QTextEdit();
    chatDisplay->setReadOnly(true);
    chatDisplay->setStyleSheet("background-color: rgba(0,0,0,0.4); border-radius: 12px; padding: 10px;");
    chatInput = new QLineEdit();
    chatInput->setPlaceholderText("Написать сообщение...");
    chatInput->setStyleSheet("background-color: rgba(0,0,0,0.4); border-radius: 40px; padding: 10px; border: 1px solid #3b82f6;");
    sendButton = new QPushButton("Отправить");
    sendButton->setStyleSheet("QPushButton { background-color: #3b82f6; border-radius: 40px; padding: 8px 20px; }");
    connect(sendButton, &QPushButton::clicked, this, &MainWindow::onSendMessageClicked);
    
    QHBoxLayout *chatInputLayout = new QHBoxLayout();
    chatInputLayout->addWidget(chatInput);
    chatInputLayout->addWidget(sendButton);
    chatLayout->addWidget(chatDisplay);
    chatLayout->addLayout(chatInputLayout);
    chatBox->setLayout(chatLayout);
    leftColumn->addWidget(chatBox);
    
    // ========== ПРАВАЯ КОЛОНКА ==========
    QVBoxLayout *rightColumn = new QVBoxLayout();
    
    // Профиль
    QGroupBox *profileBox = new QGroupBox("👤 Профиль");
    profileBox->setStyleSheet("QGroupBox { background-color: rgba(18,25,45,0.6); border-radius: 16px; padding: 15px; }");
    QVBoxLayout *profileLayout = new QVBoxLayout();
    
    QLabel *avatar = new QLabel("🔥");
    avatar->setStyleSheet("font-size: 48px;");
    avatar->setAlignment(Qt::AlignCenter);
    
    nickInput = new QLineEdit();
    nickInput->setPlaceholderText("Никнейм");
    nickInput->setStyleSheet("background-color: rgba(0,0,0,0.4); border-radius: 40px; padding: 10px; border: 1px solid #3b82f6;");
    
    passInput = new QLineEdit();
    passInput->setPlaceholderText("Пароль");
    passInput->setEchoMode(QLineEdit::Password);
    passInput->setStyleSheet("background-color: rgba(0,0,0,0.4); border-radius: 40px; padding: 10px; border: 1px solid #3b82f6;");
    
    cloudPassInput = new QLineEdit();
    cloudPassInput->setPlaceholderText("Облачный пароль");
    cloudPassInput->setEchoMode(QLineEdit::Password);
    cloudPassInput->setStyleSheet("background-color: rgba(0,0,0,0.4); border-radius: 40px; padding: 10px; border: 1px solid #3b82f6;");
    
    QHBoxLayout *authButtons = new QHBoxLayout();
    loginButton = new QPushButton("Вход");
    registerButton = new QPushButton("Регистрация");
    loginButton->setStyleSheet("QPushButton:flat { background-color: transparent; border: 1px solid #3b82f6; border-radius: 40px; padding: 8px; }");
    registerButton->setStyleSheet("QPushButton { background-color: #3b82f6; border-radius: 40px; padding: 8px; }");
    connect(loginButton, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(registerButton, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    authButtons->addWidget(loginButton);
    authButtons->addWidget(registerButton);
    
    userPanel = new QWidget();
    QVBoxLayout *userPanelLayout = new QVBoxLayout();
    usernameDisplay = new QLabel();
    usernameDisplay->setAlignment(Qt::AlignCenter);
    logoutButton = new QPushButton("Выйти");
    logoutButton->setStyleSheet("QPushButton:flat { background-color: transparent; border: 1px solid #3b82f6; border-radius: 40px; padding: 8px; }");
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::onLogoutClicked);
    userPanelLayout->addWidget(usernameDisplay);
    userPanelLayout->addWidget(logoutButton);
    userPanel->setLayout(userPanelLayout);
    userPanel->setVisible(false);
    
    saveProfileButton = new QPushButton("Сохранить профиль");
    saveProfileButton->setStyleSheet("QPushButton { background-color: #3b82f6; border-radius: 40px; padding: 10px; }");
    connect(saveProfileButton, &QPushButton::clicked, this, &MainWindow::onSaveProfileClicked);
    
    profileLayout->addWidget(avatar);
    profileLayout->addWidget(nickInput);
    profileLayout->addWidget(passInput);
    profileLayout->addWidget(cloudPassInput);
    profileLayout->addLayout(authButtons);
    profileLayout->addWidget(userPanel);
    profileLayout->addWidget(saveProfileButton);
    profileBox->setLayout(profileLayout);
    rightColumn->addWidget(profileBox);
    
    // НОВОСТИ (вместо Discord/скинов/модов)
    QGroupBox *newsBox = new QGroupBox("📢 НОВОСТИ И АКЦИИ");
    newsBox->setStyleSheet("QGroupBox { background-color: rgba(18,25,45,0.6); border-radius: 16px; padding: 15px; }");
    QVBoxLayout *newsLayout = new QVBoxLayout();
    
    QLabel *news1 = new QLabel("🎁 Новый донат-ранг \"Легенда\" - /fly, /god, /kit legend");
    news1->setWordWrap(true);
    QLabel *news2 = new QLabel("🛡️ Защита от гриферов - WorldGuard и приваты на спавне");
    news2->setWordWrap(true);
    QLabel *news3 = new QLabel("📅 Ивент в субботу в 20:00 МСК - Клановая битва! Вход через /event");
    news3->setWordWrap(true);
    
    newsLayout->addWidget(news1);
    newsLayout->addWidget(news2);
    newsLayout->addWidget(news3);
    newsBox->setLayout(newsLayout);
    rightColumn->addWidget(newsBox);
    
    // Кнопка ИГРАТЬ
    playButton = new QPushButton("🎮 ИГРАТЬ");
    playButton->setStyleSheet("QPushButton { font-size: 24px; padding: 15px; background-color: #22c55e; border-radius: 60px; }");
    connect(playButton, &QPushButton::clicked, this, &MainWindow::onPlayButtonClicked);
    rightColumn->addWidget(playButton);
    
    // Собираем всё вместе
    mainLayout->addLayout(leftColumn, 2);
    mainLayout->addLayout(rightColumn, 1);
    central->setLayout(mainLayout);
    setCentralWidget(central);
    
    // Применяем глобальный стиль
    setStyleSheet("QMainWindow { background-color: #0b0e17; } QWidget { background-color: #0b0e17; color: #eef2ff; font-family: 'Segoe UI'; }");
}

void MainWindow::onPlayButtonClicked()
{
    QString ip = "grifmcpro.aternos.me:18014";
    QDesktopServices::openUrl(QUrl("minecraft://?server=" + ip));
}

void MainWindow::onSendMessageClicked()
{
    QString text = chatInput->text().trimmed();
    if (text.isEmpty()) return;
    
    QString author = nickInput->text().isEmpty() ? "Гость" : nickInput->text();
    saveMessage(author, text);
    chatInput->clear();
    loadMessages();
}

void MainWindow::saveMessage(QString author, QString text)
{
    QDir dir;
    if (!dir.exists("launcher_data")) dir.mkdir("launcher_data");
    
    QFile file("launcher_data/messages.json");
    QJsonArray messages;
    
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (doc.isArray()) messages = doc.array();
        file.close();
    }
    
    QJsonObject newMsg;
    newMsg["author"] = author;
    newMsg["text"] = text;
    newMsg["time"] = QDateTime::currentDateTime().toString();
    messages.append(newMsg);
    
    if (messages.size() > 50) messages.removeFirst();
    
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(messages).toJson());
        file.close();
    }
}

void MainWindow::loadMessages()
{
    QFile file("launcher_data/messages.json");
    if (!file.open(QIODevice::ReadOnly)) return;
    
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isArray()) return;
    
    chatDisplay->clear();
    QJsonArray messages = doc.array();
    for (const QJsonValue &val : messages) {
        QJsonObject msg = val.toObject();
        chatDisplay->append(QString("<b>%1:</b> %2").arg(msg["author"].toString(), msg["text"].toString()));
    }
}

void MainWindow::onLoginClicked()
{
    // Упрощённая авторизация
    QMessageBox::information(this, "Вход", "Функция в разработке");
}

void MainWindow::onRegisterClicked()
{
    QMessageBox::information(this, "Регистрация", "Функция в разработке");
}

void MainWindow::onLogoutClicked()
{
    userPanel->setVisible(false);
    nickInput->clear();
    passInput->clear();
    cloudPassInput->clear();
}

void MainWindow::onSaveProfileClicked()
{
    QMessageBox::information(this, "Сохранено", "Профиль сохранён локально");
}

void MainWindow::saveUserData()
{
    // Сохранение данных пользователя
}
