#ifndef GS_MATTERHORN_PARSINGUTILITIES_H
#define GS_MATTERHORN_PARSINGUTILITIES_H

/**
 * Parses a sequence of byte to extract a 8-bit numeric type.
 * This function does not perform any check on its inputs. It is left for the
 * user to guarantee that the iterator will return at least one value.
 *
 * @tparam T The numeric type parameter
 * @param it An iterator on the byte sequence
 * @return A 16-bit numeric value of type T
 */
template<typename T>
static T parse8(vector<uint8_t>::iterator &it) {
    return static_cast<T>(*(it++));
}

/**
 * Parses a sequence of byte to extract a 16-bit numeric type.
 * This function does not perform any check on its inputs. It is left for the
 * user to guarantee that the iterator will return at least two values.
 *
 * @tparam T The numeric type parameter
 * @param it An iterator on the byte sequence
 * @return A 16-bit numeric value of type T
 */
template<typename T>
static T parse16(vector<uint8_t>::iterator &it) {
    return static_cast<T>(*(it++) << 8) | *(it++);
}

/**
 * Parses a sequence of byte to extract a 16-bit numeric type.
 * This function does not perform any check on its inputs. It is left for the
 * user to guarantee that the iterator will return at least four values.
 *
 * @tparam T The numeric type parameter
 * @param it An iterator on the byte sequence
 * @return A 32-bit numeric value of type T
 */
template<typename T>
static T parse32(vector<uint8_t>::iterator &it) {
    return static_cast<T>(*(it++) << 24) | (*(it++) << 16) | (*(it++) << 8) | *(it++);
}

#endif //GS_MATTERHORN_PARSINGUTILITIES_H
