#include "addressescolumn.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include <form/controls/controlutil.h>
#include <form/controls/plaintextedit.h>
#include <form/controls/zonesselector.h>
#include <manager/windowmanager.h>
#include <util/guiutil.h>
#include <util/ioc/ioccontainer.h>

AddressesColumn::AddressesColumn(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void AddressesColumn::retranslateUi()
{
    m_btSelectZones->retranslateUi();
}

void AddressesColumn::setupUi()
{
    // Header
    auto headerLayout = setupHeaderLayout();

    // Text Area
    m_editIpText = new PlainTextEdit();

    auto layout = ControlUtil::createVLayout();
    layout->addLayout(headerLayout);
    layout->addWidget(m_editIpText);

    this->setLayout(layout);
}

QLayout *AddressesColumn::setupHeaderLayout()
{
    m_labelTitle = ControlUtil::createLabel();
    m_labelTitle->setFont(GuiUtil::fontBold());

    // Use All
    m_cbUseAll = new QCheckBox();

    // Select Zones
    m_btSelectZones = new ZonesSelector();

    auto layout = new QHBoxLayout();
    layout->addWidget(m_labelTitle);
    layout->addStretch();
    layout->addWidget(m_cbUseAll);
    layout->addWidget(ControlUtil::createVSeparator());
    layout->addWidget(m_btSelectZones);

    return layout;
}
