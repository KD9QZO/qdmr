#ifndef CHANNEL_HH_
#define CHANNEL_HH_

#include <QObject>
#include <QAbstractTableModel>

#include "configobject.hh"
#include "configreference.hh"
#include "signaling.hh"



class Config;
class RXGroupList;
class DigitalContact;
class ScanList;
class APRSSystem;
class PositioningSystem;
class RoamingZone;
class RadioID;


/**
 * \brief The base class of all channels (analog and digital) of a codeplug configuration.
 *
 * This class holds the common configuration of \c AnalogChannel and \c DigitalChannel, that is the name, both the \b RX
 * and the \b TX frequencies, output power, the \b TOT and default scanlist properties.
 *
 * \ingroup conf
 */
class Channel: public ConfigObject {
	Q_OBJECT

	/*! \brief The name of the channel. */
	Q_PROPERTY(QString name READ name WRITE setName)
	/*! \brief The receive frequency of the channel. */
	Q_PROPERTY(double rxFrequency READ rxFrequency WRITE setRXFrequency)
	/*! \brief The transmit frequency of the channel. */
	Q_PROPERTY(double txFrequency READ txFrequency WRITE setTXFrequency)

#if 0
	/*! \brief The transmit power. */
	Q_PROPERTY(Power power READ power WRITE setPower)

	/*! \brief The transmit timeout in seconds. */
	Q_PROPERTY(unsigned timeout READ timeout WRITE setTimeout)
#endif

	/*! \brief If true, the channel is receive only. */
	Q_PROPERTY(bool rxOnly READ rxOnly WRITE setRXOnly)
	/*! \brief The scan list. */
	Q_PROPERTY(ScanListReference* scanList READ scanList)

#if 0
	/*! \brief The VOX setting. */
	Q_PROPERTY(unsigned vox READ vox WRITE setVOX)
#endif

public:
	/*! \brief Possible power settings. */
	enum class Power {
		Max,		/*!< Highest power setting (e.g. > 5W, if available). */
		High,		/*!< High power setting (e.g, 5W). */
		Mid,		/*!< Medium power setting (e.g., 2W, if avaliable) */
		Low,		/*!< Low power setting (e.g., 1W). */
		Min		/*!< Lowest power setting (e.g., <1W, if available). */
	};
	Q_ENUM(Power)

protected:
  /*! \brief Hidden constructor.
   * Constructs a new empty channel. */
  explicit Channel(QObject *parent=nullptr);
  /*! \brief Copy constructor. */
  Channel(const Channel &other, QObject *parent=nullptr);

public:
  /** Returns \c true if the channel is of type \c T. This can be used to text wheter this channel
   * is actuially an analog or digital channel: \c channel->is<AnalogChannel>(). */
  template<class T>
  bool is() const {
	  return 0 != dynamic_cast<const T *>(this);
  }

  /*! \brief Dynamic cast of channels. For example \c channel->as<AnalogChannel>(). */
  template<class T>
  T *as() {
	  return dynamic_cast<T *>(this);
  }

  /*! \brief Dynamic cast of channels. For example \c channel->as<AnalogChannel>(). */
  template<class T>
  const T *as() const{
	  return dynamic_cast<const T *>(this);
  }

  /*! \brief Returns the name of the channel. */
  const QString &name() const;
  /*! \brief (Re-)Sets the name of the channel. */
  bool setName(const QString &name);

  /*! \brief Returns the RX frequency of the channel in MHz. */
  double rxFrequency() const;
  /*! \brief (Re-)Sets the RX frequency of the channel in MHz. */
  bool setRXFrequency(double freq);
  /*! \brief Returns the TX frequency of the channel in MHz. */
  double txFrequency() const;
  /*! \brief (Re-)Sets the TX frequency of the channel in MHz. */
  bool setTXFrequency(double freq);

  /*! \brief Retunrs \c true if the channel uses the global default power setting. */
  bool defaultPower() const;
  /*! \brief Returns the power setting of the channel if the channel does not use the default power. */
  Power power() const;
  /*! \brief (Re-)Sets the power setting of the channel, overrides default power. */
  void setPower(Power power);
  /*! \brief Sets the channel to use the default power setting. */
  void setDefaultPower();

  /*! \brief Returns \c true if the transmit timeout is specified by the global default value. */
  bool defaultTimeout() const;
  /*! \brief Returns \c true if the transmit timeout is disabled. */
  bool timeoutDisabled() const;
  /*! \brief Returns the TX timeout (TOT) in seconds. */
  unsigned timeout() const;
  /*! \brief (Re-)Sets the TX timeout (TOT) in seconds. */
  bool setTimeout(unsigned dur);
  /*! \brief Disables the transmit timeout. */
  void disableTimeout();
  /*! \brief Sets the timeout to the global default timeout. */
  void setDefaultTimeout();

  /*! \brief Returns \c true, if the channel is RX only. */
  bool rxOnly() const;
  /*! \brief Set, whether the channel is RX only. */
  bool setRXOnly(bool enable);

  /*! \brief Returns \c true if the VOX is disabled. */
  bool voxDisabled() const;
  /*! \brief Retunrs \c true if the VOX is specified by the global default value. */
  bool defaultVOX() const;
  /*! \brief Returns the VOX level [0-10]. */
  unsigned vox() const;
  /*! \brief Sets the VOX level [0-10]. */
  void setVOX(unsigned level);
  /*! \brief Sets the VOX level to the default value. */
  void setVOXDefault();
  /*! \brief Disables the VOX. */
  void disableVOX();

  /*! \brief Returns the reference to the scan list. */
  const ScanListReference *scanList() const;
  /*! \brief Returns the reference to the scan list. */
  ScanListReference *scanList();

  /*! \brief Returns the default scan list for the channel. */
  ScanList *scanListObj() const;
  /*! \brief (Re-) Sets the default scan list for the channel. */
  bool setScanListObj(ScanList *list);

protected:
  bool populate(YAML::Node &node, const Context &context);

protected slots:
  /*! \brief Gets called whenever a referenced object is changed or deleted. */
  void onReferenceModified();

protected:
  /*! \brief The channel name. */
  QString _name;
  /*! \brief The RX frequency in MHz. */
  double _rxFreq;
  /*! \brief The TX frequency in MHz. */
  double _txFreq;
  /*! \brief If \c true, the channel uses the global power setting. */
  bool _defaultPower;
  /*! \brief The transmit power setting. */
  Power _power;
  /*! \brief Transmit timeout in seconds. */
  unsigned _txTimeOut;
  /*! \brief RX only flag. */
  bool _rxOnly;
  /*! \brief Holds the VOX level. */
  unsigned _vox;
  /*! \brief Default scan list of the channel. */
  ScanListReference _scanlist;
};


/** Extension to the \c Channel class to implement an analog channel.
 *
 * This class implements all the properties specific to an analog channel. That is, the admit
 * criterion, squelch, RX and TX tones and bandwidth settings.
 *
 * \ingroup conf */
class AnalogChannel: public Channel
{
  Q_OBJECT

  /*! \brief The admit criterion of the channel. */
  Q_PROPERTY(Admit admit READ admit WRITE setAdmit)
  // /*! \brief The squelch level of the channel [1-10]. */
  // Q_PROPERTY(unsigned squelch READ squelch WRITE setSquelch)
  /*! \brief The band width of the channel. */
  Q_PROPERTY(Bandwidth bandwidth READ bandwidth WRITE setBandwidth)
  /*! \brief The APRS system. */
  Q_PROPERTY(APRSSystemReference* aprs READ aprs)

public:
  /*! \brief Admit criteria of analog channel. */
  enum class Admit {
    Always,		/*!< Allow always. */
    Free,		/*!< Allow when channel free. */
    Tone		/*!< Allow when admit tone is present. */
  };
  Q_ENUM(Admit)

  /*! \brief Possible bandwidth of an analog channel. */
  enum class Bandwidth {
    Narrow,		/*!< Narrow bandwidth (12.5kHz). */
    Wide		/*!< Wide bandwidth (25kHz). */
  };
  Q_ENUM(Bandwidth)

public:
  /*! \brief Constructs a new empty analog channel. */
  explicit AnalogChannel(QObject *parent=nullptr);
  /*! \brief Copy constructor. */
  AnalogChannel(const AnalogChannel &other, QObject *parent=nullptr);

  YAML::Node serialize(const Context &context);

  /*! \brief Returns the admit criterion for the analog channel. */
	Admit admit() const;
  /*! \brief (Re-)Sets the admit criterion for the analog channel. */
	void setAdmit(Admit admit);

  /*! \brief Returns \c true if the global default squelch level is used. */
  bool defaultSquelch() const;
  /*! \brief Returns \c true if the squelch is disabled. */
  bool squelchDisabled() const;
  /*! \brief Returns the squelch level [0,10]. */
	unsigned squelch() const;
  /*! \brief (Re-)Sets the squelch level [0,10]. 0 Disables squelch (on some radios). */
	bool setSquelch(unsigned squelch);
  /*! \brief Disables the quelch. */
  void disableSquelch();
  /*! \brief Sets the squelch to the global default value. */
  void setSquelchDefault();

  /*! \brief Returns the CTCSS/DCS RX tone, \c SIGNALING_NONE means disabled. */
  Signaling::Code rxTone() const;
  /*! \brief (Re-)Sets the CTCSS/DCS RX tone, \c SIGNALING_NONE disables the RX tone. */
  bool setRXTone(Signaling::Code code);
  /*! \brief Returns the CTCSS/DCS TX tone, \c SIGNALING_NONE means disabled. */
  Signaling::Code txTone() const;
  /*! \brief (Re-)Sets the CTCSS/DCS TX tone, \c SIGNALING_NONE disables the TX tone. */
  bool setTXTone(Signaling::Code code);

  /*! \brief Returns the bandwidth of the analog channel. */
	Bandwidth bandwidth() const;
  /*! \brief (Re-)Sets the bandwidth of the analog channel. */
	bool setBandwidth(Bandwidth bw);

  /*! \brief Returns the reference to the APRS system. */
  const APRSSystemReference *aprs() const;
  /*! \brief Returns the reference to the APRS system. */
  APRSSystemReference *aprs();
  /*! \brief Returns the APRS system used for this channel or \c nullptr if disabled. */
  APRSSystem *aprsSystem() const;
  /*! \brief Sets the APRS system. */
  void setAPRSSystem(APRSSystem *sys);

protected:
  bool populate(YAML::Node &node, const Context &context);

protected:
  /*! \brief Holds the admit criterion. */
	Admit _admit;
  /*! \brief Holds the squelch level [0,10]. */
  unsigned _squelch;
  /*! \brief The RX CTCSS tone. */
  Signaling::Code _rxTone;
  /*! \brief The TX CTCSS tone. */
  Signaling::Code _txTone;
  /*! \brief The channel bandwidth. */
	Bandwidth _bw;
  /*! \brief A reference to the APRS system used on the channel or \c nullptr if disabled. */
  APRSSystemReference _aprsSystem;
};



/** Extension to the \c Channel class to implement an digital (DMR) channel.
 *
 * That is, the admit criterion, color code, time slot, RX group list and TX contact.
 *
 * \ingroup conf */
class DigitalChannel: public Channel {
	Q_OBJECT

	/*! \brief The admit criterion of the channel. */
	Q_PROPERTY(Admit admit READ admit WRITE setAdmit)
	/*! \brief The color code of the channel. */
	Q_PROPERTY(unsigned colorCode READ colorCode WRITE setColorCode)
	/*! \brief The time slot of the channel. */
	Q_PROPERTY(TimeSlot timeSlot READ timeSlot WRITE setTimeSlot)
	/*! \brief The radio ID. */
	Q_PROPERTY(RadioIDReference* radioID READ radioID)
	/*! \brief The rx group list. */
	Q_PROPERTY(GroupListReference* groupList READ groupList)
	/*! \brief The tx contact. */
	Q_PROPERTY(DigitalContactReference* contact READ contact)
	/*! \brief The positioning system. */
	Q_PROPERTY(PositioningSystemReference* aprs READ aprs)
	/*! \brief The roaming zone. */
	Q_PROPERTY(RoamingZoneReference* roaming READ roaming)

public:
	/*! \brief Possible admit criteria of digital channels. */
	enum class Admit {
		Always,		/*!< No admit criteria, allows to transmit any time. */
		Free,		/*!< Transmit only if channel is free. */
		ColorCode	/*!< Transmit only if channel is free and matches given color code. */
	};
	Q_ENUM(Admit)

	/*! \brief Possible timeslots for digital channels. */
	enum class TimeSlot {
		TS1,		/*!< Time/repeater slot 1 */
		TS2			/*!< Time/repeater slot 2 */
	};
	Q_ENUM(TimeSlot)

public:
	/*! \brief Constructs a new empty digital (DMR) channel. */
	DigitalChannel(QObject *parent=nullptr);
	/*! \brief Copy constructor. */
	DigitalChannel(const DigitalChannel &other, QObject *parent=nullptr);

	YAML::Node serialize(const Context &context);

	/*! \brief Returns the admit criterion for the channel. */
	Admit admit() const;
	/*! \brief (Re-)Sets the admit criterion for the channel. */
	void setAdmit(Admit admit);

	/*! \brief Returns the color code for the channel. */
	unsigned colorCode() const;
	/*! \brief (Re-)Sets the color code for the channel. */
	bool setColorCode(unsigned cc);

	/*! \brief Returns the time slot for the channel. */
	TimeSlot timeSlot() const;
	/*! \brief (Re-)Sets the time slot for the channel. */
	bool setTimeSlot(TimeSlot ts);

	/*! \brief Returns a reference to the group list. */
	const GroupListReference *groupList() const;
	/*! \brief Returns a reference to the group list. */
	GroupListReference *groupList();
	/*! \brief Retruns the RX group list for the channel. */
	RXGroupList *groupListObj() const;
	/*! \brief (Re-)Sets the RX group list for the channel. */
	bool setGroupListObj(RXGroupList *rxg);

	/*! \brief Returns a reference to the transmit contact. */
	const DigitalContactReference *contact() const;
	/*! \brief Returns a reference to the transmit contact. */
	DigitalContactReference *contact();
	/*! \brief Returns the default TX contact to call on this channel. */
	DigitalContact *txContactObj() const;
	/*! \brief (Re-) Sets the default TX contact for this channel. */
	bool setTXContactObj(DigitalContact *c);

	/*! \brief Returns a reference to the positioning system. */
	const PositioningSystemReference *aprs() const;
	/*! \brief Returns a reference to the positioning system. */
	PositioningSystemReference *aprs();
	/*! \brief Returns the GPS system associated with this channel or \c nullptr if not set. */
	PositioningSystem *aprsObj() const;
	/*! \brief Associates the GPS System with this channel. */
	bool setAPRSObj(PositioningSystem *sys);

	/*! \brief Returns a reference to the roaming zone. */
	const RoamingZoneReference *roaming() const;
	/*! \brief Returns a reference to the roaming zone. */
	RoamingZoneReference *roaming();
	/*! \brief Returns the roaming zone associated with this channel or \c nullptr if not set. */
	RoamingZone *roamingZone() const;
	/*! \brief Associates the given roaming zone with this channel. */
	bool setRoamingZone(RoamingZone *zone);

	/*! \brief Returns the reference to the radio ID. */
	const RadioIDReference *radioID() const;
	/*! \brief Returns the reference to the radio ID. */
	RadioIDReference *radioID();
	/*! \brief Returns the radio ID associated with this channel or \c nullptr if the default ID is used. */
	RadioID *radioIdObj() const;
	/*! \brief Associates the given radio ID with this channel. Pass nullptr to set to default ID. */
	bool setRadioIdObj(RadioID *id);

protected:
	/*! \brief The admit criterion. */
	Admit _admit;

	/*! \brief The channel color code. */
	unsigned _colorCode;

	/*! \brief The time slot for the channel. */
	TimeSlot _timeSlot;

	/*! \brief The RX group list for this channel. */
	GroupListReference _rxGroup;

	/*! \brief The default TX contact. */
	DigitalContactReference _txContact;

	/*! \brief The GPS system. */
	PositioningSystemReference _posSystem;

	/*! \brief Roaming zone for the channel. */
	RoamingZoneReference _roaming;

	/*! \brief Radio ID to use on this channel. \c nullptr if default ID is used. */
	RadioIDReference _radioId;
};


/** Internal singleton class representing the "currently selected" channel.
 * \ingroup conf */
class SelectedChannel: public Channel {
	Q_OBJECT

protected:
	/**
	 * \brief Constructs the "selected" channel.
	 *
	 * \warning Do not use this class directly, call \c SelectedChannel::get() instead.
	 */
	explicit SelectedChannel();

public:
	/*! \brief Destructor. */
	virtual ~SelectedChannel();

	/*! \brief Constructs/gets the singleton instance. */
	static SelectedChannel *get();

protected:
	/*! \brief Holds the channel singleton instance. */
	static SelectedChannel *_instance;
};


/** Container class holding all channels (analog and digital) for a specific configuration
 * (\c Config).
 *
 * This class also implements the QAbstractTableModel and can therefore be displayed using a
 * default QTableView instance.
 *
 * \ingroup conf */
class ChannelList: public ConfigObjectList {
	Q_OBJECT

public:
	/*! \brief Constructs an empty channel list. */
	explicit ChannelList(QObject *parent = nullptr);

	int add(ConfigObject *obj, int row=-1);

	/*! \brief Gets the channel at the specified index. */
	Channel *channel(int idx) const;

	/*! \brief Finds a digial channel with the given frequencies, time slot and color code. */
	DigitalChannel *findDigitalChannel(double rx, double tx, DigitalChannel::TimeSlot ts, unsigned cc) const;

	/*! \brief Finds an analog channel with the given frequeny. */
	AnalogChannel *findAnalogChannelByTxFreq(double freq) const;
};


#endif	/* !CHANNEL_HH_ */
