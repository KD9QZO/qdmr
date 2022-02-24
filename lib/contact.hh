#ifndef CONTACT_HH_
#define CONTACT_HH_

#include "configobject.hh"

#include <QVector>
#include <QAbstractTableModel>



class Config;

/**
 * \brief Represents the base-class for all contact types, Analog (DTMF) or Digital (DMR).
 *
 * \ingroup conf
 */
class Contact: public ConfigObject {
	Q_OBJECT

	/*! \brief The name of the contact. */
	Q_PROPERTY(QString name READ name WRITE setName)

	/*! \brief If \c true and supported by radio, ring on call from this contact. */
	Q_PROPERTY(bool ring READ ring WRITE setRing)

protected:
	/**
	 * \brief Hidden constructor
	 *
	 * \param	name	Specifies the name of the contact.
	 * \param	ring	Specifies whether a ring-tone for this contact is used.
	 * \param	parent	Specifies the QObject parent.
	 */
	Contact(const QString &name, bool ring = true, QObject *parent = nullptr);

public:
	/*! \brief Returns the name of the contact. */
	const QString &name() const;

	/*! \brief (Re)sets the name of the contact. */
	bool setName(const QString &name);

	/*! \brief Returns \c true if the ring-tone is enabled for this contact. */
	bool ring() const;

	/*! \brief Enables/disables the ring-tone for this contact. */
	void setRing(bool enable);

	/**
	 * \brief Typecheck contact
	 *
	 * For example, \c contact->is<DigitalContact>() returns \c true if \c contact is a \c DigitalContact
	 */
	template <class T> bool is() const {
		return (0 != dynamic_cast<const T*>(this));
	}

	/*! \brief Typecast contact. */
	template<class T> T *as() {
		return (dynamic_cast<T*>(this));
	}

	/*! \brief Typecast contact. */
	template<class T> const T *as() const {
		return (dynamic_cast<const T*>(this));
	}

protected:
	/*! \brief Contact name. */
	QString _name;

	/*! \brief Ringtone enabled? */
	bool _ring;
};


/**
 * \brief Represents an analog contact, that is a DTMF number.
 *
 * \ingroup conf
 */
class DTMFContact: public Contact {
	Q_OBJECT

	/*! \brief The contact number. */
	Q_PROPERTY(QString number READ number WRITE setNumber)

public:
	/**
	 * \brief Constructs a DTMF (analog) contact
	 *
	 * \param	name	Specifies the contact name
	 * \param	number	Specifies the DTMF number (\c 0-9, \c A, \c B, \c C, \c D, \c *, \c # are the valid digits)
	 * \param	ring	Specifies whether the ring-tone is enabled for this contact
	 * \param	parent	Specifies the QObject parent
	 */
	DTMFContact(const QString &name, const QString &number, bool ring = false, QObject *parent = nullptr);

	YAML::Node serialize(const Context &context);

	/**
	 * \brief Returns the DTMF number of this contact
	 *
	 * \note The number must consist of:
	 *       - \c 0-9
	 *       - \c A
	 *       - \c B
	 *       - \c C
	 *       - \c D
	 *       - \c *
	 *       - \c #
	 */
	const QString &number() const;

	/*! \brief (Re-)Sets the DTMF number of this contact. */
	bool setNumber(const QString &number);

protected:
	/*! \brief The DTMF number. */
	QString _number;
};


/**
 * \brief Represents a digital contact, that is a DMR number.
 *
 * \ingroup conf
 */
class DigitalContact: public Contact {
	Q_OBJECT

	/*! \brief The type of the contact. */
	Q_PROPERTY(Type type READ type WRITE setType)

	/*! \brief The number of the contact. */
	Q_PROPERTY(unsigned number READ number WRITE setNumber)

public:
	/*! \brief Possible call types for a contact. */
	typedef enum {
		PrivateCall,	/*!< A private call */
		GroupCall,		/*!< A group call */
		AllCall			/*!< An all-call */
	} Type;
	Q_ENUM(Type)

public:
	/**
	 * \brief Constructs a DMR (digital) contact
	 *
	 * \param	type	Specifies the call type (private, group, all-call).
	 * \param	name	Specifies the contact name.
	 * \param	number	Specifies the DMR number for this contact.
	 * \param	ring	Specifies whether the ring-tone is enabled for this contact.
	 * \param	parent	Specifies the QObject parent. */
	DigitalContact(Type type, const QString &name, unsigned number, bool ring=false, QObject *parent=nullptr);

	YAML::Node serialize(const Context &context);

	/*! \brief Returns the call-type. */
	Type type() const;

	/*! \brief (Re-)Sets the call-type. */
	void setType(Type type);

	/*! \brief Returns the DMR number. */
	unsigned number() const;

	/*! \brief (Re-)Sets the DMR number of the contact. */
	bool setNumber(unsigned number);

protected:
	/*! \brief The call type. */
	Type _type;

	/*! \brief The DMR number of the contact. */
	unsigned _number;
};


/**
 * \brief Represents the list of contacts within the abstract radio configuration.
 *
 * A special feature of this list, is that \b DTMF and \b digital contacts can be accessed by their own unique index,
 * although they are held within this single list. Most radios manage digital and DTMF contacts in \e separate lists,
 * hence a means to iterate over and get indices of digital and DTMF contacts only is needed.
 *
 * This class implements the \c QAbstractTableModel, such that the list can be shown with the \c QTableView widget.
 *
 * \ingroup conf
 */
class ContactList: public ConfigObjectList {
	Q_OBJECT

public:
	/*! \brief Constructs an empty contact list. */
	explicit ContactList(QObject *parent = nullptr);

	int add(ConfigObject *obj, int row = -1);

	/*! \brief Returns the number of digital contacts. */
	int digitalCount() const;

	/*! \brief Returns the number of DTMF contacts. */
	int dtmfCount() const;

	/*! \brief Returns the contact at index \c idx. */
	Contact *contact(int idx) const;

	/*! \brief Returns the digital contact at index \c idx among digital contacts. */
	DigitalContact *digitalContact(int idx) const;

	/*! \brief Searches for a digital contact with the given number. */
	DigitalContact *findDigitalContact(unsigned number) const;

	/*! \brief Returns the DTMF contact at index \c idx among DTMF contacts. */
	DTMFContact *dtmfContact(int idx) const;

	/*! \brief Returns the index of the given digital contact within digital contacts. */
	int indexOfDigital(DigitalContact *contact) const;

	/*! \brief Returns the index of the given DTMF contact within DTMF contacts. */
	int indexOfDTMF(DTMFContact *contact) const;
};



#endif	/* !CONTACT_HH_ */
