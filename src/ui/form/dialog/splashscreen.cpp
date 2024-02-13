#include "splashscreen.h"

#include <QGuiApplication>
#include <QHBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QTimer>

#include <form/controls/controlutil.h>
#include <manager/windowmanager.h>
#include <util/iconcache.h>

SplashScreen::SplashScreen() : QSplashScreen()
{
    this->setAttribute(Qt::WA_DeleteOnClose);

    setupUi();
}

void SplashScreen::showTemporary()
{
    show();

    QTimer::singleShot(2000, this, &QWidget::close);
}

void SplashScreen::setupUi()
{
    // Main Layout
    auto layout = setupMainLayout();
    this->setLayout(layout);

    // Background Color
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(0x26, 0x26, 0x26));

    this->setAutoFillBackground(true);
    this->setPalette(palette);

    // Font
    this->setFont(WindowManager::defaultFont());

    // Icon
    this->setWindowIcon(IconCache::icon(":/icons/fort.png"));

    // Size
    this->resize(250, 80);
}

QLayout *SplashScreen::setupMainLayout()
{
    // Logo image
    auto logoIcon = createLogoIcon();

    // Logo text
    auto logoText = createLogoTextLayout();

    auto layout = new QHBoxLayout();
    layout->setContentsMargins(6, 6, 6, 6);
    layout->setSpacing(10);

    layout->addStretch();
    layout->addWidget(logoIcon);
    layout->addLayout(logoText);
    layout->addStretch();

    return layout;
}

QLabel *SplashScreen::createLogoIcon()
{
    auto iconLogo = ControlUtil::createLabel();

    iconLogo->setScaledContents(true);
    const QSize logoSize(48, 48);
    iconLogo->setMinimumSize(logoSize);
    iconLogo->setMaximumSize(logoSize);

    iconLogo->setPixmap(IconCache::file(":/icons/fort-96.png"));

    return iconLogo;
}

QLayout *SplashScreen::createLogoTextLayout()
{
    auto layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QPalette palette;
    palette.setColor(QPalette::WindowText, Qt::white);

    QFont font("Franklin Gothic", 22, QFont::Bold);

    // Label
    auto label = ControlUtil::createLabel(QGuiApplication::applicationName());
    label->setPalette(palette);
    label->setFont(font);

    // Sub-label
    font.setPointSize(10);
    font.setWeight(QFont::DemiBold);

    auto subLabel = ControlUtil::createLabel("- keeping you secure -");
    subLabel->setPalette(palette);
    subLabel->setFont(font);

    layout->addWidget(label, 2, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addWidget(subLabel, 1, Qt::AlignHCenter | Qt::AlignTop);

    return layout;
}