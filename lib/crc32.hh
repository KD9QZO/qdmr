#ifndef CRC32_HH_
#define CRC32_HH_

#include <QByteArray>



/**
 * \brief Implements the CRC32 checksum
 *
 * \ingroup util
 */
class CRC32 {
public:
	/*! \brief Default constructor. */
	CRC32();

	/*! \brief Update CRC with given byte. */
	void update(uint8_t c);

	/*! \brief Update CRC with given data. */
	void update(const uint8_t *c, size_t n);

	/*! \brief Update CRC with given data. */
	void update(const QByteArray &data);

	/*! \brief Returns the current CRC. */
	inline uint32_t get() { return _crc; }

protected:
	/*! \brief Current CRC. */
	uint32_t _crc;
};


#endif	/* !CRC32_HH_ */
