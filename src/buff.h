extern unsigned char* buff;

extern void initBuffer();
extern void fillRect(int x, int y, int width, int height);
extern void putStr(char* str, int x, int y);

// putCell: takes array of various type as input and write it to buffer
#define putCell(img, x, y)                           \
    {                                                \
        int height = sizeof(img[0]) * 8;             \
        int width = sizeof(img) / (height / 8);      \
                                                     \
        if (height == 8) {                           \
            uint8_t ln;                              \
            for (size_t i = 0; i < width; i++) {     \
                ln = pgm_read_byte_near(&(img[i]));  \
                for (size_t j = 0; j < 8; j++) {     \
                    if (ln & (1 << j)) {             \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        } else if (height == 16) {                   \
            uint16_t ln;                             \
            for (size_t i = 0; i < width; i++) {     \
                ln = pgm_read_word_near(&(img[i]));  \
                for (size_t j = 0; j < 16; j++) {    \
                    if (ln & (((uint16_t)1) << j)) { \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        } else if (height == 32) {                   \
            uint32_t ln;                             \
            for (size_t i = 0; i < width; i++) {     \
                ln = pgm_read_dword_near(&(img[i])); \
                for (size_t j = 0; j < 32; j++) {    \
                    if (ln & (((uint32_t)1) << j)) { \
                        dot(x + i, y + j);           \
                    }                                \
                }                                    \
            }                                        \
        }                                            \
    }
