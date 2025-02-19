#include "digitalchanneldialog.hh"
#include "application.hh"
#include <QCompleter>
#include "rxgrouplistdialog.hh"
#include "repeaterdatabase.hh"
#include "extensionwrapper.hh"
#include "propertydelegate.hh"
#include "settings.hh"
#include "utils.hh"


/* ********************************************************************************************* *
 * Implementation of DigitalChannelDialog
 * ********************************************************************************************* */
DigitalChannelDialog::DigitalChannelDialog(Config *config, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(new DigitalChannel(this)), _channel(nullptr)
{
  construct();
}

DigitalChannelDialog::DigitalChannelDialog(Config *config, DigitalChannel *channel, QWidget *parent)
  : QDialog(parent), _config(config), _myChannel(nullptr), _channel(channel)
{
  if (_channel) {
    _myChannel = _channel->clone()->as<DigitalChannel>();
    _myChannel->setParent(parent);
  }
  construct();
}

void
DigitalChannelDialog::construct() {
  setupUi(this);
  Settings settings;

  Application *app = qobject_cast<Application *>(qApp);
  DMRRepeaterFilter *filter = new DMRRepeaterFilter(this);
  filter->setSourceModel(app->repeater());
  QCompleter *completer = new QCompleter(filter, this);
  completer->setCaseSensitivity(Qt::CaseInsensitive);
  completer->setCompletionColumn(0);
  completer->setCompletionRole(Qt::EditRole);
  channelName->setCompleter(completer);
  connect(completer, SIGNAL(activated(const QModelIndex &)),
          this, SLOT(onRepeaterSelected(const QModelIndex &)));

  rxFrequency->setValidator(new QDoubleValidator(0,500,5));
  txFrequency->setValidator(new QDoubleValidator(0,500,5));
  powerValue->setItemData(0, unsigned(Channel::Power::Max));
  powerValue->setItemData(1, unsigned(Channel::Power::High));
  powerValue->setItemData(2, unsigned(Channel::Power::Mid));
  powerValue->setItemData(3, unsigned(Channel::Power::Low));
  powerValue->setItemData(4, unsigned(Channel::Power::Min));
  powerDefault->setChecked(true); powerValue->setCurrentIndex(1); powerValue->setEnabled(false);
  totDefault->setChecked(true); totValue->setValue(0); totValue->setEnabled(false);
  scanList->addItem(tr("[None]"), QVariant::fromValue((ScanList *)(nullptr)));
  scanList->setCurrentIndex(0);
  for (int i=0; i<_config->scanlists()->count(); i++) {
    scanList->addItem(_config->scanlists()->scanlist(i)->name(),
                      QVariant::fromValue(_config->scanlists()->scanlist(i)));
    if (_myChannel && (_myChannel->scanList() == _config->scanlists()->scanlist(i)) )
      scanList->setCurrentIndex(i+1);
  }
  txAdmit->setItemData(0, unsigned(DigitalChannel::Admit::Always));
  txAdmit->setItemData(1, unsigned(DigitalChannel::Admit::Free));
  txAdmit->setItemData(2, unsigned(DigitalChannel::Admit::ColorCode));
  timeSlot->setItemData(0, unsigned(DigitalChannel::TimeSlot::TS1));
  timeSlot->setItemData(1, unsigned(DigitalChannel::TimeSlot::TS2));
  populateRXGroupListBox(rxGroupList, _config->rxGroupLists(),
                         (nullptr != _myChannel ? _myChannel->groupListObj() : nullptr));
  txContact->addItem(tr("[None]"), QVariant::fromValue(nullptr));
  if (_myChannel && (nullptr == _myChannel->txContactObj()))
    txContact->setCurrentIndex(0);
  for (int i=0; i<_config->contacts()->count(); i++) {
    txContact->addItem(_config->contacts()->contact(i)->name(),
                       QVariant::fromValue(_config->contacts()->contact(i)));
    if (_myChannel && (_myChannel->txContactObj() == _config->contacts()->contact(i)) )
      txContact->setCurrentIndex(i+1);
  }
  gpsSystem->addItem(tr("[None]"), QVariant::fromValue((GPSSystem *)nullptr));
  for (int i=0; i<_config->posSystems()->count(); i++) {
    PositioningSystem *sys = _config->posSystems()->system(i);
    gpsSystem->addItem(sys->name(), QVariant::fromValue(sys));
    if (_myChannel && (_myChannel->aprsObj() == sys))
      gpsSystem->setCurrentIndex(i+1);
  }
  roaming->addItem(tr("[None]"), QVariant::fromValue((RoamingZone *)nullptr));
  roaming->addItem(tr("[Default]"), QVariant::fromValue(DefaultRoamingZone::get()));
  if (_myChannel && (_myChannel->roamingZone() == DefaultRoamingZone::get()))
    roaming->setCurrentIndex(1);
  for (int i=0; i<_config->roaming()->count(); i++) {
    RoamingZone *zone = _config->roaming()->zone(i);
    roaming->addItem(zone->name(), QVariant::fromValue(zone));
    if (_myChannel && (_myChannel->roamingZone() == zone))
      roaming->setCurrentIndex(i+2);
  }
  dmrID->addItem(tr("[Default]"), QVariant::fromValue(DefaultRadioID::get()));
  dmrID->setCurrentIndex(0);
  for (int i=0; i<_config->radioIDs()->count(); i++) {
    dmrID->addItem(_config->radioIDs()->getId(i)->name(),
                   QVariant::fromValue(_config->radioIDs()->getId(i)));
    if (_myChannel && (_config->radioIDs()->getId(i) == _myChannel->radioIdObj())) {
      dmrID->setCurrentIndex(i+1);
    }
  }
  voxDefault->setChecked(true); voxValue->setValue(0); voxValue->setEnabled(false);

  channelName->setText(_myChannel->name());
  rxFrequency->setText(format_frequency(_myChannel->rxFrequency()));
  txFrequency->setText(format_frequency(_myChannel->txFrequency()));
  if (! _myChannel->defaultPower()) {
    powerDefault->setChecked(false); powerValue->setEnabled(true);
    switch (_myChannel->power()) {
    case Channel::Power::Max: powerValue->setCurrentIndex(0); break;
    case Channel::Power::High: powerValue->setCurrentIndex(1); break;
    case Channel::Power::Mid: powerValue->setCurrentIndex(2); break;
      case Channel::Power::Low: powerValue->setCurrentIndex(3); break;
    case Channel::Power::Min: powerValue->setCurrentIndex(4); break;
    }
  }
  if (! _myChannel->defaultTimeout()) {
    totDefault->setChecked(false); totValue->setEnabled(true);
    totValue->setValue(_channel->timeout());
  }
  rxOnly->setChecked(_myChannel->rxOnly());
  switch (_myChannel->admit()) {
  case DigitalChannel::Admit::Always: txAdmit->setCurrentIndex(0); break;
  case DigitalChannel::Admit::Free: txAdmit->setCurrentIndex(1); break;
  case DigitalChannel::Admit::ColorCode: txAdmit->setCurrentIndex(2); break;
  }
  colorCode->setValue(_myChannel->colorCode());
  if (DigitalChannel::TimeSlot::TS1 == _myChannel->timeSlot())
    timeSlot->setCurrentIndex(0);
  else if (DigitalChannel::TimeSlot::TS2 == _myChannel->timeSlot())
    timeSlot->setCurrentIndex(1);
  if (! _myChannel->defaultVOX()) {
    voxDefault->setChecked(false); voxValue->setEnabled(true);
    voxValue->setValue(_channel->vox());
  }

  extensionView->setObjectName("digitalChannelExtension");
  extensionView->setObject(_myChannel, _config);

  if (! settings.showExtensions())
    tabWidget->tabBar()->hide();

  connect(powerDefault, SIGNAL(toggled(bool)), this, SLOT(onPowerDefaultToggled(bool)));
  connect(totDefault, SIGNAL(toggled(bool)), this, SLOT(onTimeoutDefaultToggled(bool)));
  connect(voxDefault, SIGNAL(toggled(bool)), this, SLOT(onVOXDefaultToggled(bool)));
}

DigitalChannel *
DigitalChannelDialog::channel()
{
  _myChannel->setRadioIdObj(dmrID->currentData().value<DMRRadioID*>());
  _myChannel->setName(channelName->text());
  _myChannel->setRXFrequency(rxFrequency->text().toDouble());
  _myChannel->setTXFrequency(txFrequency->text().toDouble());
  if (powerDefault->isChecked())
    _myChannel->setDefaultPower();
  else
    _myChannel->setPower(Channel::Power(powerValue->currentData().toUInt()));
  if (totDefault->isChecked())
    _myChannel->setDefaultTimeout();
  else
    _myChannel->setTimeout(totValue->value());
  _myChannel->setRXOnly(rxOnly->isChecked());
  _myChannel->setScanList(scanList->currentData().value<ScanList *>());
  _myChannel->setAdmit(DigitalChannel::Admit(txAdmit->currentData().toUInt()));
  _myChannel->setColorCode(colorCode->value());
  _myChannel->setTimeSlot(DigitalChannel::TimeSlot(timeSlot->currentData().toUInt()));
  _myChannel->setGroupListObj(rxGroupList->currentData().value<RXGroupList *>());
  _myChannel->setTXContactObj(txContact->currentData().value<DigitalContact *>());
  _myChannel->setAPRSObj(gpsSystem->currentData().value<PositioningSystem *>());
  _myChannel->setRoamingZone(roaming->currentData().value<RoamingZone *>());
  if (voxDefault->isChecked())
    _myChannel->setVOXDefault();
  else
    _myChannel->setVOX(voxValue->value());

  DigitalChannel *channel = _myChannel;
  if (nullptr == _channel) {
    _myChannel->setParent(nullptr);
    _myChannel = nullptr;
  } else {
    _channel->copy(*_myChannel);
    channel = _channel;
  }

  return channel;
}

void
DigitalChannelDialog::onRepeaterSelected(const QModelIndex &index) {
  Application *app = qobject_cast<Application *>(qApp);

  QModelIndex src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->completionModel())->mapToSource(index);
  src = qobject_cast<QAbstractProxyModel*>(
        channelName->completer()->model())->mapToSource(src);
  double rx = app->repeater()->repeater(src.row()).value("tx").toDouble();
  double tx = app->repeater()->repeater(src.row()).value("rx").toDouble();
  txFrequency->setText(QString::number(tx, 'f'));
  rxFrequency->setText(QString::number(rx, 'f'));
}

void
DigitalChannelDialog::onPowerDefaultToggled(bool checked) {
  powerValue->setEnabled(!checked);
}

void
DigitalChannelDialog::onTimeoutDefaultToggled(bool checked) {
  totValue->setEnabled(!checked);
}

void
DigitalChannelDialog::onVOXDefaultToggled(bool checked) {
  voxValue->setEnabled(! checked);
}

