/**
 * @author MIC Lab Team
 * @brief Bit manipulation macros
 *
 */

#ifndef bit_h_tie8AeRuf7unoh7l
#define bit_h_tie8AeRuf7unoh7l

/** @return a word with only bit-th bit set. */
#define BIT(bit) (1ull << (bit))
/** Raise bit-th bit in word. */
#define BIT_SET(word, bit) ((word) |= BIT(bit))
/** Clear bit-th bit in word. */
#define BIT_CLR(word, bit) ((word) &= ~BIT(bit))
/** Toggle the bit-th bit in word. */
#define BIT_TOGGLE(word, bit) ((word) ^= BIT(bit))
/** Mask out given bit in word. */
#define BIT_MASK(word, bit) ((word) & BIT(bit))

/** @return value "1" if bit-th bit of word is set and zero otherwise. */
#define BIT_IS_SET(word, bit) (!!BIT_MASK(word, bit))

/** Set bith-th bit in word to given value. */
#define BIT_ASSIGN(word, bit, value) ((value) ? BIT_SET(word, bit) : BIT_CLR(word, bit))

#endif
